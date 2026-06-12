# Architecture Compliance Audit Report

This document presents an independent architecture compliance audit of the Cyberpunk Cannon Shooter codebase. The evaluation measures implementation alignment against the official project specifications, standards, and Architecture Decision Records (ADRs).

---

# Executive Summary

### Compliance Score: **26.9%** (35 / 130 Points)

The project exhibits severe **Architectural Drift** and **Subsystem Decay**. While the compilation pipeline builds successfully on Linux, macOS, and Windows, almost all core design invariants, separation of concerns (SoC), memory constraints, and quality gates specified in the documentation are currently violated or entirely missing.

*   **ADR Compliance**: 10.0% (1 PARTIAL, 4 FAIL)
*   **Documentation Compliance**: 37.5% (6 PARTIAL, 2 FAIL)

| Audit Category | Status | Assigned Score |
| :--- | :---: | :---: |
| [ADR-0001: Screen Routing via State Stack](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/adr/ADR-0001-state-stack.md) | **PARTIAL** | 5 / 10 |
| [ADR-0002: Entity-View Separation](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/adr/ADR-0002-entity-view-separation.md) | **FAIL** | 0 / 10 |
| [ADR-0003: Dependency-Injected Resource Manager](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/adr/ADR-0003-resource-manager.md) | **FAIL** | 0 / 10 |
| [ADR-0004: Synchronous Event Dispatcher](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/adr/ADR-0004-event-system.md) | **FAIL** | 0 / 10 |
| [ADR-0005: Decoupled Persistence Architecture](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/adr/ADR-0005-persistence-architecture.md) | **FAIL** | 0 / 10 |
| [architecture_overview.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/architecture_overview.md) | **PARTIAL** | 5 / 10 |
| [coding_standards.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/coding_standards.md) | **PARTIAL** | 5 / 10 |
| [reviewing-guidelines.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/reviewing-guidelines.md) | **PARTIAL** | 5 / 10 |
| [testing_strategy.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/testing_strategy.md) | **FAIL** | 0 / 10 |
| [repository_structure.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/repository_structure.md) | **PARTIAL** | 5 / 10 |
| [build_deployment.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/build_deployment.md) | **PARTIAL** | 5 / 10 |
| [ci_cd_pipeline.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/ci_cd_pipeline.md) | **PARTIAL** | 5 / 10 |
| [assets_manifest_spec.md](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/docs/assets_manifest_spec.md) | **FAIL** | 0 / 10 |

---

## Critical Issues (Blocks Merge/Release)

1.  **Direct Graphics and SFML Coupling (ADR-0002)**:
    Gameplay entities ([Cannon](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Cannon.h), [Brick](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Brick.h), [Projectile](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Projectile.h)) directly own SFML visual assets, shapes, fonts, and textures. They handle rendering animations and execute raw `window.draw()` calls internally. This prevents headless compilation and isolates gameplay simulation from test harnesses.
2.  **Complete Absence of Testing Suite (testing_strategy.md)**:
    The [tests/](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/tests) directory contains no unit, integration, property-based, or Golden Master tests. The [CMakeLists.txt](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/tests/CMakeLists.txt) is a placeholder, and CTest runs on zero assertions.
3.  **Static Singletons and Hardcoded Paths (ADR-0003)**:
    `FontManager` and `AudioManager` are static singleton classes. Disk paths (e.g., `"assets/fonts/Orbitron-Bold.ttf"`, `"assets/audio/cyberpunk_theme.wav"`) are hardcoded directly in C++ files. There is no constructor injection, no `ResourceManager<T>` template, and no central `assets_manifest.json`.
4.  **Missing Event Dispatcher (ADR-0004)**:
    The synchronous message bus `EventDispatcher` is not implemented. Systems are tightly coupled. For example, [Cannon.cpp](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/src/entities/Cannon.cpp#L253) calls `AudioManager::getInstance().playSound()` directly.
5.  **Coupled Non-Atomic Persistence (ADR-0005)**:
    There is no decoupled save system. Highscores are loaded and saved inline in [PlayingState.cpp](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/src/core/states/PlayingState.cpp#L871-L888) using raw `std::ifstream` and `std::ofstream` streams targeting a hardcoded file in the working directory, with no atomic rename guarantees.

---

## Major Issues

1.  **C++ Standard Misalignment**:
    The [CMakeLists.txt](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/CMakeLists.txt#L5) sets `CMAKE_CXX_STANDARD 17`, while `build_deployment.md` mandates C++20 standard structures.
2.  **File Naming and Extension Violations**:
    All header files use the `.h` extension instead of the `.hpp` extension required by `coding_standards.md` Part C.
3.  **Invalid Repository Subfolder Layout**:
    Source files reside in `/src/entities`, `/src/ui`, `/src/core`, `/src/managers` instead of the mandated `/src/gameplay`, `/src/rendering`, `/src/persistence`, etc.
4.  **Doxygen API Gaps**:
    Many public interfaces (such as [Button.h](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/ui/Button.h)) lack `@param` and `@return` documentation.
5.  **Hot-Path Heap Allocations**:
    Loops dynamically instantiate and return vectors of pointers by value (e.g., `BlockManager::getActiveBlocks()`, `ProjectilePool::getActiveProjectiles()`, and `Block::getBricks()`) every frame.

---

## Minor Issues

1.  **Standard Output Pollution**:
    Classes print messages directly to `std::cout` and `std::cerr` rather than utilizing logging macros.
2.  **Coupled State Stack**:
    The state stack is implemented inline within the `Game` class rather than as a decoupled `StateStack` coordinator.

---

# ADR Compliance Review

## ADR-0001: Screen Routing via State Stack
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   No standalone `StateStack` coordinator class exists.
    *   State routing vector `stateStack_` and command handler `pendingStateChange_` are owned and managed directly inside the [Game](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/Game.h#L129-L132) container.
    *   The base state interface is named `GameState` instead of `State`.
    *   State methods require reference to `sf::RenderWindow` directly rather than `sf::RenderTarget` (limiting offscreen rendering compatibility).
*   **Architectural Drift**:
    The state management routines are partially deferred but lack a dedicated, testable coordinator class.

## ADR-0002: Entity-View Separation for Graphics Isolation
*   **Compliance Level**: **FAIL**
*   **Violations**:
    *   There is zero separation between game simulation models and rendering view managers.
    *   [Cannon](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Cannon.h), [Brick](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Brick.h), and [Projectile](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Projectile.h) directly include `<SFML/Graphics.hpp>` and inherit or embed visual components like `sf::RectangleShape`, `sf::CircleShape`, and `sf::Text`.
    *   Gameplay models own their drawing configurations, colors, and animation states, and contain `render(sf::RenderWindow&)` methods with direct window draw calls.
*   **Architectural Drift**:
    This is the most severe violation. The core static library (`GameplayCore`) cannot be built headlessly or tested without linking graphics libraries, completely violating the primary goal of the ADR.

## ADR-0003: Dependency-Injected Resource Manager
*   **Compliance Level**: **FAIL**
*   **Violations**:
    *   No template-based `ResourceManager<T>` class exists.
    *   `FontManager` is implemented as a static class containing static `std::unique_ptr<sf::Font>` variables.
    *   `AudioManager` is implemented as a static singleton accessed via `AudioManager::getInstance()`.
    *   File paths (e.g., `"assets/fonts/Orbitron-Bold.ttf"`, `"assets/audio/cyberpunk_theme.wav"`) are hardcoded directly in C++ methods.
    *   There is no `assets_manifest.json` file in the assets directory.
*   **Architectural Drift**:
    Dependency injection is completely bypassed. Classes access resources globally via static hooks, violating test isolation and deterministic lifetime constraints.

## ADR-0004: Synchronous Event Dispatcher
*   **Compliance Level**: **FAIL**
*   **Violations**:
    *   No `EventDispatcher` class exists.
    *   No event schemas or data structs (e.g., `BrickDestroyedEvent`, `ProjectileFiredEvent`) are defined.
    *   Subsystems are tightly coupled: gameplay classes directly invoke external singletons (e.g., [Cannon.cpp](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/src/entities/Cannon.cpp#L253) calls `AudioManager::getInstance().playSound()`).
*   **Architectural Drift**:
    The lack of an event bus forces gameplay classes to know about sound and score managers, breaking the Open-Closed Principle and making headless execution impossible.

## ADR-0005: Decoupled Persistence Architecture
*   **Compliance Level**: **FAIL**
*   **Violations**:
    *   No `IPersistenceProvider`, `DiskPersistenceProvider`, or `SaveManager` classes exist.
    *   High score loading and saving are implemented inline in [PlayingState.cpp](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/src/core/states/PlayingState.cpp#L871-L888) using raw `std::ifstream` and `std::ofstream` streams.
    *   The file path `"highscore.txt"` is hardcoded and accessed directly in the local folder.
    *   Writes are executed inline on the main thread without atomic rename operations (creating data corruption risks).
*   **Architectural Drift**:
    Persistence is directly coupled to standard C++ filesystem tools inside gameplay state loops, preventing mock-testing saving behaviors.

---

# Documentation Compliance Review

## architecture_overview.md
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   Diagrammed dependency flow does not match code. The UI layer directly pulls from coupled entities.
    *   Modules listed (e.g., `Asset Pipeline`, `Save System`, `Event System`) do not exist as decoupled libraries.

## coding_standards.md
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   **C++ Version**: Configured as C++17 instead of C++20.
    *   **Header Extensions**: Every header uses `.h` instead of the `.hpp` extension required by `coding_standards.md` Part C.
    *   **Directory Layout**: Code files are not located in modules like `/src/gameplay`, `/src/rendering`, or `/src/persistence`.
    *   **Singletons**: `FontManager` and `AudioManager` violate the singleton prohibition.
    *   **No File I/O during Gameplay**: Save triggers occur inline during game ticks.
    *   **Stdout Prints**: Production code is littered with raw `std::cout` and `std::cerr` print statements.
    *   **Doxygen Rules**: Public methods in [Button.h](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/ui/Button.h) and elsewhere are missing Doxygen tags.
    *   **Hot-Loop Heap Allocations**: Frame updates resize and return collections by value.

## reviewing-guidelines.md
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   Merge-blocking conditions (no singletons, no graphics in gameplay, no direct file I/O, no raw pointer leaks) are present throughout the active branch.
    *   The 80% Coverage Diff Gate is bypassed since there are no test scripts.

## testing_strategy.md
*   **Compliance Level**: **FAIL**
*   **Violations**:
    *   There is zero testing code. The `tests/` folder contains no source files.
    *   Catch2 v3, unit tests, mock fakes, replay logging, property tests, and Golden Master render captures are completely absent.
    *   There are no coverage indicators running in CI.

## repository_structure.md
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   The physical subfolders on disk do not match the documented layout tree (e.g., `/include/gameplay`, `/src/rendering`, `/tools` do not exist).
    *   Raw asset registry files (`assets_manifest.json`) are missing.

## build_deployment.md
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   The project builds with C++17 flags rather than C++20.
    *   Release symbols stripping and packaging options (.zip, .tar.gz, .dmg wraps) are not verified due to the lack of test binaries and asset pack scripts.

## ci_cd_pipeline.md
*   **Compliance Level**: **PARTIAL**
*   **Violations**:
    *   The `Run Automated Tests` workflow step compiles successfully but executes zero test cases.
    *   Lint checks (`clang-format` and `clang-tidy`) execute, but code format overrides were required manually since local tooling wasn't configured.

## assets_manifest_spec.md
*   **Compliance Level**: **FAIL**
*   **Violations**:
    *   No `assets_manifest.json` catalog file is present.
    *   Assets are loaded via hardcoded local path string literals.
    *   Python verify scripts (`verify_assets.py`) and packaging utilities (`packer.py`) are missing.

---

# Architectural Risks

1.  **Platform Crashing on Missing Assets**:
    Since textures and fonts are loaded directly from loose disk paths via static wrappers without a fallback resource manager cache, if a file is missing or misplaced, SFML will log an error to standard error and fail. This can trigger crashes or null references during execution.
2.  **Save File Corruption on Interruption**:
    Writing to `highscore.txt` directly via standard streams on the main thread can leave the file half-written and corrupted if a crash or power interruption occurs during the write call.
3.  **High Dependency Coupling**:
    Because gameplay models hold direct dependencies on audio, font, and window managers, any change to the rendering pipeline or sound execution requires recompiling the gameplay simulation, increasing the likelihood of regression bugs.
4.  **No Core Verification Safety**:
    The absence of automated unit tests means gameplay math (AABB checks, normal vector bounces) has no validation safety nets, creating regression risks if physics code is updated.

---

# Technical Debt

1.  **C++17 vs. C++20 Core Drift**:
    Building the project under C++17 limits the use of concepts, ranges, spans, and atomic wait primitives described in the build/deployment manuals, which will complicate future integrations.
2.  **Unmanaged Heap allocations on Hot Paths**:
    Constructing vectors of raw pointers by value every frame in collision loops (`Block::getBricks()`, etc.) causes substantial garbage memory generation, forcing frequent memory manager splits and increasing CPU frame times.
3.  **Header (.h vs .hpp) Discrepancy**:
    Using `.h` files throughout the codebase diverges from the C++ standard manual, making it difficult for automated tools to distinguish between C and C++ interfaces.
4.  **Stdout/Stderr Logging Pollution**:
    The lack of a unified logging macro system forces developers to write raw stream output commands (`std::cout`, `std::cerr`), which pollutes the developer console and makes redirecting outputs to disk files impossible.

---

# Missing Systems

1.  **Event System**: Synchronous `EventDispatcher` message bus and payloads.
2.  **Instanced Resource Managers**: Constructor-injected `ResourceManager<T>` caches and `assets_manifest.json` registry.
3.  **Decoupled Save System**: `IPersistenceProvider` interface, concrete disk providers, and `SaveManager`.
4.  **Catch2 Testing Suite**: Standalone `tests_run` runner target, unit scripts under `tests/`, and mock fakes.
5.  **Build Tools**: Python packaging scripts (`packer.py` and `verify_assets.py`) in `/tools`.
6.  **StateStack Coordinator Class**: Standalone state overlay machine.

---

# Required Refactoring Work

## Priority 1 (Critical)

1.  **Implement Entity-View Separation**:
    *   Refactor [Cannon](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Cannon.h), [Brick](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Brick.h), and [Projectile](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Projectile.h) to remove all SFML shapes, sprites, texts, colors, and render calls.
    *   Create view classes (`CannonView`, `BrickView`, `ProjectileView`) in a rendering package to handle visual representations and draw commands.
2.  **Establish Catch2 Test Target**:
    *   Initialize the Catch2 v3 test project under `tests/`.
    *   Write initial math and collision unit tests to ensure `ctest` executes assertions in CI.
3.  **Upgrade compiler target to C++20**:
    *   Update standard definitions to `CMAKE_CXX_STANDARD 20` inside `CMakeLists.txt`.

## Priority 2 (Important)

1.  **Create Instanced Resource Managers**:
    *   Define a templated `ResourceManager<T>` class.
    *   Create `assets_manifest.json` mapping string IDs to paths.
    *   Refactor `Game` to own instanced resource managers and inject them into state/view constructors.
2.  **Define and Integrate the Event System**:
    *   Implement `EventDispatcher` and inject it into subsystems.
    *   Define POD event payloads (`BrickDestroyedEvent`, `ProjectileFiredEvent`, `PlaySoundEvent`).
    *   Decouple `Cannon` from `AudioManager` by emitting `PlaySoundEvent` signals instead of calling it directly.
3.  **Decouple the Persistence Layer**:
    *   Implement the `IPersistenceProvider` interface and concrete disk rename streams.
    *   Replace inline filesystem I/O in `PlayingState.cpp` with a constructor-injected `SaveManager`.

## Priority 3 (Future)

1.  **Rename Header Files and Rearrange Directories**:
    *   Rename all `.h` headers to `.hpp`.
    *   Reorganize folders on disk to match documented structures (moving simulation to `/src/gameplay` and drawings to `/src/rendering`).
2.  **Optimize Hot-Loop Vectors**:
    *   Refactor get methods (`Block::getBricks()`, etc.) to return const references or spans rather than allocating vectors by value.
3.  **Integrate Logging Macros**:
    *   Introduce `LOG_INFO`, `LOG_WARN`, and `LOG_ERROR` logging macros and replace raw console print streams.
