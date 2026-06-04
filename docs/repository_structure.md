# Repository Structure Manual (repository_structure.md)

This document defines the folder layout, module boundaries, and file location rules for the Cyberpunk Cannon Shooter codebase. Maintaining a structured repository prevents architectural drift and ensures developer consistency.

---

## 1. Folder Tree Hierarchy

```
BrickBreaker/
  ├── .github/                  # GitHub configurations (CI/CD workflows)
  ├── assets/                   # Loose asset files (shipped with the project)
  │     ├── textures/           # Sprites, UI icons, decals (.png)
  │     ├── audio/              # Sound effects, soundtrack loop (.wav, .ogg)
  │     ├── fonts/              # Typography styles (.ttf, .otf)
  │     └── shaders/            # GLSL post-processing shaders (.vert, .frag)
  ├── docs/                     # Technical specifications and manuals
  │     └── adr/                # Architecture Decision Records
  ├── include/                  # Public C++ header interface files (.hpp)
  │     ├── gameplay/           # Math bounds, physics, entity simulation
  │     ├── rendering/          # Views, layer pipelines, camera coordinators
  │     ├── persistence/        # File serialization, profile managers
  │     └── ui/                 # HUD panels, menu overlays, button widgets
  ├── src/                      # Concrete C++ implementation files (.cpp)
  │     ├── gameplay/
  │     ├── rendering/
  │     ├── persistence/
  │     └── ui/
  ├── tests/                    # Catch2 unit and integration test suites
  │     ├── gameplay/
  │     ├── persistence/
  │     ├── integration/
  │     └── fakes/              # Test doubles (stubs, mock providers)
  ├── tools/                    # Build-time python tools (packers, validators)
  ├── CMakeLists.txt            # Root build configuration
  └── CONTRIBUTING.md           # Onboarding developer workflows
```

---

## 2. Directory Allocations & Responsibilities

### 1. `include/` and `src/` (Gameplay core modules)
The C++ source directories are split into four primary architectural namespaces to prevent circular referencing:

* **`gameplay/`**:
  - *Responsibilities*: Physics updates, collision resolution, spatial queries (AABBs), wave directors, score computation, and core coordinate models.
  - *Invariants*: Must remain completely graphics-agnostic. No references to SFML rendering components or file I/O operations are allowed.
* **`rendering/`**:
  - *Responsibilities*: Entity View controllers (mapping models to sprites), starfield animations, particle emitter buffers, and post-processing shader queues.
  - *Invariants*: Depends on `gameplay/` via read-only const references. Passes coordinates to the GPU draw pipeline.
* **`persistence/`**:
  - *Responsibilities*: Profile save files, settings configurations, and high-score logs serialization.
  - *Invariants*: Reads state snapshots from `gameplay/` and writes them atomically using platform-specific paths.
* **`ui/`**:
  - *Responsibilities*: Translucent settings overlays, health meters, pause screens, and typography formatting.
  - *Invariants*: Draws directly to the static UI viewport (`sf::View`). Receives updates via the state manager.

### 2. `assets/` (Loose Resources)
Contains the raw, unpackaged assets utilized during development and local testing.
* **Invariants**: Assets must be added to the registry metadata (`assets_manifest.json`) using unique string IDs before they can be requested in code.

### 3. `tests/` (Verification Suites)
Contains Catch2 test cases.
* **Invariants**: The folder layout inside `tests/` must mirror the structure of `include/`/`src/` to make related tests easy to locate.

### 4. `tools/` (Pipeline scripts)
Contains scripts utilized for build-time operations:
- `packer.py`: Compresses loose folders inside `assets/` into a single compressed binary package (`assets.pack`).
- `verify_assets.py`: Validates manifest keys, formatting bounds, and licensing compliances.

## 3. Internal Architecture Dependency Flow

To visualize the software boundaries, the dependency flow moves sequentially downward through layers:

```
  [UI / HUD Layer]           ──► draws gauges, score labels, menus
         │
         ▼ (delegates inputs and frame ticks)
  [StateStack Coordinator]   ──► pushes/pops State interfaces (MainMenu, Playing)
         │
         ▼ (updates physics and logic step ticks)
  [GameplayWorld Simulation] ──► coordinates entities (Cannon, Bricks, Projectiles)
         │
         ▼ (serializes snapshots atomically)
  [Persistence Services]     ──► IPersistenceProvider, SaveManager, file outputs
```

---

## 4. File Allocation Guidelines

Reviewers must enforce these location boundaries during code evaluations:

| Code Content | Target Header Location | Target Implementation Location |
| :--- | :--- | :--- |
| Simulation model variables | `include/gameplay/` | `src/gameplay/` |
| View draw submissions | `include/rendering/` | `src/rendering/` |
| Serialization streams | `include/persistence/` | `src/persistence/` |
| HUD layouts | `include/ui/` | `src/ui/` |
| Catch2 assertions | None | `tests/<module>/` |
| Python packer steps | None | `tools/` |
