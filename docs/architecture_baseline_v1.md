# Architecture Baseline v1.0
## 0. Baseline Metadata

Version: 1.0
Status: Frozen
Freeze Date: YYYY-MM-DD
Superseded By: N/A

## 1. Purpose

This document establishes the frozen Architecture Baseline (v1.0) for the Cyberpunk Cannon Shooter project. It serves as the authoritative governance reference for developers, code reviewers, continuous integration systems, and AI engineering assistants. All active implementation, refactoring, and code validation processes must conform to this baseline. It ensures that the engineering principles and design patterns agreed upon in the Architecture Decision Records (ADRs) are strictly enforced and historically traceable.

---

## 2. Accepted Architecture

The Cyberpunk Cannon Shooter engine is built on a highly modular, decoupled architecture structured as follows:

*   **StateStack Architecture**:
    High-level routing and UI screen transitions are managed by a centralized, standalone `StateStack` coordinator. Individual states (e.g., `MainMenuState`, `PlayingState`, `PausedState`) are isolated from each other. They request screen changes (push, pop, change) via a deferred command buffer that is processed safely at the end of the frame tick to prevent stack iteration corruption. This stack allows transparent drawing overlays for paused gameplay and translucent settings screens.
*   **Gameplay / Rendering Separation (Model-View Separation)**:
    The core gameplay logic operates inside a dedicated static library (`GameplayCore`). Gameplay simulation classes (e.g., `Cannon`, `Brick`, `Projectile`, `GameplayWorld`) contain only mathematical models, coordinates, velocities, and bounds calculations. They have no references to SFML graphics headers (`sf::Sprite`, `sf::Shape`, `sf::RenderWindow`) or drawing functions. Separated View classes (e.g., `CannonView`, `BrickView`) observe the models using read-only `const` references and submit draw commands to the rendering pipeline, enabling headless unit testing.
*   **Event System (Decoupled Message Bus)**:
    Subsystems communicate through a synchronous event dispatcher(`EventDispatcher`). Gameplay simulation events (such as a projectile firing or a brick breaking) are emitted as Plain Old Data (POD) structs. Auxiliary systems (such as `ScoreSystem` or `AudioSystem`) subscribe callback methods to these event types, eliminating class-level coupling and keeping side-effects isolated from the simulation loop.
*   **Persistence Architecture**:
    The persistence pipeline is isolated from gameplay states using interface-driven boundaries. High-level game configurations and progression maps are managed by `SaveManager` which directs serialization through `PersistenceService`. Disk writes are executed by an abstract provider (`IPersistenceProvider`) using concrete implementations (like `DiskPersistenceProvider` with temp-write, verify, and atomic rename pipelines) to guarantee transaction integrity and enable filesystem-free unit testing via in-memory stubs.
*   **Resource Management Architecture**:
    The codebase does not contain global static resource managers or singletons. All assets (textures, fonts, shaders, sound buffers) are loaded and cached by instanced managers (e.g., `ResourceManager<T>`) owned by the top-level application context (`Game`) and injected by reference into dependent states and views. Asset file paths are resolved dynamically using string metadata IDs mapped inside `assets_manifest.json`, eliminating path hardcoding in source files.
*   **Testing Architecture**:
    The engine relies on a standalone automated Catch2 v3 test suite compiled into a dedicated binary (`tests_run`) separate from the main application. It tests:
    *   Headless physics and math collision invariants.
    *   In-memory persistence schemas and mock serialization flows using hand-written stubs.
    *   Replay logs and event-driven integration scenarios.
    *   Performance allocations using tracking wrappers.

---

## 3. Authoritative Documents

The following documents constitute the official project specification suite and must be treated as authoritative references:

| Document | Purpose | Ownership | Review Responsibility |
| :--- | :--- | :--- | :--- |
| [architecture_overview.md](architecture_overview.md) | Central portal mapping software architecture layers and navigation indices. | Lead Architect | Architecture Governance Lead |
| [coding_standards.md](coding_standards.md) | Enforced programming conventions, parameter design, naming styles, and logging rules. | Senior C++ Developer | Peer Reviewers / Linter Checks |
| [reviewing-guidelines.md](reviewing-guidelines.md) | Specific checklists and validation guidelines for human and AI reviewers. | Senior QA Engineer | CodeRabbit / Peer Reviewers |
| [testing_strategy.md](testing_strategy.md) | Unit, integration, performance, property-based, and Golden Master testing strategy. | QA Lead | QA Lead / CI CTest Runner |
| [repository_structure.md](repository_structure.md) | Repository subfolder taxonomy, package locations, and file extensions. | DevOps Engineer | Governance Lead / Peer Reviewers |
| [build_deployment.md](build_deployment.md) | Compiler flags, C++ standard targets, build types, and packaging bundles. | Release Engineer | DevOps Lead / CI Compilers |
| [ci_cd_pipeline.md](ci_cd_pipeline.md) | CI quality gates, compiler checks, coverage gates, and branch protections. | DevOps Lead | DevOps Lead / GitHub Actions |
| [assets_manifest_spec.md](assets_manifest_spec.md) | JSON registry schemas, key taxonomies, and supported resource formats. | Audio/Art Director | Asset Pipeline Validator Script |

---

## 4. Accepted ADR Registry

The following table lists the active and frozen Architecture Decision Records:

| ADR | Title | Status | Scope |
| :---: | :--- | :---: | :--- |
| **ADR-0001** | [Screen Routing via State Stack](adr/ADR-0001-state-stack.md) | **ACCEPTED** | Decouples high-level screen states and provides overlapping pause structures. |
| **ADR-0002** | [Entity-View Separation](adr/ADR-0002-entity-view-separation.md) | **ACCEPTED** | Isolates game physics and math from SFML drawing libraries for headless testing. |
| **ADR-0003** | [Dependency-Injected Resource Manager](adr/ADR-0003-resource-manager.md) | **ACCEPTED** | Bans singletons, injecting instanced cache managers using manifest string IDs. |
| **ADR-0004** | [Synchronous Event Dispatcher](adr/ADR-0004-event-system.md) | **ACCEPTED** | Decouples simulation triggers from side-effects via synchronous template broadcasting. |
| **ADR-0005** | [Decoupled Persistence Architecture](adr/ADR-0005-persistence-architecture.md) | **ACCEPTED** | Separates filesystem operations from game states, ensuring atomic writes. |

---

## 5. Architectural Invariants

Reviewers must reject any pull request that violates the following non-negotiable architectural invariants:

1.  **Gameplay Code is Rendering-Free**:
    Gameplay classes (located in `/src/gameplay/` and `/include/gameplay/`) must not include rendering library headers (`<SFML/Graphics.hpp>`, etc.) or store graphics structures (`sf::Sprite`, `sf::Shape`, `sf::RenderWindow`, `sf::Shader`). 
2.  **No Singleton Resource Managers**:
    Global singleton managers (such as classes implementing static `getInstance()` resource accessors or static caching vectors) are strictly prohibited. Resources must be constructor-injected.
3.  **State Transitions are Deferred**:
    Derived state classes must never mutate the stack active list directly. All state push, pop, or change operations must be queued in a deferred buffer to execute at the end of the frame tick.
4.  **Persistence Decoupled via Interface**:
    Game states and gameplay logic must never invoke standard file streams (`std::ofstream`, etc.) or read from the disk directly. All filesystem operations must pass through the `IPersistenceProvider` interface.
5.  **Event Propagation uses EventDispatcher**:
    Gameplay entities must not invoke other systems directly when generating triggers (e.g. playing sounds). They must broadcast payload structs via `EventDispatcher::emit<T>()`.
6.  **Headless-Testable Simulation**:
    The mathematical simulation model must be fully compilable and testable without raising an OpenGL window context or linking graphics resources.
7.  **No Hardcoded Disk Paths**:
    Path string literals (e.g., `"assets/textures/cannon.png"`) are prohibited in C++ logic. Resources must be requested strictly using string identifiers defined in `assets_manifest.json`.

---

## 6. Change Management Policy

To prevent architectural drift and maintain technical consistency, the following changes policy is enforced:

*   **Frozen ADRs**: Accepted ADRs are considered historically frozen.
*   **Architectural Changes require a New ADR**: Any major structural update or design deviation from the baseline must be documented in a new ADR under `docs/adr/`.
*   **ADR Modifications require a Superseding Record**: Existing ADRs must not be edited in place. Any changes to their decisions must be proposed in a new ADR that explicitly supersedes the older document.
*   **Roadmap Alignment**: Backlog tickets and roadmap phases must not propose implementations that violate accepted ADRs.
*   **Code Review Invalidation**: Reviewers must reject implementations that conflict with accepted ADRs, regardless of convenience or local performance excuses.

---

## 7. Architecture Compliance Rules

All peer reviews, continuous integration builds, and AI engineering assistants must verify compliance with the following quality gates:

*   **Coding Standards Compliance**:
    *   Verify all headers use the `.hpp` extension and implementation files use `.cpp`.
    *   Verify casing rules (PascalCase for classes/structs/enums, camelCase for methods, trailing underscores `_` for private members).
    *   Ensure zero heap allocations occur inside active game update loops (using object pools or contiguous vectors).
    *   Verify no raw console streams (`std::cout`, `std::cerr`) exist in production files (all logs must pass through macros).
*   **Architectural Boundary Compliance**:
    *   Ensure dependencies flow strictly top-down (UI $\rightarrow$ State $\rightarrow$ Gameplay $\rightarrow$ Persistence).
    *   Verify no circular imports exist between namespaces.
*   **Testing Compliance**:
    *   Ensure all new math, collision, and serializing implementations include corresponding unit checks in the test suite.
    *   Verify mock fakes are used to isolate file saving and resource caches during unit tests.
*   **Coverage Compliance**:
    *   Verify new code satisfies coverage gate minimums (Persistence $\ge 80\%$, Gameplay $\ge 70\%$).
    *   Enforce the **80% Coverage Diff Gate** blocking PRs with untested modifications.
*   **ADR Compliance**:
    *   Verify that class dependency injection and synchronous dispatcher mappings align with ADR-0001 through ADR-0005.

---

## 8. Current Architecture Status

As of v1.0, the architectural definition is complete and locked:

*   **Documentation Complete**: All core documentation guides, coding standards, and testing strategies are written and saved in the repository.
*   **Architecture Reviewed**: The decoupled layout model has been successfully validated.
*   **Compliance Audit Completed**: All deviations in the current prototype (lack of views, static singletons, inline saving, and missing tests) have been cataloged.
*   **Gap Analysis Completed**: Root causes, dependencies, and risk levels for all issues are documented.
*   **Roadmap Established**: A dependency-driven execution roadmap (`ROADMAP_FORWARD.md`) is active to resolve all structural gaps.

---

## 9. Implementation Authorization

Implementation work is officially authorized to proceed according to the phases defined in [ROADMAP_FORWARD.md](../ROADMAP_FORWARD.md). 

Developers must focus strictly on resolving the architectural gaps and completing the gameplay features outlined in the roadmap. No new structural patterns, singletons, or coupled dependencies may be introduced. Any future modification to this baseline or its accepted design decisions must follow the official ADR proposal process.

---
*End of Architecture Baseline v1.0*
