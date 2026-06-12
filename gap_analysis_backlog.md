# Architecture Gap Analysis & Prioritized Backlog

This document translates the findings of the **Architecture Compliance Audit Report** into a structured Gap Analysis and a prioritized backlog. Each gap is evaluated for its root cause, architectural impact, implementation effort, dependency list, and risk level, categorized across the project's primary engineering domains.

---

# PART 1: Architectural Gap Analysis

## Category: Foundation

### Gap 1.1: C++ Standard Misalignment (C++17 vs C++20)
*   **Description**: The CMake build target is configured to compile under the C++17 standard, whereas the project specifications mandate C++20.
*   **Root Cause**: Legacy configuration inherited from prototype build scripts.
*   **Architectural Impact**: Prevents implementation of modern, safe abstractions (such as `std::span` for contiguous memory views, concepts for compile-time template constraints, and range-based collections).
*   **Implementation Effort**: **Low** (Trivial change to `CMakeLists.txt`).
*   **Dependencies**: None.
*   **Risk Level**: **Low**.

### Gap 1.2: Header File Extension Violations (.h vs .hpp)
*   **Description**: All C++ header files in the project use the `.h` extension, violating the coding standard requiring `.hpp` for C++ interfaces.
*   **Root Cause**: Standard conventions were not enforced during early development.
*   **Architectural Impact**: Weakens structural code layout and creates inconsistency. Automated code scanning tools cannot easily differentiate C-family headers from C++ templates.
*   **Implementation Effort**: **Medium** (Requires renaming all header files and modifying `#include` references throughout the codebase).
*   **Dependencies**: Complete codebase files.
*   **Risk Level**: **Low** (Repetitive but mechanically straightforward; minimal logical risk).

### Gap 1.3: Non-Compliant Directory and Namespace Structure
*   **Description**: C++ source files are grouped into `/src/entities`, `/src/ui`, `/src/core`, and `/src/managers` instead of the mandated `/src/gameplay`, `/src/rendering`, `/src/persistence`, `/src/audio`, and `/src/ui`.
*   **Root Cause**: Initial physical folder tree layout was preserved without refactoring during prototype scaling.
*   **Architectural Impact**: Disorganizes file routing and violates namespace boundaries. Encourages coupling due to fuzzy package boundaries.
*   **Implementation Effort**: **Medium** (Requires moving directories, updating header include paths, and modifying CMake build file targets).
*   **Dependencies**: [CMakeLists.txt](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/CMakeLists.txt), all headers and sources.
*   **Risk Level**: **Medium** (Can cause temporary merge conflicts for active branches with local uncommitted changes).

---

## Category: Core Systems

### Gap 2.1: Coupled Global Singletons & Hardcoded Assets (ADR-0003)
*   **Description**: `FontManager` and `AudioManager` are static singleton managers. File paths to assets are hardcoded inline in C++ files. There is no central resource manifest JSON.
*   **Root Cause**: Singleton patterns were used in the prototype to quickly handle asset loading.
*   **Architectural Impact**: Heavy coupling to the disk filesystem. Bypasses constructor dependency injection, preventing independent unit testing and concurrent memory resource management.
*   **Implementation Effort**: **High** (Requires implementing a templated `ResourceManager<T>` class, creating `assets_manifest.json`, and updating `Game` and state initialization lists).
*   **Dependencies**: [Game](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/Game.h), derived GameStates, and views.
*   **Risk Level**: **High** (Changes class construction interfaces globally across the application).

### Gap 2.2: Complete Lack of Event Dispatch System (ADR-0004)
*   **Description**: The synchronous template-based `EventDispatcher` messaging bus is entirely missing. Subsystems invoke each other directly.
*   **Root Cause**: Deferred implementation of the message broker, relying instead on direct method links.
*   **Architectural Impact**: Strict coupling between gameplay models and side-effects (e.g., `Cannon` directly invokes `AudioManager` to play sound effects on firing). Breaks both Single Responsibility and Open-Closed principles.
*   **Implementation Effort**: **High** (Requires writing the template `EventDispatcher` system, defining POD event structures, and decoupling class references).
*   **Dependencies**: [Cannon](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Cannon.h), [Brick](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Brick.h), [PlayingState](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/states/PlayingState.h), and scoring/audio subsystems.
*   **Risk Level**: **High** (Requires rewriting interaction paths between gameplay entities and external systems).

### Gap 2.3: Coupled State Stack Implementation (ADR-0001)
*   **Description**: The routing stack is handled inline within the `Game` container via vector operations rather than using a dedicated, testable `StateStack` class.
*   **Root Cause**: Quick implementation of routing logic in the main application coordinator.
*   **Architectural Impact**: Couples screen overlay behaviors directly to the window loop, preventing isolated tests of state routing transitions.
*   **Implementation Effort**: **Medium** (Extract stack management logic into a decoupled `StateStack` coordinator).
*   **Dependencies**: [Game](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/Game.h), [GameState](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/GameState.h).
*   **Risk Level**: **Low**.

---

## Category: Gameplay Systems

### Gap 3.1: Hot-Path Heap Allocations (Container Copies)
*   **Description**: The gameplay update loop copies and returns dynamic vectors of pointers by value (e.g. `BlockManager::getActiveBlocks()`, `ProjectilePool::getActiveProjectiles()`, `Block::getBricks()`) every frame.
*   **Root Cause**: Container elements were returned by value for simplicity without profiling memory allocation costs.
*   **Architectural Impact**: Causes heavy heap allocation spikes and memory fragmentation during gameplay ticks. Increases garbage collection overhead and degrades L1/L2 cache line hits.
*   **Implementation Effort**: **Medium** (Refactor methods to return const references or C++20 `std::span` views).
*   **Dependencies**: [PlayingState](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/src/core/states/PlayingState.cpp), [BlockManager](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/src/managers/BlockManager.cpp), [ProjectilePool](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Projectile.h).
*   **Risk Level**: **Low** (Safe optimization that compiler checks will enforce).

### Gap 3.2: Logging Pollution (Direct stdout/stderr prints)
*   **Description**: Diagnostic logging utilizes raw `std::cout` and `std::cerr` print statements directly in the C++ logic.
*   **Root Cause**: Bypassed logging macros for quick debugging.
*   **Architectural Impact**: Bypasses log level configurations (INFO, WARN, ERROR) and prevents routing messages to disk output files in production release targets.
*   **Implementation Effort**: **Low** (Introduce logging macros and search-and-replace console streams).
*   **Dependencies**: None.
*   **Risk Level**: **Low**.

---

## Category: Persistence

### Gap 4.1: Direct and Coupled Non-Atomic Persistence (ADR-0005)
*   **Description**: `PlayingState.cpp` reads and writes high score records directly to a file (`highscore.txt`) in the local execution directory using standard streams, with no atomic rename guarantees.
*   **Root Cause**: Legacy save script that bypassed persistence design layers.
*   **Architectural Impact**: Hard couples gameplay states to disk I/O. Creates data corruption vulnerabilities if the game crashes during a save tick, and prevents filesystem mocking.
*   **Implementation Effort**: **High** (Define `IPersistenceProvider` interface, create concrete disk provider with temp-rename streams, implement `SaveManager`).
*   **Dependencies**: [PlayingState](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/states/PlayingState.h).
*   **Risk Level**: **Medium** (Essential for data integrity, requiring careful testing of atomic directory operations).

---

## Category: Rendering

### Gap 5.1: Direct Visual Coupling in Entities (ADR-0002)
*   **Description**: `Cannon`, `Brick`, and `Projectile` directly own SFML render drawables (`sf::RectangleShape`, `sf::CircleShape`, etc.), manage visual animation states, and invoke window draw commands.
*   **Root Cause**: Standard prototype structure where rendering and math are coupled in the same entity classes.
*   **Architectural Impact**: Completely blocks headless testing. Instantiating a gameplay entity is impossible without linking graphics backends, violating the core Separation of Concerns (SoC) invariant.
*   **Implementation Effort**: **High** (Strip models of all SFML dependencies and create view wrappers like `CannonView` under the rendering layer).
*   **Dependencies**: [Cannon](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Cannon.h), [Brick](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Brick.h), [Projectile](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/entities/Projectile.h), [PlayingState](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/include/core/states/PlayingState.h).
*   **Risk Level**: **High** (Fundamental refactoring of the game core simulation loop).

---

## Category: Testing

### Gap 6.1: Zero Test Cases and Inactive Test Target (testing_strategy.md)
*   **Description**: The `tests/` directory contains no test files, and the `tests/CMakeLists.txt` is an empty template. CTest has zero assertions.
*   **Root Cause**: Test implementation was entirely deferred.
*   **Architectural Impact**: Bypasses Catch2 validation gates, code coverage checks, and sanitizers, leaving the codebase without automated checks for gameplay math or persistence bugs.
*   **Implementation Effort**: **High** (Set up Catch2 v3 test runner, write unit test scripts, hand-write stubs and fakes).
*   **Dependencies**: Core libraries (`GameplayCore`, `PersistenceCore`).
*   **Risk Level**: **Medium** (Crucial quality gate; requires writing substantial new test code).

---

## Category: CI/CD

### Gap 7.1: Missing Automated Quality Gates (ci_cd_pipeline.md)
*   **Description**: The CI pipeline runs `ctest` on zero test assertions and does not enforce code coverage gates or coverage diff gates (due to Gap 6.1).
*   **Root Cause**: Dependent on the missing testing suite.
*   **Architectural Impact**: Bypasses PR checks, allowing regressions and unverified code changes to merge into the main branch unchecked.
*   **Implementation Effort**: **Low** (Update `.github/workflows/ci.yml` once test targets compile).
*   **Dependencies**: Gap 6.1.
*   **Risk Level**: **Low** (Simple workflow adjustments).

---

## Category: Documentation

### Gap 8.1: Incomplete Doxygen Annotations (coding_standards.md)
*   **Description**: Public APIs and interface parameters are missing Doxygen summaries, `@param` logs, and `@return` documentation.
*   **Root Cause**: Loose coding discipline during initial prototype development.
*   **Architectural Impact**: Weakens API documentation depth, increasing developer onboarding friction and leading to documentation drift.
*   **Implementation Effort**: **Low** (Complete header document blocks).
*   **Dependencies**: None.
*   **Risk Level**: **Low**.

---

# PART 2: Prioritized Backlog

This backlog lists the required refactoring tasks grouped into prioritized execution phases.

## Priority 1: Foundation & Test Framework (Immediate Action)

### Ticket T1.1: Upgrade to C++20 Standard
*   **Category**: Foundation
*   **Goal**: Set standard target flags in CMake.
*   **Tasks**:
    *   Change `CMAKE_CXX_STANDARD 17` to `20` in [CMakeLists.txt](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/CMakeLists.txt).
    *   Verify compile target configurations across macOS, Linux, and Windows.

### Ticket T1.2: Initialize Catch2 v3 Testing Target
*   **Category**: Testing
*   **Goal**: Implement the `tests_run` runner and configure automated CTest discovery.
*   **Tasks**:
    *   Set up [tests/CMakeLists.txt](file:///Users/sergeychukhno/Desktop/C:C++/BrickBreaker/tests/CMakeLists.txt) to fetch and link Catch2 v3.
    *   Create `tests/main.cpp` configured with `Catch2WithMain`.
    *   Write the first unit test check (e.g. basic math operations or game settings).
    *   Add the test execution step to `.github/workflows/ci.yml`.

### Ticket T1.3: Apply Header Extensions and Directory Restructuring
*   **Category**: Foundation
*   **Goal**: Rename headers to `.hpp` and restructure file directories on disk.
*   **Tasks**:
    *   Rename all `.h` header files inside `include/` to `.hpp`.
    *   Update all `#include` lines to reference `.hpp` paths.
    *   Move files to match documented folders: `/src/gameplay/`, `/src/rendering/`, `/src/persistence/`, `/include/gameplay/`, etc.
    *   Update CMake target file listings.

---

## Priority 2: Decoupling & SoC (Middle Action)

### Ticket T2.1: Implement Model-View Separation (Entity Decoupling)
*   **Category**: Rendering / Gameplay Systems
*   **Goal**: Remove graphics dependencies from gameplay models and create view classes.
*   **Tasks**:
    *   Remove SFML headers, shapes, text, color states, and draw calls from `Cannon`, `Brick`, and `Projectile`.
    *   Ensure gameplay simulation classes contain only math, coordinates, speeds, and bounds variables.
    *   Create views (`CannonView`, `BrickView`, `ProjectileView`) to manage texture bindings and submit window draw calls.
    *   Integrate model queries via const references.
    *   Write unit tests for isolated, graphics-free gameplay simulation checks under `tests/gameplay/`.

### Ticket T2.2: Implement Resource Manager Cache & Manifest Schema
*   **Category**: Core Systems
*   **Goal**: Replace static singletons with constructor-injected resource manager caches.
*   **Tasks**:
    *   Implement templated `ResourceManager<T>` class.
    *   Create the `assets_manifest.json` catalog file under `assets/`.
    *   Refactor `FontManager` and `AudioManager` to load paths dynamically from the manifest via the resource manager.
    *   Inject references to managers into the constructors of all views and states.
    *   Eliminate hardcoded path literals inside C++ files.

### Ticket T2.3: Implement Decoupled Save System
*   **Category**: Persistence
*   **Goal**: Decouple file persistence from gameplay states.
*   **Tasks**:
    *   Define `IPersistenceProvider` interface.
    *   Implement concrete `DiskPersistenceProvider` with temp-rename write streams.
    *   Implement `SaveManager` to encapsulate player progress mapping.
    *   Refactor `PlayingState` to save scores through the constructor-injected `SaveManager` rather than raw fstreams.
    *   Write hand-written mock stubs (`FakePersistenceProvider`) and persistence unit tests under `tests/persistence/`.

### Ticket T2.4: Integrate the Event Dispatcher Messaging Bus
*   **Category**: Core Systems
*   **Goal**: Decouple systems using synchronous event-driven communication.
*   **Tasks**:
    *   Implement synchronous `EventDispatcher` templates.
    *   Define POD event payload structs (`BrickDestroyedEvent`, `ProjectileFiredEvent`, `PlaySoundEvent`).
    *   Refactor `Cannon` to emit a `PlaySoundEvent` when firing, rather than calling the audio manager directly.
    *   Inject `EventDispatcher&` into the constructors of auxiliary systems, and register event subscriptions internally.

---

## Priority 3: Optimization & Documentation (Final Action)

### Ticket T3.1: Resolve Hot-Path Dynamic Memory Allocations
*   **Category**: Gameplay Systems
*   **Goal**: Eliminate heap allocations inside update/render frames.
*   **Tasks**:
    *   Modify methods (`Block::getBricks()`, `BlockManager::getActiveBlocks()`, etc.) to return const references or C++20 `std::span` containers.
    *   Pre-allocate vector capacities (`reserve()`) inside update loops, or utilize static arrays.
    *   Write performance allocation benchmark assertions using custom tracking allocators under `tests/performance/`.

### Ticket T3.2: Implement Decoupled State Stack Coordinator
*   **Category**: Core Systems
*   **Goal**: Separate state stack management from the Game class.
*   **Tasks**:
    *   Create the `StateStack` coordinator class.
    *   Update `Game` to delegate frame updates and draws to the instanced `StateStack`.
    *   Implement deferred stack state pop/push command buffers to ensure stack safety.
    *   Write state transition tests under `tests/integration/`.

### Ticket T3.3: Finalize Logging and API Documentation
*   **Category**: Documentation / Gameplay Systems
*   **Goal**: Complete Doxygen comments and route prints through logging macros.
*   **Tasks**:
    *   Add missing Doxygen tags (`@param`, `@return`) to all public APIs.
    *   Define `LOG_INFO`, `LOG_WARN`, and `LOG_ERROR` logging macros.
    *   Replace direct console stdout/stderr stream logs with the new logging macros.
