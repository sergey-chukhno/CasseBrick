# Rendering Pipeline & Visual Effects

This manual defines the rendering architecture, visual order of operations, camera viewport transformations, draw call boundaries, caching models, and post-processing shader pipelines for Cyberpunk Cannon Shooter.

---

## PART A: Visual Aesthetics & Design Language

The game implements a retro-futuristic / cyberpunk visual identity:
* **Vibrant Neon Accents**: A color palette consisting of neon pink (`#ff006e`), neon cyan (`#00d9ff`), neon purple (`#9d4edd`), neon green (`#06ffa5`), and bright orange (`#FF8800`).
* **Angled UI Panels**: Convex geometry with cropped $45^\circ$ corners for buttons and window frames.
* **Deep Space Backdrop**: A moving starfield simulating depth via multi-plane parallax scrolling.

### 1. Multi-Pass CPU Glow Simulation
To support cross-platform compatibility across older OpenGL drivers without writing complex fragment shaders, the engine uses a **multi-pass CPU offset rendering method** to simulate glow.

#### Glow Offset Algorithm
For a given shape (e.g., text, rectangular button, or circular projectile), the engine renders $N$ outer concentric layers with decreasing opacities and outward offsets:

For each layer $i \in [1, L]$:
1. Calculate the layer opacity:
   $$\alpha_i = \alpha_{\text{base}} - (i \cdot \alpha_{\text{decrement}}) \cdot I_{\text{pulse}}$$
   Where $I_{\text{pulse}}$ is a pulsing animation coefficient ($0.4 \le I_{\text{pulse}} \le 1.0$) driven by a sine wave.
2. Determine the layer offset radius:
   $$R_i = i \cdot \text{stepSize}$$
3. Render the shape $8$ times in a radial layout, shifting the coordinates by:
   $$(\Delta x, \Delta y) \in \{ (0, \pm R_i), (\pm R_i, 0), (\pm R_i, \pm R_i) \}$$
4. Finally, render the core shape in full intensity at the base coordinates.

### 2. Particle System (Sparks & Debris)
The particle system handles short-lived visual effects (such as brick destruction sparks or thruster trails).
* **Storage**: Active particles are held in a double-ended queue (`std::deque<Particle>`) to allow fast insertion at the tail and extraction from the head.
* **Update**: Each frame, particles apply velocity, deceleration, and fade their color alpha values to zero.
* **Draw Call**: Rendered using standard SFML point primitives or vertex arrays to minimize draw submissions.

---

## PART B: Render Loop Order

To ensure correct visual depth and prevent elements from overlapping incorrectly (e.g., ensuring HUD elements draw on top of game objects), the frame rendering loop executes passes in a strict sequential order:

```
  [Frame Start]           ──► clears backbuffer (sf::RenderWindow::clear)
        │
        ▼
  [Background Pass]       ──► draws scrolling starfields and background grids
        │
        ▼
  [Gameplay Objects]      ──► draws Cannon, Projectiles, Bricks, and Bosses
        │
        ▼
  [Effects Pass]          ──► draws active particle systems, laser beams, and trails
        │
        ▼
  [Glow Pass]             ──► draws concentric CPU offsets for neon elements
        │
        ▼
  [HUD / UI Pass]         ──► draws score counters, combos, settings, and health text
        │
        ▼
  [Debug Overlay Pass]    ──► draws physics wireframes and stats overlays
        │
        ▼
  [Frame Present]         ──► swaps backbuffer (sf::RenderWindow::display)
```

### Debug Overlay Pass Content

The debug overlay pass runs strictly in development or non-distribution builds when activated by a developer hotkey. It outputs the following visualization aids directly to the screen:
* **Axis-Aligned Bounding Boxes (AABBs)**: Green bounding wireframes drawn around all active collidable entities (Cannon, Projectiles, Bricks, and Canisters) to verify collision shapes and spatial partitioning.
* **Collision Normals**: Red vector lines extending from active collision contact points to check reflection angle mathematics and normal vector directions.
* **FPS & Frame Time Counter**: Top-corner real-time performance tracker displaying both frames per second and raw CPU frame time (ms).
* **Active Entity Counts**: Real-time metrics tracking the number of active gameplay entities (such as bricks, active projectile objects, and dynamic canisters).
* **Target Particle Counts**: Performance monitoring counting the number of currently active particle emitters and total live sparks or debris particles in the deque.


---

## PART C: Render Layers

Visual elements are categorized into distinct render layers using a classification enumeration:
```cpp
enum class RenderLayer {
    Background,
    Gameplay,
    Effects,
    UI,
    Debug
};
```
During the render phase, instead of objects drawing themselves directly to the window context on-the-fly, views submit their draw primitives along with their target `RenderLayer` to the `RenderSystem` coordinator. The coordinator sorts these submissions and executes the draw calls sequentially.

---

## PART D: Camera & View System

To separate gameplay coordinates from static interface elements, the engine uses decoupled SFML view viewports (`sf::View`):

### 1. World View (`sf::View`)
* **Role**: Displays the active gameplay coordinates where physics simulation occurs.
* **Behavior**:
  * Can shift dynamically to apply screen shake effects (camera vibration offsets) during explosions or boss hits without moving the HUD.
  * Can scale its dimensions to apply time dilation slow-motion zooms (near-miss visual zooms) when a projectile passes close to a hazard.

### 2. UI View (`sf::View`)
* **Role**: Displays static interface layers.
* **Behavior**: Uses a static orthographic projection locked to the window's physical pixel dimensions. HUD meters, warning logs, pause menus, and combo counts are drawn here, remaining fixed during camera vibrations.

---

## PART E: Draw Call Ownership & Data Decoupling

Following the decoupling rules in the [Coding Standards](coding_standards.md#part-h-performance--sfml-rules), gameplay simulation classes (`Cannon`, `Brick`, `Projectile`) must never contain SFML graphics headers, rendering methods, or make direct draw calls.

Instead, the drawing loop isolates logic using a strict data-to-view pipeline:

```
  [GameplayWorld (Model)]
             │
             ▼ coordinates / state (read-only)
     [CannonView (View)]
             │
             ▼ submits sf::Drawable & RenderLayer
    [RenderSystem (System)]
             │
             ▼ schedules submissions
   [sf::RenderTarget (Window)]
```

* **GameplayWorld**: Houses raw simulation state data (positions, velocities, angles, and health).
* **CannonView / BrickView / ProjectileView**: Subscribes to the model state, queries coordinate variables (strictly read-only), retrieves associated sprites or shapes from the resource cache, and submits them to the `RenderSystem`.
* **RenderSystem**: Schedules draw calls based on `RenderLayer` priority and submits the final draw calls to the `sf::RenderTarget` (the screen window context).

---

## PART F: Resource Lifetimes & Caching

To prevent loading assets on-demand and causing framerate hitches, asset lifetimes are managed by dedicated caching providers:

### 1. Cache Owners
* **Textures**: Loaded and cached inside `ResourceManager` (templated instances owned by the application context `Game` and injected into views).
* **Shaders**: Managed by `ResourceManager` once the shader pipeline described in [Part G](#part-g-future-post-processing-shader-pipeline) is introduced. *Note: The project is currently shader-free.*
* **Fonts**: Managed by the application context and shared among UI components.
* **Sound Buffers**: Cached in `AudioManager` to prevent memory address shifts, as described in the [Asset Manual](asset_pipeline.md#part-a-current-asset-pipeline).

### 2. Asset Lifetime Guidelines
* All textures, fonts, and shaders must be pre-loaded during state transitions (loading screens, menus).
* Asset files must never be loaded from disk during active gameplay ticks.
* Cached resources are retained in memory for the duration of the active state. Unloading caches occurs during state exit (e.g. returning to main menu triggers `ResourceManager::clear()`).

---

## PART G: Future Post-Processing Shader Pipeline

For release builds (Phase 3), the engine will transition CPU-bound rendering steps to GPU fragment shaders using a post-processing chain:

```
  [Game Views Draw] ──► [Offscreen Render Texture]
                               │
                               ▼ bound as input texture
                    [Post-Processing Shaders]
                      ├─ Bloom filter blurs
                      ├─ CRT scanline distortion
                      └─ Chromatic aberration shifts
                               │
                               ▼ final draw
                    [sf::RenderWindow (Screen)]
```

### Planned Shader Effects
1. **Bloom Shader**:
   * Extracts bright pixels from the offscreen render texture (threshold filter).
   * Applies horizontal and vertical Gaussian blur passes.
   * Blends the blurred highlight texture back over the original base frame, creating a glowing neon effect.
2. **CRT Scanline Effect**:
   * Applies a subtle spherical curvature grid distortion to simulate retro CRT screens.
   * Implements moving horizontal scanlines and pixel shadow mask patterns.
3. **Chromatic Aberration**:
   * Offsets the Red and Blue color channel coordinates dynamically.
   * The offset distance scales with gameplay events (e.g., spikes during screen shakes or hit stop freezes).

---

## PART H: Rendering Performance Budget

Because the CPU-bound offset glow method requires multiple passes ($8 \times L + 1$ draw submissions per glowing entity), the rendering engine operates under strict performance budgets to prevent framerate dips.

| Performance Metric | Target Budget | Description |
| :--- | :--- | :--- |
| **Target Framerate** | 60 FPS | Target refresh rate standard for display synchronization. |
| **Total Frame Budget** | 16.67 ms | Maximum execution limit per frame. |
| **Rendering Budget** | $\le 5.0$ ms | Maximum frame time allocated for rendering (leaving $\ge 11.6$ ms for physics and logic). |
| **Target Draw Calls** | $\le 500$ / frame | Total draw call submissions limit per frame. |
| **Target Active Particles** | $\le 2000$ | Maximum concurrent particle count. |
| **Target Glow Objects** | $\le 100$ | Maximum concurrent entities drawing CPU glow passes. |

> [!WARNING]
> **Performance Hazard**: The multi-pass CPU glow simulation requires $8 \times L + 1$ draw calls per glowing object. Without a strict budget, adding even a moderate number of glowing objects (e.g., 300 objects) can result in over $2,400$ draw calls, instantly destroying frame times and dropping performance below 60 FPS. Hence, glow passes must be reserved strictly for core entities (Cannon, Projectiles, and active Bricks) and limited to $\le 100$ concurrent objects.

---

## PART I: Resolution & UI Scaling

To ensure the interface scales cleanly across various monitors, television screens, and portable display formats (e.g., Steam Deck), the rendering engine implements standard resolution boundaries.

### 1. Supported Resolutions
* **Reference Resolution**: `1920 x 1080` (all UI layout coordinates, button dimensions, and text offsets are defined relative to this grid).
* **Officially Supported Formats**:
  * `1280 x 720` (720p HD / Handheld scale)
  * `1920 x 1080` (1080p Full HD)
  * `2560 x 1440` (1440p QHD)
  * `3840 x 2160` (4K UHD)

### 2. Aspect Ratio & UI Scaling Policies
* **Aspect Ratio Handling & Letterboxing Policy**: For displays that do not match the reference 16:9 aspect ratio (such as a 16:10 Steam Deck screen or a 21:9 ultrawide monitor), the engine maps the world view within a letterboxed or pillarboxed frame to preserve aspect ratio scaling.
  * **Pillarboxing**: Active on ultrawide monitors (21:9 or 32:9). The gameplay area is centered horizontally, flanked by black side margins.
  * **Letterboxing**: Active on 16:10 screens (e.g., Steam Deck 1280x800). The gameplay area is centered vertically, flanked by black top/bottom margins.
  * **Implementation**: Managed dynamically via `sf::View::setViewport` by computing normalized coordinate rectangle mappings `sf::FloatRect(left, top, width, height)` of the target window.
* **UI Scaling Behavior**: HUD coordinates use vector-based proportional scaling or anchor offsets (e.g. centering a panel, or offsetting from the top-left edge) rather than absolute pixel locations to prevent stretching.
  * The UI view is rendered to a fixed virtual resolution of `1920x1080` and is scaled by the engine to fit the physical window dimensions using a uniform letterboxed scaling viewport to guarantee consistent text sizes and UI ratios across all platforms.

