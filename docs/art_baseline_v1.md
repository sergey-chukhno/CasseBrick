# Art Baseline v1.0

This document is the authoritative visual source of truth for **Cyberpunk Cannon Shooter**. It establishes the concrete implementation metrics, spatial layouts, asset specs, and quality benchmarks that realize the vision defined in [docs/art_direction.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/art_direction.md).

---

## 1. Purpose of Art Baseline

The **Art Baseline** serves as the practical bridge between high-level visual styling and raw asset implementation:

```
                  [ Art Direction v1.0 ]
              (Establishes Hues, Style, Tone)
                            │
                            ▼
                    [ Art Baseline v1.0 ]
       (Defines Dimensions, Layouts, ASCII Wireframes)
                            │
                            ▼
              [ Asset Production Pipeline ]
          (Exports PNGs, WAVs, Sets Manifest Registry)
```

### Purpose in Production and Quality Gates
*   **Relationship to Art Direction**: While Art Direction defines *why* a visual style exists, the Art Baseline specifies *what* exact dimensions, layouts, coordinates, and frames are approved for the final game.
*   **Relationship to Asset Production Pipeline**: The scales, sizes, and naming schemes defined here feed directly into asset packing scripts (`tools/packer.py`) and validator routines (`tools/verify_assets.py`) as defined in [docs/asset_pipeline.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/asset_pipeline.md).
*   **Role in Asset Approval**: All assets must pass the checklists in this document before they are integrated into the main branch.
*   **Role in Outsourcing**: Provides external freelancers and outsourcing studios with non-negotiable guidelines for file exports, proportions, and layering, eliminating style drift during development.

---

## 2. Golden Screenshot Specification

The Golden Screenshot represents the absolute target visual composition of the game during active gameplay. All screen rendering must conform to the geometry, hierarchy, and lighting shown here.

### Playfield Dimensions
*   **Active Canvas**: $1920\text{px} \times 1080\text{px}$ (16:9 Aspect Ratio).
*   **Gameplay Bounds**: $1200\text{px} \times 1000\text{px}$ (centered horizontally, $y$-offset from top: $40\text{px}$).

### Annotated Composition Wireframe

```
0px ─────────────────────────────────── 1920px ───────────────────────────────────
   │ [HUD Left: Score/Level]                                   [HUD Right: Audio/Pause] │
40px ┌───────────────────────── GAMEPLAY AREA: 1200px ─────────────────────────┐
   │ │                                                                         │ │
   │ │   [Armored Brick]          [Standard Brick]           [Standard Brick]  │ │
   │ │    (Pink Outline)           (Cyan Outline)             (Cyan Outline)   │ │
   │ │                                                                         │ │
   │ │                               [Explosive]                               │ │
   │ │                               (Orange Hex)                              │ │
   │ │                                                                         │ │
   │ │                       * (Star)                                          │ │
   │ │                                                                         │ │
   │ │                                            \  / [Laser Line Target]     │ │
   │ │                                             \/  (Dotted Cyan)           │ │
   │ │                                             /\                          │ │
   │ │                     (Proj)                 /  \                         │ │
   │ │                     [Circle]              /                             │ │
   │ │                    (Orange Core)         /                              │ │
   │ │                         \               /                               │ │
   │ │                          \ [Trail (8fr)]                                │ │
   │ │                                                                         │ │
   │ │                                        [Powerup]                        │ │
   │ │                                        (Hex Canister)                   │ │
   │ │                                                                         │ │
   │ │                                                                         │ │
   │ │                                                                         │ │
   │ │                                   ( Core )                              │ │
   │ │                               ┌──────┴──────┐                           │ │
   │ │                              /               \                          │ │
   │ │                       ______/_____ BARREL _____\______                  │ │
   │ │                      [____________ CANNON ____________]                 │ │
1040px └───────────────────────────────────────────────────────────────────────┘ │
   │ [HUD Bottom: High Score]                                                   │
1080px ───────────────────────────────────────────────────────────────────────────
```

### Visual Hierarchy & Elements Distribution
1.  **Level 1 (Highest Focus - gameplay elements)**:
    *   **Projectiles**: Circle shape, active outer glow, diameter $16\text{px}$. Pure orange core (`#ff8800`).
    *   **Bricks**: Sharp borders ($3\text{px}$ line weight). Standard: Cyan (`#00d9ff`), Armored: Pink (`#ff006e`), Explosive: Orange (`#ff8800`).
2.  **Level 2 (Mid Focus - interaction sources)**:
    *   **The Cannon**: Clean, dark grey metal structure (`#121224`) framed by a $2\text{px}$ Cyan outline. Exposed central core pulses between pink and cyan.
    *   **Powerups**: Vertical hexagons sliding down. Color: Purple (`#9d4edd`).
3.  **Level 3 (Low Focus - background and decorations)**:
    *   **Playfield Border**: $1\text{px}$ line weight, opacity $40\%$, color: Pink (`#ff006e`).
    *   **HUD Display Text**: Monospaced text aligned to top left/right edges outside the gameplay zone.
    *   **Background Grid & Stars**: $10\%$ opacity vector grids aligned behind the bricks.

---

## 3. Cannon Baseline Sheet

The Cannon is the player's primary anchor. It represents a heavy, retro-futuristic particle accelerator housing.

### State Variations

```
┌─────────────┬─────────────────────────────────┬────────────────────────────────┐
│ STATE       │ VISUAL BEHAVIOR                 │ EMISSIVE / GLOW LAYER          │
├─────────────┼─────────────────────────────────┼────────────────────────────────┤
│ Idle        │ Solid static silhouette.        │ Low energy pulse on Core (1Hz).│
├─────────────┼─────────────────────────────────┼────────────────────────────────┤
│ Moving      │ Exhaust particles exit bottom;  │ Directional thruster flare on  │
│             │ slight chassis tilt ($1.5^\circ$).│ left/right edges (80% opacity).│
├─────────────┼─────────────────────────────────┼────────────────────────────────┤
│ Firing      │ Barrel scales down (recoil),     │ Instant 100% white glow on core│
│             │ vents release orange sparks.    │ flash, decaying over 100ms.    │
├─────────────┼─────────────────────────────────┼────────────────────────────────┤
│ Damaged     │ Sparks flicker; scanline glitches│ Grid pattern breaks on body;   │
│             │ intersect the outline.          │ color changes from Cyan to Red.│
└─────────────┴─────────────────────────────────┴────────────────────────────────┘
```

### Detailed Scale & Wireframe

```
             ◄─────────────── 128px ────────────────►
             
             _.-'-._.-'-._.-'-._.-'-._   ◄─── Barrel Fins (High detail density)
            |                         |
            |   |   |   |   |   |   | |
            |   |   |   |   |   |   | |
            \                         /
             \                       /
              \    .---""""---.     /    ◄─── Central Core (Glow focal point)
               \  /            \   /
                \/  (O) CORE (O)\/
                /\              /\
       ________/  \            /  \________
      /            '---____---'            \
     /   [========= PANEL PLATE =========]  \  ◄─── Chassis (Solid dark fills,
    /________________________________________\      low detail density)
    ◄───────────────── 128px ────────────────►
```

### Implementation Notes for Artists
*   **Dimensions**: Must fit exactly within a $128\text{px} \times 128\text{px}$ frame. Anchor point: Bottom Center ($64, 128$).
*   **Materials**: Anodized charcoal metal plates (`#121224`) with highly polished surface gloss highlights. Under-lighting must project a subtle cyan reflection on the gameplay floor.
*   **Lines**: Outer profile lines must be kept at a clean $2\text{px}$ width. Avoid soft, painted gradients.

---

## 4. Brick Family Baseline

Bricks represent structural obstacles that the player shatters. Every brick category must communicate its durability, danger, or special traits through its geometry and borders.

```
       [ Standard ]             [ Armored ]             [ Explosive ]
   ┌───────────────────┐    ┌───/───────────\───┐    ┌───────────────────┐
   │  [ 45 ]  (Cyan)   │    │ ╳ [ 90 ] ╳ (Pink) │    │  ▲▲ [ 15 ] ▲▲(Org)│
   └───────────────────┘    └───\───────────/───┘    └───────────────────┘
```

### Standard Brick Specification
*   **Silhouette**: Rectangle with clean $45^\circ$ beveled corners ($4\text{px}$ cuts).
*   **Dimensions**: $64\text{px} \times 32\text{px}$.
*   **Borders**: $2\text{px}$ solid Cyan line (`#00d9ff`).
*   **Fill**: Transparent Dark BG (`#0a0a1a`) at $80\%$ opacity.
*   **Zoom Detail**: A small digit displaying durability in the center using Rajdhani font.

### Armored Brick Specification
*   **Silhouette**: Faceted octagon with double structural trim lines.
*   **Dimensions**: $64\text{px} \times 32\text{px}$.
*   **Borders**: Outer line $2\text{px}$ Pink (`#ff006e`), inner line $1\text{px}$ Pink.
*   **Fill**: Dark pink core tint ($15\%$ opacity).
*   **Zoom Detail**: Inner cross-brace line elements supporting the corners.

### Explosive Brick Specification
*   **Silhouette**: Sharp rectangle with danger stripes on left and right margins.
*   **Dimensions**: $64\text{px} \times 32\text{px}$.
*   **Borders**: $3\text{px}$ solid Orange line (`#ff8800`).
*   **Fill**: Heavy diagonal hazard grid stripes across the center face.
*   **Glow**: Active pulse glow cycle between $40\%$ and $100\%$ opacity.

### Unbreakable Brick Specification
*   **Silhouette**: Heavy block with solid inset corner brackets.
*   **Dimensions**: $64\text{px} \times 32\text{px}$.
*   **Borders**: $2\text{px}$ Metallic Gold line (`#e5c158`).
*   **Fill**: Solid dark charcoal carbon texture (`#1f1f2e`). No interior glow.

### Boss Shield Segment Specification
*   **Silhouette**: Arc-curved polygon segment designed to rotate in orbits around the central boss core.
*   **Dimensions**: $128\text{px} \times 24\text{px}$ bounding box.
*   **Borders**: Thick $3\text{px}$ line weight in secondary Pink (`#ff006e`).
*   **Fill**: Honeycomb structure visible through a semi-transparent glass panel.

---

## 5. Projectile Baseline

Projectiles are high-velocity energy spheres designed to pierce or bounce off data cores.

```
       ( Core Area )   ──► Circle shape, 6px diameter. Color: White (#ffffff)
       ( Plasma Envelope ) ──► Outer Ring, 16px diameter. Color: Orange (#ff8800)
       ( Motion Trail ) ──► Tapering polygon, 8-frame memory history.
```

### Visual Specifications
*   **Shape**: Perfect circle (provides highest contrast against brick angles).
*   **Diameter**: $16\text{px}$.
*   **Colors**:
    *   **Core**: Pure White (`#ffffff`), $6\text{px}$ diameter.
    *   **Envelope**: Intense Orange (`#ff8800`), fading to red at the outer edge.
*   **Trail**: Constructed from 8 historical segment coordinates. Trail width reduces from $16\text{px}$ down to $0\text{px}$ at the trailing node. Opacity fades linearly:

$$\text{Opacity}(n) = 1.0 - \left(\frac{n}{8}\right)$$

---

## 6. Powerup Baseline

Powerups drop from destroyed cores as data canisters that the player collects to modify cannon fire.

```
                    ┌───────┐
                   /  ( | )  \   ◄─── Canister Top
                  /   ( | )   \
                 |  ┌───────┐  |
                 |  │ ( O ) │  |  ◄─── Function Icon (e.g. Multi-shot)
                 |  └───────┘  |
                  \           /
                   \         /   ◄─── Canister Bottom
                    └───────┘
```

### Canister Proportions
*   **Shape**: Elongated vertical hexagon.
*   **Dimensions**: $32\text{px} \times 48\text{px}$.
*   **Borders**: $2\text{px}$ line in Accent Purple (`#9d4edd`).
*   **Fill**: Translucent glass casing (`#120a24` at $70\%$ opacity).

### Iconography and Colors
*   **Multi-Shot Canister**: Icon displays three arrows branching upwards from a single point. Core color: Purple (`#9d4edd`).
*   **Laser Sight Canister**: Icon displays a thin vertical dotted pointer line. Core color: Cyan (`#00d9ff`).
*   **Shield Canister**: Icon displays a wide horizontal bracket. Core color: Green (`#06ffa5`).

---

## 7. Environment Baseline

The environment provides a deep virtual canvas that anchors the playfield without distracting from gameplay.

### Environment Composition Diagram

```
┌────────────────────────────────────────────────────────────────────────┐
│ [Layer 3: Background Nebula] (Speed: 0.1)                              │
│  - Soft large circular nodes (Blur: 32px), Opacity: 5%. Hues: Purple.  │
│                                                                        │
│       [Layer 2: Midground Stars] (Speed: 0.4)                          │
│        - Drifting octagonal stars, Opacity: 15%. Hues: Cyan & Pink.    │
│                                                                        │
│             [Layer 1: Playfield Grid] (Speed: 1.0)                     │
│              - Orthogonal grid lines, Opacity: 10%. Color: Dark Blue.  │
└────────────────────────────────────────────────────────────────────────┘
```

### Operational Metrics
1.  **Background Void**: Flat fill color: `#0a0a1a`.
2.  **Playfield Grid**: Lines are placed at $64\text{px}$ intervals. Line width $1\text{px}$, color: `#151535`, opacity: $10\%$.
3.  **Midground Stars**: Drifting downwards at a speed of $30\text{px/s}$. Max active stars on screen: $40$. Star sizes range from $2\text{px}$ to $4\text{px}$.
4.  **Nebula Fields**: Three static, overlapping radial color fields located in the upper third of the canvas. Color: HSL Purple (`#9d4edd`) at $5\%$ opacity.

### Visual Quality Review Board

```
┌────────────────────────────────────────┬───────────────────────────────┐
│ APPROVED                               │ REJECTED                      │
├────────────────────────────────────────┼───────────────────────────────┤
│ Flat vector grid lines aligned to axes.│ Curving grid lines or grids   │
│                                        │ drawn in perspective.         │
├────────────────────────────────────────┼───────────────────────────────┤
│ Sharp octagonal star sprites.          │ Blurred round star sprites.   │
├────────────────────────────────────────┼───────────────────────────────┤
│ Monochrome dark void backing.          │ Photographic space nebulae or │
│                                        │ city skyline backdrops.       │
└────────────────────────────────────────┴───────────────────────────────┘
```

---

## 8. UI Baseline

The UI must present high-tech telemetry data layout, maintaining a clean workspace.

### HUD Specifications
*   **Active Margins**: Top: $24\text{px}$, Left: $24\text{px}$, Right: $24\text{px}$, Bottom: $24\text{px}$.
*   **Fonts**:
    *   Labels (e.g., "SCORE", "LEVEL"): Orbitron Bold, $14\text{pt}$, color: `#ff006e`.
    *   Numbers/Values: Rajdhani Regular, $24\text{pt}$, color: `#00d9ff`.

### Main Menu Wireframe Layout

```
┌─────────────────────────────────── 1920px ───────────────────────────────────┐
│                                                                              │
│                         [ CYBERPUNK CANNON SHOOTER ]                         │
│                         (Orbitron Bold, 64pt, Cyan)                          │
│                                                                              │
│                                                                              │
│                            ┌───────────────────┐                             │
│                            │   [ START GRID ]  │  ◄── Button: 300px x 60px   │
│                            └───────────────────┘      Beveled Cuts (12px)    │
│                            ┌───────────────────┐                             │
│                            │    [ UPGRADES ]   │                             │
│                            └───────────────────┘                             │
│                            ┌───────────────────┐                             │
│                            │    [ TERMINAL ]   │                             │
│                            └───────────────────┘                             │
│                            ┌───────────────────┐                             │
│                            │    [ DISCONNECT ] │                             │
│                            └───────────────────┘                             │
│                                                                              │
│ [Terminal Status: ONLINE]                                [System Latency: 1ms]│
└──────────────────────────────────────────────────────────────────────────────┘
```

### Menu Design System Elements
*   **Buttons**:
    *   Normal: Dark blue body (`#15152a`), $2\text{px}$ Cyan outline (`#00d9ff`).
    *   Hover: Glowing cyan body (`#00d9ff`), text color changes to dark void (`#0a0a1a`).
    *   Active/Pressed: Pink body (`#ff006e`), white text.
*   **Panel Framing**: Rectangular boxes with $12\text{px}$ corner cuts on diagonal corners ($45^\circ$), with border lines left open at the edges to suggest technical schematics.

---

## 9. VFX Baseline

Visual effects must provide immediate feedback on impact without obscuring the collision states of gameplay.

```
┌─────────────────┬──────────────────────┬──────────┬───────────┬──────────────┐
│ EVENT           │ PARTICLE SHAPE / QTY │ DURATION │ OPACITY   │ GLOW LAYER   │
├─────────────────┼──────────────────────┼──────────┼───────────┼──────────────┤
│ Projectile Trail│ 8 rectangle segments │ Fades    │ 100% to 0%│ Additive     │
│                 │ (tapering width)     │ linearly │           │ blending.    │
├─────────────────┼──────────────────────┼──────────┼───────────┼──────────────┤
│ Brick Hit       │ 8-12 sharp shards    │ 150ms    │ Fades     │ Pulse on     │
│                 │ (2px x 2px square)   │          │ quickly   │ shards.      │
├─────────────────┼──────────────────────┼──────────┼───────────┼──────────────┤
│ Brick Destroy   │ 20-30 vector blocks  │ 250ms    │ Decays    │ Radial flash │
│                 │ (4px x 4px maximum)  │          │ linearly  │ (100% bloom) │
├─────────────────┼──────────────────────┼──────────┼───────────┼──────────────┤
│ Powerup Pickup  │ 1 expanding ring     │ 200ms    │ Tapers    │ Glow color:  │
│                 │ (10px to 128px)      │          │ to zero   │ Purple.      │
├─────────────────┼──────────────────────┼──────────┼───────────┼──────────────┤
│ Warning / Crit  │ Scanline glitches    │ Active   │ Periodic  │ Red flashing │
│                 │ (1px offset shifts)  │ during   │ wave      │ frame overlay│
└─────────────────┴──────────────────────┴──────────┴───────────┴──────────────┘
```

### VFX Layout Details
*   **Brick Destruction Particles**: Sprites are square fragments matching the parent brick's border color. Physics: initial speed $150\text{px/s}$ blasting outwards radially from the collision point, decelerating rapidly with a downward gravity pull of $400\text{px/s}^2$.
*   **Powerup Ring**: Scaled dynamically via:

$$\text{Radius}(t) = 10.0 + 118.0 \cdot \left(\frac{t}{200\text{ms}}\right)$$

Line thickness decreases from $4\text{px}$ to $0\text{px}$ as time approaches $200\text{ms}$.

---

## 10. Animation Baseline

Arcade games demand responsive animation structures. All animations are mapped to the timeline to ensure timing consistency.

### Animation Timeline Chart

```
Frame Index:  0  1  2  3  4  5  6  7  8  9  10 ... 15 ... 20 (Time at 60fps)
─────────────────────────────────────────────────────────────────────────────
Recoil:       [====== Barrel Compress (5%) ======]
              [== Recoil Expand (15%) ==]
              [================ Elastic Settle (100%) ================]
─────────────────────────────────────────────────────────────────────────────
Impact:       [Flash (100% White)]
              [============ Shards Blast ============]
              [==================== Shake Decays ====================]
```

### Timing and Physics Guidelines
*   **Cannon Recoil**:
    *   Compression (0 to 3 frames): Barrel height scales down to $95\%$ using ease-in-out interpolation.
    *   Release (3 to 5 frames): Barrel height stretches to $115\%$ using linear interpolation.
    *   Recovery (5 to 15 frames): Barrel returns to $100\%$ scale using elastic easing.
*   **Screen Shake (Decaying Sine Wave)**:
    *   Initiated on Brick Destruction.
    *   Start Offset: $8\text{px}$ max.
    *   Decay: $\lambda = 12.0$ (exponential decay).
    *   Total duration: 9 frames ($150\text{ms}$).

---

## 11. Color Reference Baseline

To prevent visual drift across development platforms, artists must utilize these exact values.

```
┌──────────────┬─────────┬───────────────────┬───────────────────┬──────────────┐
│ DESIGN NAME  │ HEX     │ RGB               │ HSL               │ SAMPLE USE   │
├──────────────┼─────────┼───────────────────┼───────────────────┼──────────────┤
│ Primary      │ #00d9ff │ RGB(0, 217, 255)  │ HSL(189, 100%, 50%)│ Cannon Outline│
├──────────────┼─────────┼───────────────────┼───────────────────┼──────────────┤
│ Secondary    │ #ff006e │ RGB(255, 0, 110)  │ HSL(334, 100%, 50%)│ Boss Borders  │
├──────────────┼─────────┼───────────────────┼───────────────────┼──────────────┤
│ Accent       │ #9d4edd │ RGB(157, 78, 221) │ HSL(274, 61%, 59%) │ Powerup Glow │
├──────────────┼─────────┼───────────────────┼───────────────────┼──────────────┤
│ Danger       │ #ff8800 │ RGB(255, 136, 0)  │ HSL(32, 100%, 50%) │ Projectiles  │
├──────────────┼─────────┼───────────────────┼───────────────────┼──────────────┤
│ Reward       │ #06ffa5 │ RGB(6, 255, 165)  │ HSL(157, 100%, 50%)│ Score Popups │
├──────────────┼─────────┼───────────────────┼───────────────────┼──────────────┤
│ Void BG      │ #0a0a1a │ RGB(10, 10, 26)   │ HSL(240, 44%, 7%)  │ Canvas Fill  │
└──────────────┴─────────┴───────────────────┴───────────────────┴──────────────┘
```

### Forbidden Usages
*   Do not overlay Pink text on red backgrounds (breaks accessibility/contrast).
*   Do not use `#000000` (pure black) for backgrounds. Always back gameplay with the Dark Void (`#0a0a1a`) to ensure glows blend correctly.
*   Do not use gradients that mix opposing temperatures (e.g., green-to-orange) on a single brick face. Keep gradients confined to hues within the same palette category.

---

## 12. Asset Scale Reference Sheet

This scale comparison layout defines the size ratios of all assets relative to each other on a single unified canvas.

```
                             SCALED GEOMETRIES
─────────────────────────────────────────────────────────────────────────────
  CANNON [128px x 128px]        BOSS SEGMENT [128px x 24px]
  ┌───────────────────┐         ┌─────────────────────────┐
  │                   │         │                         │
  │                   │         └─────────────────────────┘
  │       (O)         │
  │                   │         STANDARD BRICK [64px x 32px]
  │   ┌───────────┐   │         ┌───────────┐
  │  /             \  │         │  [ 250 ]  │
  └─/───────────────\─┘         └───────────┘
                                POWERUP [32px x 48px]  PROJECTILE [16px]
                                ┌───────┐
                                │  ( )  │               O
                                └───────┘
─────────────────────────────────────────────────────────────────────────────
```

---

## 13. Visual Approval Board

This checklist allows reviewers, technical artists, and outsourcing managers to instantly validate submissions.

### Geometry & Shapes

```
┌────────────────────────────────────────┬───────────────────────────────┐
│ APPROVED                               │ REJECTED                      │
├────────────────────────────────────────┼───────────────────────────────┤
│ 45-degree angle corner cuts.           │ Rounded corners.              │
├────────────────────────────────────────┼───────────────────────────────┤
│ Vector flat line profiles.             │ Organic hand-drawn strokes.   │
├────────────────────────────────────────┼───────────────────────────────┤
│ Convex octagonal/hexagonal silhouettes.│ Symmetrical circles for bricks│
│                                        │ or UI icons.                  │
└────────────────────────────────────────┴───────────────────────────────┘
```

### Color & Glow

```
┌────────────────────────────────────────┬───────────────────────────────┐
│ APPROVED                               │ REJECTED                      │
├────────────────────────────────────────┼───────────────────────────────┤
│ Core glows that use additive blending. │ Core glows that overlay grey  │
│                                        │ drop shadows or black edges.  │
├────────────────────────────────────────┼───────────────────────────────┤
│ Colors mapped to functional groups     │ Muted olive greens, browns,   │
│ (Cyan = player, Orange = bullet).      │ or pastels.                   │
├────────────────────────────────────────┼───────────────────────────────┤
│ Outer glow fading cleanly to 0% opacity│ Jagged glow outlines with     │
│ over 3 blending passes.                │ visible texture banding.      │
└────────────────────────────────────────┴───────────────────────────────┘
```

### UI & UX Layouts

```
┌────────────────────────────────────────┬───────────────────────────────┐
│ APPROVED                               │ REJECTED                      │
├────────────────────────────────────────┼────────────────────────◄──────┤
│ 24px margin alignments.                │ Freefloating elements or skewed│
│                                        │ text boxes.                   │
├────────────────────────────────────────┼───────────────────────────────┤
│ Monospaced technical typography.       │ Serif or cursive font faces.  │
├────────────────────────────────────────┼───────────────────────────────┤
│ HUD items aligned outside active zones.│ Gameplay items rendering behind│
│                                        │ HUD plates without contrast.  │
└────────────────────────────────────────┴───────────────────────────────┘
```

---

## 14. Art Baseline Freeze Rules

To ensure that the visual pipeline remains stable and assets remain consistent across multiple development environments, the following rules are **frozen** and cannot be changed without an official Art Direction revision process:

1.  **Strict Coordinate Alignments**: The active gameplay arena bounds must remain locked at $1200\text{px} \times 1000\text{px}$. Art assets must be framed exactly to these aspect constraints.
2.  **No Organic Texturing**: Sprites must not feature textures simulating wood, rust, mud, stone, or human skin. All textures must represent clean digital interfaces, glass, or metals.
3.  **Strict Color Mappings**: Projectiles must remain exclusively inside the Danger Orange (`#ff8800`) color boundaries. At no point may the player fire cyan or pink bullets, as these hues are reserved for the Cannon and Boss components.
4.  **Vector Outlines Requirement**: All interactive entities (bricks, player ship, bumpers) must carry an outline border with a minimum line weight of $2\text{px}$ to preserve readability. Borderless assets will be rejected.
5.  **No Scale Deviations**: Assets must be delivered in their exact target resolutions (e.g., standard bricks must be exactly $64\text{px} \times 32\text{px}$ files). Scaling up low-resolution sheets at runtime is prohibited.

---
*End of Art Baseline v1.0 Specification*
