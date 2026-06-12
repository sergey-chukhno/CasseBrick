# Art Direction & Visual Identity Specification (art_direction.md)

This document serves as the authoritative visual bible for the **Cyberpunk Cannon Shooter** project. It establishes the creative guidelines, aesthetic principles, production pipelines, and visual invariants to ensure consistency across all elements produced by internal artists, UI designers, VFX developers, external outsourcing partners, and AI generation pipelines.

---

## 1. Vision Statement

### Emotional Experience & Player Feeling
The player must feel a sense of hyper-focused, high-stakes, kinetic control. They are positioned as an elite grid operator defending a secure mainframe against descending waves of rogue encrypted data cores and security blocks. Every action must deliver an immediate, intense physical response. Visual feedback is loud, bright, and clean—inducing a flow state akin to a high-speed command-line hacking session meets pure retro-arcade physics. 

We target the psychological feelings of:
*   **Adrenaline & Urgency**: The threat is descending; space is shrinking.
*   **Precision & Control**: The Cannon feels like a calibrated, surgical tool of light.
*   **Kinetic Satisfaction**: Shattering blocks must feel heavy, crunchy, and explosive.

### The Delivered Fantasy
The player is operating a heavy-duty particle accelerator inside a dark, retro-futuristic virtual terminal network. The blocks descending from the top are not stone bricks; they are encrypted data packets, firewall shards, and malicious subroutines. Breaking them is a literal act of decryption and structural shattering. The cannon does not shoot lead bullets; it fires condensed energy packets (plasma projectiles) that ricochet off digital boundaries, behaving according to terminal laws of physics.

### Visual Identity & Differentiation
We reject generic cyberpunk aesthetics of rain-slicked neon alleys, dirty trash cans, and crowded, messy industrial corridors. Instead, our visual identity is **Clean, High-Tech Virtual Neon**. 

Our distinct look is defined by:
*   **Infinite Virtual Voids**: Structured geometric coordinate systems floating in deep, clean digital space.
*   **Emissive Glow Pipelines**: High-intensity, HSL-targeted glow lines utilizing multi-pass bloom algorithms.
*   **Hard Geometric Trims**: Clean, clipped $45^\circ$ angles on all borders, brackets, and interfaces.
*   **Mechanical Energy Contrasts**: Absolute visual distinction between static, dark grey terminal structures and dynamic, glowing energy assets.

---

## 2. Visual Pillars

```
┌─────────────────────────────────────────────────────────────────────────┐
│                             VISUAL PILLARS                              │
├────────────────────────────────┬────────────────────────────────────────┤
│ PILLAR                         │ CORE CONCEPT                           │
├────────────────────────────────┼────────────────────────────────────────┤
│ 1. Vector Neon Minimalism      │ High-emissive wireframes over voids.   │
│ 2. High-Contrast Readability   │ Absolute foreground/background split.  │
│ 3. Clean Gameplay First        │ Visuals never obstruct collisions.     │
│ 4. Angular Cybernetic Geometry │ 45-degree beveled trims; no curves.    │
│ 5. Controlled Color Script     │ Colors denote system/gameplay state.   │
│ 6. Glitched System Degradation │ Visual noise signals impact and damage. │
└────────────────────────────────┴────────────────────────────────────────┘
```

### Pillar 1: Vector Neon Minimalism
*   **Meaning**: Perfect, crisp vector-style outlines glowing with high intensity over flat, dark, non-textured fills.
*   **Purpose**: Keeps the screen free of visual noise. The player should immediately identify active entities (cannon, projectiles, bricks) without being distracted by organic textures or painted details.
*   **Implementation**: Elements must use solid dark base colors (`#0a0a1a`) with outer borders colored in bright, single-hue emissive neon shades (`NEON_CYAN`, `NEON_PINK`). Texture maps are strictly restricted to flat color masks and glow maps.

### Pillar 2: High-Contrast Gameplay Readability
*   **Meaning**: Layered division of the visual stack based on functional importance.
*   **Purpose**: Prevents fast-moving projectiles ($>500\text{ px/s}$) or small powerups from getting lost in glowing backgrounds, preventing visual fatigue.
*   **Implementation**:
    *   **Background Elements (Grid/Stars)**: Opacity $\le 10\%$, rendering below the main gameplay layer.
    *   **Inactive/Static Elements (Walls/HUD Frames)**: Opacity at $40\% - 60\%$.
    *   **Active Gameplay Elements (Bricks/Cannon/Projectiles)**: Opacity at $100\%$ with active, multi-pass bloom rendering.

### Pillar 3: Clean Gameplay First
*   **Meaning**: Visual effects (VFX, trails, screen shakes) must strictly represent mathematical and physics states without extending beyond collision boundaries.
*   **Purpose**: Ensures the game feel remains responsive, fair, and mathematically accurate.
*   **Implementation**: Particle systems must fade to transparent within $300\text{ms}$. No particle should have a diameter greater than $4\text{px}$. Projectile trails are capped at a history of 8 frames, and must taper to a point that exactly matches the projectile's trajectory.

### Pillar 4: Angular Cybernetic Geometry
*   **Meaning**: The absolute prohibition of organic shapes, smooth curves, and circles—except for projectiles.
*   **Purpose**: Reinforces the cold, computer-mainframe, manufactured terminal environment.
*   **Implementation**: All UI buttons, panels, cannon housings, and bricks must feature a beveled, clipped corner trim. Standard cut: $12\text{px}$ offset at a $45^\circ$ angle. Rounded corners on any asset will cause immediate review rejection.

### Pillar 5: Controlled Color Script
*   **Meaning**: Visual hues are assigned strictly according to functionality, rather than aesthetics alone.
*   **Purpose**: Allows the player to read the game state sub-consciously. Colors communicate friend-or-foe, danger, reward, and interactive status.
*   **Implementation**: Only elements carrying the exact HSL coordinates defined in the color script may display neon colors. Unassigned colors are forbidden.

### Pillar 6: Glitched System Degradation
*   **Meaning**: The application of digital artifacts (chromatic aberration, horizontal scanline shifts, pixel noise) to indicate force and damage.
*   **Purpose**: Adds visual history, weight, and impact to the clean terminal space when actions occur.
*   **Implementation**: Glitch shaders and scanline distortions are active only during collision frames or critical warnings, decaying exponentially over a maximum of 150ms.

---

## 3. Artistic Influences

### Games
*   **Rez & Tetris Effect (Enhance Games)**:
    *   *What we borrow*: Synesthetic integration of geometry, grid-based sound-reactive pulses, and glowing vector wireframes that react to game state.
    *   *What we reject*: Shifting 3D camera angles, abstract camera zooms, and organic, fluid particle clusters. We maintain a locked 2D orthographic perspective.
*   **Cyberpunk 2077 (CD Projekt RED)**:
    *   *What we borrow*: Tactical UI styling, monospaced diagnostic layouts, alert boxes with neon cyan/pink brackets, and sharp terminal grids.
    *   *What we reject*: Industrial decay, garbage, mud, rust, blood, and physical dirt textures. Our virtual terminal is a clean, idealized digital workspace.
*   **Hades (Supergiant Games)**:
    *   *What we borrow*: Hyper-responsive impact frames, snappy visual feedbacks, and distinct silhouette readability for active elements.
    *   *What we reject*: Hand-painted, watercolor organic shapes and fantasy mythology palettes.
*   **Geometry Wars (Bizarre Creations)**:
    *   *What we borrow*: Intense vector neon lines and explosive particle bursts on entity destruction.
    *   *What we reject*: Screen-covering paint splashes and fluid black-hole gravity distortions that deform the gameplay grid.

### Films & Anime
*   **Tron / Tron: Legacy (Disney)**:
    *   *What we borrow*: Crisp, glowing circuit tracks, dark voids, high-contrast grids, and sharp mechanical illumination.
    *   *What we reject*: Monochromatic layouts. We require a rich, saturated, dual-color cyberpunk palette.
*   **Akira (Katsuhiro Otomo)**:
    *   *What we borrow*: Kinetic trail lines, heavy mechanical weapon designs, and raw visual impact during combat.
    *   *What we reject*: Gritty city debris, concrete textures, and hand-drawn cell imperfections.
*   **Ghost in the Shell (1995)**:
    *   *What we borrow*: Cybernetic console layouts, digital green coding readouts, and systemic, structured technical lines.
    *   *What we reject*: Real-world dirty environments and muted palettes.

### Industrial & Architectural Design
*   **Deconstructivism (Zaha Hadid, Daniel Libeskind)**:
    *   *What we borrow*: Angled, overlapping plates, aggressive geometry, and dynamic, non-parallel structural lines for menus and the Cannon design.
    *   *What we reject*: Curve-dominated structures and raw concrete finishes.
*   **Brutalist Web Design**:
    *   *What we borrow*: Solid color blocks, heavy borders, high-contrast text alignments, and monospaced typography.
    *   *What we reject*: Overly complex textures and realistic drop shadows.

---

## 4. Color Script

To guarantee absolute visual consistency and high-speed gameplay readability, the visual palette is restricted to these coordinates:

```
┌────────────────────────────────────────────────────────────────────────┐
│                              COLOR SCRIPT                              │
├─────────────────┬─────────┬───────────────────┬────────────────────────┤
│ PALETTE TIER    │ HEX     │ HSL COORDINATES   │ PSYCHOLOGICAL PURPOSE  │
├─────────────────┼─────────┼───────────────────┼────────────────────────┤
│ Primary (Cyan)  │ #00d9ff │ HSL(189, 100%, 50%)│ Player, UI Outlines,    │
│                 │         │                   │ Shield indicators.     │
│ Secondary (Pink)│ #ff006e │ HSL(334, 100%, 50%)│ Boss nodes, high-value │
│                 │         │                   │ targets, HUD frames.   │
│ Accent (Purple) │ #9d4edd │ HSL(274, 61%, 59%) │ Power-ups, secondary   │
│                 │         │                   │ armor, energy rings.   │
│ Danger (Orange) │ #ff8800 │ HSL(32, 100%, 50%) │ Projectiles, falling   │
│                 │         │                   │ canisters, critical warnings.
│ Reward (Green)  │ #06ffa5 │ HSL(157, 100%, 50%)│ Score animations,      │
│                 │         │                   │ level clears, health up.
│ Metallic Gold   │ #e5c158 │ HSL(45, 76%, 63%)  │ Unbreakable Bricks,    │
│                 │         │                   │ rare Boss armor.       │
│ Dark BG (Void)  │ #0a0a1a │ HSL(240, 44%, 7%)  │ Pitch-black workspace, │
│                 │         │                   │ virtual canvas.        │
└─────────────────┴─────────┴───────────────────┴────────────────────────┘
```

### Color Usage Rules
1.  **Readability Requirements**: Glow layers must be applied using a multi-pass blend. Emissive outlines must always render at $100\%$ opacity, with glow fade opacity scaling from $40\%$ down to $0\%$ over 3 passes.
2.  **Forbidden Colors**:
    *   **Muted Browns/Greens**: Absolutely forbidden. These colors indicate dirt and organic components, which break the clean virtual terminal aesthetic.
    *   **Pastels**: Forbidden. Pastel colors break the high-energy neon look.
    *   **Pure White**: Restricted strictly to impact frames and lightning flashes. Never use pure white for UI text or general outlines.

### Blend Mode Architecture
All glow layers must use additive blending (`sf::BlendAdd` in SFML) to accumulate light intensity where elements overlap, simulating a true physical CRT screen or laser projector.

```
       [ Emissive Core Sprite ]   ──► Opacity: 100% (Normal Blend)
       [ Inner Glow Map ]         ──► Opacity: 60%  (Additive Blend)
       [ Outer Bloom Shadow ]     ──► Opacity: 20%  (Additive Blend, 12px blur)
```

---

## 5. Shape Language

We enforce a strict shape hierarchy to communicate gameplay roles completed at a glance:

```
    [Player Assets (Cannon)]   ──► Solid, heavy, horizontal trapezoids with clipped corners.
    [Enemy Blocks (Bricks)]    ──► Sharp octagons, rigid grids, and angular crystals.
    [Projectiles (Bullets)]    ──► Perfect circles (highest contrast against angular blocks).
    [UI Elements (Menus)]      ──► Angled plates, offset parallel lines, and hex matrices.
```

### Shape Language Rules
*   **Player Cannon**: Must communicate weight, mass, and grounding. Structured with horizontal trapezoids with $45^\circ$ beveled corners. Movement is strictly horizontal; the shape's wide base emphasizes this constraint.
*   **Enemies (Bricks)**: Rigid, structural, and angular. Bricks are sharp octagons or beveled rectangles, conveying armor and data encryption blocks. Outer boundaries must look hard.
*   **Projectiles**: Must be perfect circles. Because the rest of the game consists of sharp angles, straight lines, and grids, circles stand out immediately. This visual contrast allows players to track bullet trajectories easily.
*   **Powerups**: Floating canisters shaped like vertical hexagons. They slide downwards slowly, contrasting with the fast horizontal motion of the player and the vertical drops of bricks.
*   **UI Components**: Skewed, overlapping plates. Submenus and panels use parallel lines with corner cutouts, mimicking a military tactical display.

### Readability Hierarchy at Distance
*   **Foreground (Level 1)**: Projectiles and Bricks. Thick outer outlines ($3\text{px}$ line weight). Emissive value at $100\%$.
*   **Midground (Level 2)**: The Cannon and Powerup indicators. Outline thickness $2\text{px}$. Emissive value at $80\%$.
*   **Background (Level 3)**: Arena boundary walls and UI HUD borders. Line weight $1\text{px}$. Emissive value at $40\%$.

---

## 6. Environment Direction

### Background Style
The background represents a deep virtual space inside a supercomputer. It is pitch black (`#0a0a1a`) with a subtle, glowing structural coordinate system.

### Parallax Layer Configuration
To establish depth without creating visual clutter, the game uses three distinct parallax layers:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        PARALLAX STACK SCHEMATIC                        │
├────────────────────────────────────────────────────────────────────────┤
│ Layer 1: Active Arena (Speed: 1.0)                                     │
│  - Gameplay grid lines, glowing playfield walls, and active entities.  │
├────────────────────────────────────────────────────────────────────────┤
│ Layer 2: Midground Stars & Nodes (Speed: 0.4)                           │
│  - Sharp, small octagonal stars drifting vertically. Color: Purple.    │
├────────────────────────────────────────────────────────────────────────┤
│ Layer 3: Background Nebula (Speed: 0.1)                                │
│  - Soft, blurred, large circular light fields simulating network node │
│    hubs. Color: Deep Pink/Indigo (Opacity: 5%).                        │
└────────────────────────────────────────────────────────────────────────┘
```

*   **Layer 1 (Active Arena)**: Coordinates, boundaries, and active objects. Vector grid lines rendered at $10\%$ opacity.
*   **Layer 2 (Midground Stars)**: Small octagonal vector star sprites (`texture_bg_stars`). Colors: `#00d9ff` and `#9d4edd`. Opacity: $15\%$.
*   **Layer 3 (Background Nebula)**: Large, blurred, soft-circle nebulae. Color: Deep indigo and pink. Opacity: $5\%$.

### Lighting & Atmospheric Storytelling
*   **Lighting Style**: Emissive glow lighting only. All light originates from active components (the Cannon core, projectiles, glowing data bricks). No global ambient source.
*   **Environmental storytelling**: Glitch zones, warning grids, glowing circuit traces.

#### Acceptable Environment Approach
*   A clean, dark digital void.
*   Background grids that remain perfectly aligned to the camera view.
*   Stars that fade out dynamically when active projectiles pass near them.

#### Unacceptable Environment Approach
*   Real-world city backgrounds, realistic rain, dirty brick walls, or photorealistic gas clouds.
*   Organic, hand-painted background layers.
*   Parallax layers that scroll at high speeds, causing motion sickness.

---

## 7. Character and Object Design Language

### The Cannon
*   **Visual Identity**: Heavy energy emitter terminal. Solid plates with a bright core.
*   **Materials & Surface Treatment**: Anodized dark metal plates with high-gloss finishes and glowing neon cyan circuits.
*   **Detail Density**: High detail density around the central core (energy rings, fins) and low detail density on the outer base plates.
*   **Silhouette Rules**: A wide, stable trapezoidal base with a straight, thick barrel that extends upward.

```
                  _   _   _     [Barrel Fins: High Detail]
                 | |_| |_| |
                 |         |
                /           \
               /  ( CORE )   \  [Glowing Core: Emissive Pink/Cyan]
              /               \
       ______/_________________\______
      /                               \
     /    [====== PLATFORM ======]     \ [Base Plates: Clean Metal]
    /___________________________________\
```

### Projectiles
*   **Visual Identity**: Unstable plasma energy sphere.
*   **Materials**: High-intensity plasma core with glowing trail.
*   **Detail Density**: Zero interior details (solid `#ff8800` center) with a multi-layered glow.
*   **Silhouette Rules**: Perfect circle. No flat edges.

### Bricks
Bricks are categorized by structural type, each carrying a unique visual treatment to denote strength:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                               BRICK TYPES                               │
├─────────────────┬────────────────────────────┬──────────────────────────┤
│ BRICK CATEGORY  │ VISUAL STYLE               │ GLOW SCHEME              │
├─────────────────┼────────────────────────────┼──────────────────────────┤
│ Standard        │ Flat beveled rectangle     │ Cyan outline, semi-      │
│                 │ (64x32px), thin border.    │ transparent core.        │
├─────────────────┼────────────────────────────┼──────────────────────────┤
│ Armored         │ Octagonal panel with inner │ Pink outline with        │
│                 │ cross-hatch reinforcement. │ internal support braces. │
├─────────────────┼────────────────────────────┼──────────────────────────┤
│ Unbreakable     │ Solid dark grey block      │ Metallic gold trim,      │
│                 │ with metallic gold corners.│ zero center glow.        │
├─────────────────┼────────────────────────────┼──────────────────────────┤
│ Explosive       │ Hexagonal caution patterns │ Pulsing orange grid      │
│                 │ on face, high line weight. │ (Cycle rate: 2Hz).       │
└─────────────────┴────────────────────────────┴──────────────────────────┘
```

*   **Standard Brick**: Clean beveled rectangle, HSL Cyan border, semi-transparent center.
*   **Armored Brick**: Octagonal shape with double inner outline, HSL Pink border.
*   **Unbreakable Brick**: Heavy solid metal block, HSL Gold border, no glow.
*   **Explosive Brick**: Hexagonal caution lines on face, HSL Orange border.

### Powerups
*   **Visual Identity**: Glowing cybernetic canisters.
*   **Materials**: Semi-transparent glass tubes with a glowing fluid core.
*   **Detail Density**: Medium. Simple icons printed on the canister face representing its capability (e.g., three arrows for Multi-shot, a straight line for Laser).
*   **Silhouette Rules**: Vertical elongated hexagons.

### Bosses
*   **Visual Identity**: Heavy, multi-segmented digital fortresses.
*   **Materials**: Dark carbon-fiber plates overlaying glowing core systems.
*   **Detail Density**: Extremely high. Exposed circuitry, glowing heat-sinks, and rotating armor plates.
*   **Silhouette Rules**: A central core protected by independent, rotating shield segments (modeled strictly as angular segmented octagonal plates; curves are prohibited).

---

## 8. User Interface Direction

### Typography
*   **Display Font (Headers, Titles)**: **Orbitron Bold** (TrueType). A highly geometric, techno-stylized sans-serif font.
*   **Body Font (Controls, Values)**: **Rajdhani Regular** (TrueType). A clean, squared-arc font with high readability at small scales.

### Spacing & Layout Philosophy
The UI layout is based on a rigid $24\text{px}$ terminal grid. All panels, frames, and borders are aligned to these margins. The center playfield must remain completely unobstructed by non-critical HUD elements.

```
  ┌────────────────────────────────────────────────────────┐
  │ Score: 0012500                          [ MUTE AUDIO ] │
  │ Level: 04                                              │
  │                                                        │
  │                                                        │
  │                    [ GAMEPLAY FIELD ]                  │
  │                                                        │
  │                                                        │
  │                                                        │
  │ High: 0099420                                          │
  └────────────────────────────────────────────────────────┘
```

### HUD Specifications
*   **Layout**: Top left: Score and current level; Top right: Audio toggle button and pause indicator. Bottom left: High score.
*   **Borders**: Sharp 1px neon pink lines (`#ff006e`) with clipped corners on all HUD boxes.
*   **Contrast**: Active text must exceed a $7:1$ contrast ratio against the background void.

### Menu Design & Screen Transitions
*   **Menu Panels**: Overlapping, tilted plates (`45^\circ` angle trims) that slide in from the screen edges.
*   **Transitions**: Transitions are snappy and high-tech:
    1.  **Glitch-in**: The new screen renders with a $50\text{ms}$ chromatic aberration and scanline offset.
    2.  **Typewriter effect**: Text labels populate characters one by one at a rate of 1 character per $10\text{ms}$.
    3.  **Slide-out**: Old menus slide out along their angled cuts, not vertically or horizontally.

---

## 9. VFX Direction

### Explosions & Brick Destruction
When a brick shatters, it must burst into $15$–$25$ small, sharp, square particles of matching colors (e.g., cyan or pink).
*   **Glow Intensity**: High intensity bloom on burst, fading to transparent within 250ms.
*   **Particle Physics**: Snappy initial velocity, decaying rapidly with a slight gravity pull downwards.

### Projectile Trails
*   Trails must be rendered as clean, overlapping line segments (constructed via `sf::RectangleShape`) that fade out linearly.
*   **Trail Length**: Restructured to 8 historical segments.
*   **Trail Thickness**: Scales down from projectile diameter to $0\text{px}$ at the oldest segment.

```
       ( BULLET ) ──► [ Segment 1 ] ──► [ Segment 4 ] ──► [ Segment 8 ]
         16px            14px              8px              0px
        Glow:100%       Glow:80%          Glow:40%         Glow:0%
```

### Powerup Collection
*   **Visual Effect**: A expanding, circular shockwave ring centered on the Cannon's core.
*   **Color**: HSL Purple (`#9d4edd`).
*   **Physics**: Ring expands from $10\text{px}$ to $128\text{px}$ diameter over $200\text{ms}$, with line thickness tapering from $4\text{px}$ to $0\text{px}$.

### Readability Constraints
At no point may VFX particles obscure or mask active projectiles. Projectiles render at the highest draw layer, with a write-mask that prevents particle sprites from drawing on top of them.

---

## 10. Animation Direction

### Snappy Arcade Motion Invariants
We follow a strict, high-frame-rate arcade animation philosophy. Drag, heavy inertia, or long ease-in times are forbidden. The controls and responses must feel instantaneous.

*   **Anticipation (Firing)**:
    Before firing, the Cannon barrel compresses on the Y-axis by $5\%$ over $50\text{ms}$, then expands on the Y-axis by $15\%$ over $30\text{ms}$ during the firing frame, returning to base scale over $100\text{ms}$ with a slight elastic bounce.
*   **Impact Frames**:
    When a projectile hits a brick, render a single-frame flash of pure white (`#ffffff`) at the collision normal location. This flash is a single vector rectangle offset by the collision angle.
*   **Viewport Screen Shake**:
    Screen shake must occur only during major gameplay events (brick shatters, boss hits). The shake path follows a fast decaying sine wave:

$$\text{offset} = A \cdot e^{-\lambda t} \cdot \sin(\omega t)$$

*   **Maximum Amplitude ($A$)**: $\le 8\text{px}$.
*   **Decay rate ($\lambda$)**: $12.0$.
*   **Frequency ($\omega$)**: $45.0$.
*   **Duration**: Capped at $150\text{ms}$.

---

## 11. Asset Quality Standards

All assets must meet the following technical requirements before being committed to the repository:

### Sprites & Textures
*   **Format**: 32-bit RGBA `.png` with pre-multiplied alpha channel.
*   **Resolution Guidelines**:
    *   `texture_cannon_base`: $128\text{px} \times 128\text{px}$.
    *   `texture_brick_standard`: $64\text{px} \times 32\text{px}$.
    *   `texture_projectile`: $16\text{px} \times 16\text{px}$.
    *   `texture_powerup`: $32\text{px} \times 32\text{px}$.
*   **Glow Maps**: Glow maps must be stored in the alpha channel or as a separate grayscale texture map suffixed with `_glow.png`.

### UI Assets
*   UI panels must be defined as vector shapes or clean 9-slice sprites.
*   All UI panels must be defined with exact slice coordinates mapped in [assets_manifest_spec.md](assets_manifest_spec.md).

### Audio Specifications
*   **Sound Effects (SFX)**: `.wav` format, 16-bit PCM, 44.1kHz, Mono, to guarantee low-latency loading and playbacks.
*   **Background Music (BGM)**: `.ogg` format, 44.1kHz, Stereo, compressed, configured with loop points in the metadata.

### Asset Naming Conventions
Assets must follow strict naming patterns:
*   Sprites: `texture_[category]_[asset_name].png` (e.g., `texture_brick_standard.png`, `texture_cannon_neon.png`).
*   Audio SFX: `audio_sfx_[event_name].wav` (e.g., `audio_sfx_laser_fire.wav`).
*   Audio BGM: `audio_bgm_[track_name].ogg` (e.g., `audio_bgm_mainframe_groove.ogg`).
*   Fonts: `font_[font_name]_[weight].ttf` (e.g., `font_orbitron_bold.ttf`).

---

## 12. AI Asset Generation Pipeline

Generative AI (e.g., Midjourney, Stable Diffusion, DALL-E) is authorized only to generate raw silhouette concepts or decorative texture layers. AI outputs must never be committed directly to the repository without passing through the manual post-processing pipeline.

### Prompt Design Guidelines
*   **Positive Descriptors**: `vector cyberpunk icon, clean lines, high-contrast neon, flat fills, dark background, 2d game asset, orthographic projection, beveled edges, hard surface`.
*   **Negative Descriptors**: `organic, hand-drawn, 3d render, complex lighting, grunge, rust, dirty, noise, paint textures, shadow, blur, round corners`.

### Manual Post-Processing Pipeline
```
  [ AI Image Output ] ──► [ Vectorization (Illustrator) ] ──► [ HSL Color Remap ] ──► [ Hard-Angle Trim (45°) ] ──► [ Export RGBA PNG ]
```
1.  **Vectorization**: Trace AI outputs in vector format (e.g., Adobe Illustrator) to remove artifacts and pixel noise.
2.  **Color Remapping**: Remap colors to match the exact HSL coordinates defined in the **Color Script**.
3.  **Geometric Cleanup**: Apply beveled corners ($45^\circ$ cut guideline) and align borders to clean grid boundaries.
4.  **Export**: Save as 32-bit RGBA `.png` with transparent background.

### AI Asset Review Checklist

| Checkpoint | Criteria | Pass / Fail |
| :--- | :--- | :---: |
| **Silhouette Check** | Does the asset display clean, recognizable geometric silhouettes from a gameplay distance? | [ ] |
| **Color Check** | Does the asset match the exact HSL coordinates of our Color Script? | [ ] |
| **Texture Check** | Is the surface clean of grunge, noise, gradients, and photographic textures? | [ ] |
| **Geometry Check** | Are the corners sharp and follow the beveled $45^\circ$ cut guideline? | [ ] |
| **Transparency Check** | Is the asset completely isolated on a transparent alpha background? | [ ] |

*Rejection Criteria*: Any asset containing organic gradients, dirty textures, rounded corners, or color drift outside the script must be rejected immediately.

---

## 13. Outsourcing Guidelines

For assets commissioned to external freelancers or studios, deliverables must conform to these rules:

*   **File Formats**: Source files must be delivered as layered Photoshop files (`.psd`) or vector files (`.svg` / `.ai`). Compiled files must be exported as `.png`.
*   **Naming Conventions**: Same as defined in Section 11 (e.g., `texture_brick_crystal.png`).
*   **Revision Expectations**:
    *   *Concept Stage*: 3 rough black-and-white silhouettes must be approved before clean line work begins.
    *   *Color Stage*: Base flat colors must be validated against the color script before glow mapping or shading is applied.
    *   *Final Delivery*: Must pass the **AI Asset Review Checklist** and run correctly in the SFML test scene.

---

## 14. Art Production Roadmap

In alignment with [ROADMAP_FORWARD.md](../ROADMAP_FORWARD.md), art production is divided into the following priorities:

### Priority 1: Vertical Slice Basics (Phase 1.5 Targets)
*   `texture_cannon_base`: Standard cannon assembly.
*   `texture_brick_standard`: Standard beveled brick.
*   `font_orbitron_bold`: Orbitron TrueType font.
*   `font_rajdhani_regular`: Rajdhani TrueType font.
*   `audio_sfx_laser_fire`: Cannon firing sound effect.
*   `audio_sfx_brick_shatter`: Brick destruction sound effect.

### Priority 2: Gameplay Progression Assets (Phase 2 Targets)
*   `texture_brick_armored`: Octagonal armored brick.
*   `texture_brick_explosive`: Hexagonal explosive brick.
*   `texture_powerup_multishot`: Multi-shot canister icon.
*   `texture_powerup_laser`: Laser sight canister icon.
*   `texture_boss_shield`: Segmented shield core boss sprite.
*   `audio_bgm_mainframe_groove`: Primary gameplay loop track.

### Priority 3: Polish and Custom Cosmetics (Phase 3 Targets)
*   `texture_bg_stars`: Drifting star sprites.
*   `texture_ui_panel`: 9-slice panel frames.
*   `texture_cosmetic_heavy`: Heavy cannon cosmetic skin.
*   `audio_sfx_powerup_collect`: Powerup collection sound effect.

### Priority 4: Marketing and Release assets (Phase 5 Targets)
*   `promo_capsule_steam`: Steam store capsule key art.
*   `promo_banner_discord`: Discord banner graphic.
*   `icon_game_launcher`: Game desktop launcher icon (256x256, 128x128, 48x48 sizes).

---

## 15. Art Direction Freeze Rules

The following visual styles and patterns are frozen and must be rejected by reviewers:

### Forbidden Visual Styles
*   **Hand-painted Watercolor**: Breaks the digital terminal theme.
*   **Pixel Art**: The project is vector-styled; pixel art will conflict with clean font rendering.
*   **3D Skeletal Mesh Renders**: Keep everything strictly 2D flat sprites.
*   **Organic Curves & Circular Bricks**: Bricks must remain rigid and angular. Only projectiles use circular shapes.

### Forbidden UI Patterns
*   **Rounded Borders & Round Buttons**: Buttons must use $45^\circ$ beveled corners.
*   **Skewed Alignments**: The HUD layout must remain orthogonal and aligned to the $24\text{px}$ margins.
*   **Drop Shadows**: Realistic drop shadows are forbidden. Use offset solid color panels instead.

### Forbidden Shortcuts
*   **Loose copy-pasting of raw AI assets**: AI-generated assets must go through manual paint-over passes to ensure they match our exact HSL colors and carry vector-clean line trims.

---
*End of Art Direction & Visual Identity Specification v1.0*
