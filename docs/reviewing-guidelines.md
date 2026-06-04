# Technical Reviewing Guidelines (reviewing-guidelines.md)

This document is the official blueprint for code reviews, providing specific checklists and heuristics for automated AI reviewers (CodeRabbit) and senior human engineers.

---

## 1. Architectural Vigilance & Separation of Concerns (SoC)

Reviewers must guarantee that the mathematical simulation model (gameplay logic) remains completely independent of the rendering and audio execution layers.

### Checklist
- [ ] **No Graphics in Simulation**: Do files under `include/gameplay/` or `src/gameplay/` inherit from `sf::Drawable` or store `sf::RenderWindow` / `sf::Texture` references? (Exception: View/Renderer classes specifically designed for drawing).
- [ ] **State Independence**: Verify that update calculations in entities (`Cannon`, `Projectile`, `Brick`) do not read from UI layout sizes or viewport window transformations directly.
- [ ] **Decoupled Audio Calls**: Check that audio triggering is managed via event dispatchers or returned commands, rather than gameplay entities invoking sound triggers directly.

---

## 2. Architectural Layer Validation

Ensure components do not violate the strict top-down dependency flow:
```
  UI Layer
     │
     ▼
  State Layer
     │
     ▼
  Gameplay Layer
     ▲
     │ (snapshot serialization only)
  Persistence Layer
```

### Checklist
- [ ] **Gameplay Layer Isolation**: Gameplay classes do not include UI headers (`ui/`) or perform drawing commands.
- [ ] **Rendering Layer Isolation**: Gameplay classes do not include rendering headers (`rendering/`) or store graphics handles.
- [ ] **No Inline File I/O**: Gameplay classes do not perform direct file I/O (saving, loading, configuration reading). These must be delegated to `SaveManager` / `PersistenceService`.
- [ ] **Read-Only Simulation Queries**: Rendering and View classes (e.g. `CannonView`) depend only on gameplay simulation data for visual state, querying properties strictly by read-only const observers.
- [ ] **Non-Mutating Persistence**: Persistence/serialization classes do not mutate active gameplay states; they load or store raw state snapshots.
- [ ] **State Control Isolation**: State classes (such as `PlayingState`) orchestrate subsystems (rendering, physics ticks, input) but do not contain detailed entity physics, movement, or collision calculations directly.

---

## 3. Ownership Analysis & RAII Invariants

Memory must be managed deterministically. Shared ownership is a potential leak vector and must be avoided.

### Checklist
- [ ] **Single Ownership Rule**: Enforce `std::unique_ptr` for all major subsystem lifecycles and objects.
- [ ] **Restricted Shared Pointer**: Reject `std::shared_ptr` usage unless there is proof that the resource lifetime is genuinely shared and cannot be represented hierarchically.
- [ ] **Observer Pointer Safety**:
  - Raw pointers (`T*`) must only be used as non-owning observers.
  - Check if raw observer pointers are safely cleared or invalidated when the subject is deleted.
  - Review maps (e.g. `projectileHitBricks_`) that store raw pointers to ensure cleanup loops run *before* any entity deallocation.

---

## 4. Const Correctness & API Design

Verify that functions, parameters, and return types are const-qualified and optimized to protect state invariants and enable compiler optimizations.

### Checklist
- [ ] **Const Member Functions**: Non-mutating member functions must be marked `const` (e.g., `int getDamage() const noexcept`).
- [ ] **Const Reference Parameters**: Large read-only parameters (such as `std::string`, `std::vector`, or custom class types) are passed as `const T&` to avoid copy allocations.
- [ ] **Const Container Returns**: Internal containers exposed publicly by reference must be returned as `const T&` to prevent caller modification of private members.
- [ ] **Noexcept Usage**: Frequently called utility functions, move constructors, move assignment operators, and non-throwing getters should be marked `noexcept` whenever possible. This enables move optimizations (e.g. for `std::vector` relocations) and guarantees non-throwing API contracts.

---

## 5. SOLID Violations & Singletons

Ensure our object-oriented architecture conforms to SOLID principles and avoids global mutable states.

### Checklist
- [ ] **Single Responsibility Principle (SRP)**: Reject classes that handle more than one domain. For example, a `Brick` class must not handle damage mechanics *and* text fonts *and* drawing passes.
- [ ] **Open-Closed Principle (OCP)**: Check if new gameplay behaviors (e.g. new brick shapes or power-ups) require modifying existing switch cases in core code, or if they are decoupled using interfaces and registries.
- [ ] **Interface Segregation**: Verify that interface structures remain narrow and specialized rather than monolithic.
- [ ] **Singleton Detection**: Reject newly introduced Singleton patterns. All subsystems must receive their dependencies through constructor injection or references owned by the core application instance (`Game`).

---

## 6. Allocation & Hot Path Performance

No dynamic heap allocations or costly container operations are allowed inside performance-critical sections (the hot path).

### Hot Path Definition
Any function invoked inside the main loops: `Game::run()`, `PlayingState::update()`, `PlayingState::render()`, or entity `update()` / `draw()` ticks.

### Checklist
- [ ] **No Dynamic Allocations**: Scan the hot path for explicit allocations: `new`, `std::make_unique`, or `std::make_shared`.
- [ ] **Hidden Allocation Detection**:
  - **Vector growth inside update/render loops**: Watch for container insertions like `std::vector::push_back` or `std::vector::emplace_back` without pre-allocated capacity (`reserve()`). Recommend static pools (`ProjectilePool`).
  - **Hash map insertions inside update/render loops**: Watch for `std::unordered_map` insertions or element lookups via `operator[]` that trigger bucketing and rehashing.
  - **String construction inside render loops**: Watch for string allocations, concatenations, or `std::stringstream` usage on hot paths (pre-format or cache text assets).
  - **Temporary large container creation inside hot paths**: Avoid allocation spikes from creating local maps, vectors, or arrays inside updates.
  - **Repeated asset lookups causing allocations**: Lookups from resource managers should resolve by reference or ID index, avoiding nested map searches inside loops.
- [ ] **Virtual Dispatch Audit**: Review excessive virtual function calls inside tight update/render loops. Prefer static polymorphism (templates, CRTP), component composition, or pre-resolved dispatch for frequently executed systems (such as particles, projectiles, and collision systems) where thousands of calls occur per frame.

---

## 7. Concurrency & Thread Ownership

Review the multi-threading borders to ensure thread safety without mutating gameplay simulation variables asynchronously.

### Checklist
- [ ] **Main-Thread Gameplay**: Verify that `GameplayWorld` execution (simulation, updates, physics checks) occurs strictly on the main application thread.
- [ ] **Single-Threaded Physics**: Ensure collision detection math and entity updates never execute on background worker threads.
- [ ] **Worker Thread Constraints**: Ensure worker threads are restricted exclusively to:
  - Asset loading (pre-fetching resources to avoid frame hitches)
  - Save serialization (writing profile saves atomically to disk)
  - Telemetry (dispatching diagnostic metrics asynchronously)
- [ ] **Message Queue Preference**: Cross-thread communication should prefer command queues, message queues, futures, or immutable snapshots over shared mutex-protected gameplay data.

---

## 8. SFML Compliance & Resource Caching

Ensure SFML integration respects third-party boundaries, resource ownership, and performance targets.

### Checklist
- [ ] **Unified Resource Managers**: Reject direct calls to `sf::Texture::loadFromFile` or `sf::Font::loadFromFile` inside gameplay/view classes. All textures and fonts must be loaded and resolved through `ResourceManager`.
- [ ] **Unified Audio Managers**: Reject direct ownership or creation of `sf::Sound` or `sf::Music` classes. Audio playback and life cycles must be managed by `AudioManager`.
- [ ] **No Draw Calls in Gameplay**: Gameplay simulation models (e.g. `Cannon`, `Brick`) contain no SFML graphics dependencies (`sf::Sprite`, `sf::RectangleShape`) and perform no draw call submissions.
- [ ] **Transition Loading**: Ensure asset loading does not occur inside hot update/render loops. All resources must be pre-loaded during state transitions (loading screens) or cached on background threads.
- [ ] **Resource Lifetime Safety**: Verify that textures, fonts, sound buffers, and shaders outlive all views and entity components referencing them. Views and UI elements must never store references to resources that can be unloaded during their lifetime.

---

## 9. Naming Consistency & Style

Strictly enforce naming conventions to ensure structural readability across files, matching [Coding Standards](coding_standards.md#part-b-naming-conventions):

| Element | Format | Example |
| :--- | :--- | :--- |
| **Classes / Structs / Enums** | PascalCase | `ProjectilePool` |
| **Methods / Functions** | camelCase | `checkAABBCollision` |
| **Private Member Variables** | camelCase_ with trailing underscore | `stateStack_` |
| **Local Variables** | camelCase | `deltaTime` |
| **Constants** | kPascalCase with leading 'k' | `kProjectileSpeed` |
| **Namespaces** | lowercase | `gameplay` |

---

## 10. CI/CD & Build Compliance

Verify compatibility with our continuous integration quality gates and compiler targets.

### Checklist
- [ ] **CMake Target Inclusions**: Ensure new source files (`.cpp`) and header files (`.hpp`) are added to the corresponding CMake target declarations in `CMakeLists.txt`.
- [ ] **Automated Test Updates**: Check that gameplay logic modifications or persistence changes include accompanying unit test cases under the `tests/` directory.
- [ ] **Coverage Diff Gate**: New or modified gameplay and persistence code must include tests covering at least 80% of newly introduced lines (ensuring diff coverage remains high rather than just checking total coverage).
- [ ] **Zero Warnings Compliance**: Verify that compiler warning gates are clean (warnings treated as errors).
- [ ] **Static Analysis Audits**: Confirm that new code passes `clang-tidy` rules and does not introduce static violations.
- [ ] **Sanitizer Cleanliness**: Ensure local executions under AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) targets are free of memory leaks and memory access faults.

---

## 11. Documentation & API Design

Ensure documentation matches code changes to prevent drift.

### Checklist
- [ ] **Doxygen Headers**: Public class declarations in header files contain Doxygen block summaries detailing their architectural purpose.
- [ ] **Parameter Documenting**: All parameters (`@param`) are documented for public APIs.
- [ ] **Exception Documentation**: Any public API capable of throwing must document its failure conditions using `@throws` (functions that cannot throw must not include this tag).
- [ ] **Return Conditions**: Functions with non-void return values have `@return` conditions documented.
- [ ] **Architecture Manual Updates**: Ensure modifications to managers, message paths, or persistence structures are updated in the `/docs` manual directories.
- [ ] **Architecture Decision Records (ADR)**: Significant design choices or deviations from established conventions must include a corresponding Architecture Decision Record inside `docs/adr/`.
- [ ] **Developer Log Parity**: Every pull request must append a description of changes to `DEV-LOG.md`.

---

## 12. Review Severity Classification

CodeRabbit and human reviewers must classify pull request findings into the following tiers to ensure review efficiency and consistency:

### Critical (Blocks Merge)
Must be resolved before merging.
* **Examples**:
  * Memory safety or resource leaks (e.g. RAII violations, unowned raw pointers).
  * Threading race conditions, shared mutable state, or concurrency violations.
  * Architectural layer boundary crossings (e.g., Gameplay including UI/Rendering headers).
  * Critical gameplay logic bugs or physics simulation breakdowns.
  * Save data corruption risks or non-atomic write procedures.

### Major (Blocks Merge unless explicitly justified)
Should be addressed unless there is a strong, documented exception.
* **Examples**:
  * Code coverage regressions or failing the 80% Coverage Diff Gate.
  * Substantial performance degradations inside hot paths (e.g., heap allocations in loops, excessive virtual dispatches).
  * Missing unit tests for newly introduced systems or components.
  * Public API design violations (e.g., non-const query methods, copies of large parameters, missing `noexcept`).
  * Missing Doxygen documentation for newly added public APIs.

### Minor (Non-blocking, suggested improvements)
Optional suggestions to improve cleanliness; does not prevent integration.
* **Examples**:
  * Minor naming style deviations or inconsistent capitalization.
  * Code formatting issues (easily resolved via `clang-format`).
  * Small readability changes or redundant code simplifications.
  * Comment adjustments, typos, or minor Doxygen formatting fixes.
