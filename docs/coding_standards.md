# C++ Coding Standards & Style Manual

This document defines the architectural guidelines, programming conventions, memory safety rules, and validation standards for the Cyberpunk Cannon Shooter codebase. All code contributions must adhere to these policies.

---

## Part A: Core Principles

1. **RAII First (Resource Acquisition Is Initialization)**:
   All resource lifetimes (heap allocations, file handles, texture bounds, thread buffers) must be bound to C++ stack scopes. Use standard library containers (`std::vector`, `std::string`) or smart wrappers to manage allocation automatically. Manual `delete` calls are prohibited.
2. **No Raw Ownership**:
   Raw pointers (`T*`) are strictly non-owning observers. They are never allowed to call `delete` or manage lifetimes. If a class requires ownership of an object, it must express it explicitly via standard smart pointers or containers.
3. **Single Responsibility Principle (SRP)**:
   Keep subsystem components focused. A class must have only one reason to change. Decouple simulation math (models) from render passes (views) and input polling handlers.
4. **Dependency Injection over Singletons**:
   Avoid global mutable states. Singletons are prohibited. Resources, settings, and persistence managers must be owned by the top-level application context (`Game`) and passed by reference or interface pointer to dependent structures.
5. **Composition over Inheritance**:
   Build complex behaviors by composing component behaviors rather than deriving deep class hierarchies. Limit inheritance to interface implementation (pure virtual classes) or state overrides.
6. **Data-Oriented Design (DoD)**:
   For performance-critical systems (such as particles, projectiles, and collision solvers), keep memory layout contiguous using pools or flat arrays (e.g. `std::vector<T>`) to optimize L1/L2 cache line hits and prevent cache misses during iteration loops.

---

## Part B: Naming Conventions & Enum Rules

### 1. Types (Classes, Structs, Type Aliases)
Types must use **PascalCase**:
```cpp
class CannonModel;
struct ProjectileState;
using CoordinateList = std::vector<sf::Vector2f>;
```

### 2. Enum Class Rule
* **Plain enums are forbidden** (e.g. `enum Color { Red, Blue }`).
* Use `enum class` exclusively to prevent namespace pollution.
* Enum type names and value keys must use **PascalCase**:
```cpp
enum class BrickType {
    Standard,
    Shield,
    GoldenCore
};
```

### 3. Methods and Functions
Methods and functions must use **camelCase**:
```cpp
void updatePhysics(float deltaTime);
void spawnWave(int waveIndex);
```

### 4. Variable Names
Local variables must use **camelCase**:
```cpp
float targetAngle = 45.0f;
int hitCount = 0;
```

### 5. Member Variables
Private and protected member variables must use **camelCase** with a trailing underscore (`_`):
```cpp
int currentLevel_;
float comboTimer_;
std::unique_ptr<CannonModel> activeCannon_;
```
*Note: Public members in plain data structures (POD/structs) do not use trailing underscores.*

### 6. Constants and Constexpr
Constants must use the prefix `k` followed by **PascalCase**:
```cpp
constexpr float kProjectileSpeed = 1200.0f;
const std::string kHighScoreFile = "highscore.txt";
```

### 7. Namespaces
Namespaces must use all **lowercase** characters:
```cpp
namespace gameplay {
    // gameplay classes
}
```

---

## Part C: File Organization

To keep components separated, the codebase organizes header files (`.hpp`) and implementation files (`.cpp`) by module:

```
/src                          # C++ implementation files (.cpp)
├── core/
│   └── states/               # Game state files (PlayingState.cpp, PausedState.cpp)
├── gameplay/                 # Entity simulation loops (Cannon.cpp, Projectile.cpp)
├── rendering/                # Drawing wrappers (CannonView.cpp, Starfield.cpp)
├── persistence/              # File I/O operations (SaveManager.cpp)
├── audio/                    # Sound player wrappers
└── ui/                       # Interface button renderers

/include                      # C++ header files (.hpp)
├── core/
│   └── states/
├── gameplay/
├── rendering/
├── persistence/
├── audio/
└── ui/
```

*Note: All files must use the `.hpp` extension for header files and `.cpp` for source files.*

---

## Part D: Header Rules & API Parameter Design

### 1. Pragma Once
Every header file must start with `#pragma once` to prevent double-inclusion issues:
```cpp
#pragma once
```

### 2. Include Order
To prevent hidden compilation dependencies, group and sort includes alphabetically within their categories:
1. **Corresponding Header**: The `.hpp` file implemented by this `.cpp` file (first include in `.cpp` files).
2. **Standard Library Headers**: Sorted alphabetically (e.g. `<memory>`, `<vector>`).
3. **Third-Party Libraries**: Sorted alphabetically (e.g. `<SFML/Graphics.hpp>`).
4. **Project Local Headers**: Sorted alphabetically using relative paths (e.g. `"core/states/PlayingState.hpp"`).

Example:
```cpp
#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "gameplay/Projectile.hpp"
```

### 3. Forward Declarations
Minimize compilation dependencies:
* Avoid including heavy headers in header files when declaring pointers or references.
* Use **forward declarations** in header files whenever possible:
```cpp
// In Cannon.hpp
class ProjectilePool; // Forward declaration

class Cannon {
public:
    void fire(ProjectilePool& pool);
};
```
* Include the full header file (`ProjectilePool.hpp`) in the implementation file (`Cannon.cpp`).

### 4. API Design & Parameter Passing
To maintain consistency and optimize register allocations, follow these standard signatures for parameter passing:
* **Small/Primitive Types** ($\le 8$ bytes: `int`, `float`, `char`, `bool`): Pass **by value** to utilize registers directly:
  ```cpp
  void setHealth(int value);
  void setRotation(float angle);
  ```
* **Large/Complex Types** ($> 8$ bytes: `std::string`, `std::vector`, large structs): Pass **by const reference** to prevent copying overhead:
  ```cpp
  void setName(const std::string& name);
  void setTargetCoordinates(const sf::Vector2f& position);
  ```
* **Ownership Transfer**: Pass by `std::unique_ptr` **by value** to explicitly force the caller to move ownership:
  ```cpp
  void setWeapon(std::unique_ptr<Weapon> weapon); // Transferred via std::move
  ```

---

## Part E: Memory, Const Correctness & Move Semantics

### 1. Ownership Smart Pointers
* **`std::unique_ptr`**: The default choice for heap-allocated object ownership. Transfer ownership explicitly using `std::move`.
* **`std::shared_ptr`**: Permitted only when an object has true shared ownership across multiple threads or subsystems (e.g., sharing a font resource across multiple UI text elements).
* **`std::weak_ptr`**: Use to break cyclic reference graphs when using `std::shared_ptr` or to observe shared resources without extending their lifetimes.

### 2. Raw Pointers (`T*`)
* Raw pointers represent **non-owning observers**.
* They must never call `delete` or manage lifetimes.
* Always check raw pointers for `nullptr` before dereferencing if the lifecycle of the observed target is variable.
* If a target lifecycle is guaranteed to outlive the observer, prefer passing dependencies by reference (`T&`).

### 3. Const Correctness Rules
* **Const Member Functions**: Every method that does not mutate the object's logical state must be marked `const`:
  ```cpp
  class Projectile {
  public:
      int getDamage() const; // Marked const
  };
  ```
* **Const Reference Returns**: When exposing internal data containers, prefer returning const references to prevent encapsulation breaks:
  ```cpp
  const std::vector<Projectile>& projectiles() const; // Read-only view
  ```

### 4. Move Semantics (Rule of Zero / Five)
* **Rule of Zero**: Prefer designing classes so that they do not declare custom destructors, copy/move constructors, or copy/move assignment operators. Use standard library components (e.g. `std::vector`, `std::unique_ptr`) to automatically manage resource lifetimes.
* **Rule of Five**: If a class manages custom low-level resources and requires a custom destructor, you must explicitly declare or delete all five special member functions (Destructor, Copy Constructor, Copy Assignment, Move Constructor, Move Assignment).
* **Move Operations**: Declare move constructors and move assignment operators as `noexcept = default` (or provide custom `noexcept` implementations) to ensure that containers like `std::vector` can perform optimized shifts during reallocations:
  ```cpp
  class TextureCache {
  public:
      // Declare explicit move semantics
      TextureCache(TextureCache&&) noexcept = default;
      TextureCache& operator=(TextureCache&&) noexcept = default;

      // Disable copying
      TextureCache(const TextureCache&) = delete;
      TextureCache& operator=(const TextureCache&) = delete;
  };
  ```

### 5. noexcept Policy
* Mark functions that cannot throw exceptions with `noexcept` (e.g., basic getters, boolean checkers, move constructors, and destructors):
  ```cpp
  int getHealth() const noexcept;
  bool isAlive() const noexcept;
  ```
* Marking non-throwing functions with `noexcept` provides compiler optimization hints and ensures proper STL container behavior (like moving instead of copying elements during array reallocations).

### 6. Run-Time Type Information (RTTI) Prohibition
* The use of `dynamic_cast` and `typeid` is prohibited unless explicitly justified. Heavy reliance on RTTI indicates poor abstraction design.
* Prefer alternative patterns:
  * Safe design options like `enum class` type tags.
  * Polymorphism and virtual functions.
  * Visitor patterns or component-based systems.
  * State pattern structures.

---

## Part F: Exception & Error Handling Policy

### 1. Exception Boundaries (Fatal & Startup)
* Exception throwing (e.g., `std::runtime_error`) is allowed during startup, initial level configuration loads, or resource parsing failures.
* These represent fatal configuration setups where terminating or aborting the state is expected because the engine cannot proceed.

### 2. Error Handling (Expected / Recoverable Failures)
* For expected gameplay logic failures (e.g., trying to read a level ID that is out of range, or looking up an asset that doesn't exist), throwing exceptions is prohibited.
* Use `std::optional<T>` (or `std::expected<T, E>` in C++23) to represent clean, expected recoverable results:
  ```cpp
  std::optional<LevelConfig> loadLevel(int levelId); // Returns std::nullopt if file missing
  ```

### 3. Loop Fault Mitigation
* Gameplay ticks (`update(float)`), physics solvers, collision checks, and render passes must **never throw** exceptions.
* Subsystems must catch and resolve logic errors locally to guarantee a zero-crash gameplay loop.

---

## Part G: Logging Standards

* Directly printing to standard output channels (`std::cout`, `std::cerr`, `printf`) is prohibited in production code.
* Logging must pass through defined logging macros:
  * `LOG_INFO("...")`: General state changes (e.g., loading level configurations, completed saving).
  * `LOG_WARN("...")`: Recoverable inconsistencies (e.g., missing a texture file fallback, using defaults).
  * `LOG_ERROR("...")`: Unrecoverable errors that require state termination (e.g., corrupted profile save files, file read failures).
* Channeling logs through macros allows the engine to route output to both the developer console and local log files in production.

---

## Part H: Performance & SFML Rules

### 1. No Hot-Loop Heap Allocations
* Allocating memory on the heap (using `new`, `std::make_unique`, or resizing collections) inside the game tick (`update()`) or render passes (`render()`) is prohibited.
* Pre-allocate dynamic collections at startup or level load times using stable pools (such as `ProjectilePool`) or pre-size vectors using `std::vector::reserve()`.

### 2. No File IO During Gameplay
* File reads, writing saves, and asset compiling are prohibited during active gameplay ticks.
* All configuration parsing (`gameplay.json`, `levels/*.json`) and asset loading must occur during transitions (loading screens, menus).
* Writes to disk must follow the [Save Trigger Policy](save_system.md#part-b-production-persistence-architecture) during checkpoints or level exits.

### 3. SFML-Specific Asset Loading Binds
* **Textures**: Textures (`sf::Texture`) must be loaded only through the unified resource cache. Direct file fetches inside gameplay classes are forbidden:
  ```cpp
  // Forbidden:
  texture.loadFromFile("assets/textures/cannon.png");
  
  // Required:
  const sf::Texture& tex = textureManager.get("cannon_glow");
  ```
* **Audio**: Only the audio manager (`AudioManager` / audio system wrapper) may own and coordinate `sf::Sound` and `sf::Music` instances.
* **Views**: Graphics shapes and drawables (`sf::Drawable` objects) belong strictly in the Rendering/View layer, never in the Gameplay simulation models.

---

## Part I: Testing Standards & Complexity Limits

### 1. AAA Testing Pattern (Arrange, Act, Assert)
All unit test suites (implemented under `tests/` using Catch2) must separate tests into three distinct blocks:
* **Arrange**: Set up the test conditions, initialize models, and allocate mockup contexts.
* **Act**: Execute the target function or trigger the event.
* **Assert**: Verify that variables, statistics, and coordinates match expected outcomes.

### 2. Code Size & Complexity Limits
To prevent the formation of massive files and god classes, code contributions must conform to these metrics:
* **Function Length**: $\le 50$ lines.
* **Cyclomatic Complexity**: $\le 10$.
* **Class Size**: $\le 500$ lines.
* **Source/Header File Length**: $\le 1000$ lines.
* *Note: When a class or function exceeds these limits, it must be refactored into smaller component parts.*

### 3. Coverage Targets
Automated test coverage must satisfy the following minimum thresholds:
* **Persistence Layer**: $\ge 80\%$ (validating schemas, atomic writes, and version migrations).
* **Gameplay Simulation Systems**: $\ge 70\%$ (validating collision reflections, damage scaling, and score multipliers).

---

## Part J: Code Review & Documentation Checklist

### 1. Documentation Standards
All public APIs (classes, member structures, public methods, and utility functions) must contain structured Doxygen comments:
```cpp
/**
 * Applies damage to the brick.
 *
 * @param amount Positive durability reduction value.
 * @return true if the brick's health reached zero and was destroyed.
 */
bool applyDamage(int amount);
```

### 2. Static Analysis Configurations
CI validation sweeps run format checking and static analysis. Contributions must run clean under the following `clang-tidy` profiles:
* `modernize-*`: Ensures the use of modern C++ standard traits (e.g. `nullptr`, `auto`, range-based loops).
* `performance-*`: Flags performance bottlenecks (e.g. copying objects where const-ref is preferred).
* `readability-*`: Checks style alignment and descriptive naming metrics.
* `bugprone-*`: Highlights structural bugs (e.g. memory leaks, uninitialized values).

### 3. Code Review Checklist (Pre-Merge Gates)
Before integrating a Pull Request (PR) into the master branch, reviewers and CI quality gates must verify:
- `[ ]` **Compilation**: Code compiles warning-free on all target platforms under `-Wall -Wextra -Werror` compiler flags.
- `[ ]` **Automated Tests**: All unit tests pass successfully.
- `[ ]` **Static Checks**: Code format audits (`clang-format`) and static analysis checks (`clang-tidy`) run clean.
- `[ ]` **Sanitizers**: Build sanitizers (AddressSanitizer and UndefinedBehaviorSanitizer) report zero violations.
- `[ ]` **Memory Ownership**: Objects have clear ownership scopes with no raw `new` / `delete` or memory leaks.
- `[ ]` **Doxygen Comments**: All public methods and classes are documented with Doxygen tags.
- `[ ]` **Documentation**: Corresponding `/docs/` guides and the developer log (`DEV-LOG.md`) are updated to reflect any API changes.

---

## Part K: Concurrency Standards

As the engine moves asynchronously to support background asset streaming, persistence file saving, and analytics tracking, threads must be managed safely.

### 1. Threading Partitioning
* **Single-Threaded Simulation**: The core game tick and physics solver (`GameplayWorld`) must remain strictly **single-threaded** to prevent race conditions and locking overhead on hot collision checks.
* **Worker Thread Tasks**: Only asynchronous tasks that are decoupled from gameplay ticks may execute on worker threads:
  * File-saving writes.
  * Asset decompression loads.
  * Telemetry logging.

### 2. Decoupling & Synchronization
* **Shared Mutable State is Forbidden**: Subsystems must not share raw pointer states across threads.
* **Message and Command Queues**: Communication between worker threads and the main game loop must use thread-safe message queues, command queues, or immutable snapshots instead of global mutex locking wrappers to avoid deadlock conditions.

---

## Part L: Architectural Layering Rules

All classes must respect the design boundaries set in [architecture.md](architecture.md):

```
    UI Layer (HUD elements, menus)
             ↓
    State Layer (PlayingState, SettingsState)
             ↓
    Gameplay Layer (GameplayWorld, Physics Solver)
             ↓
    Persistence Layer (SaveManager, ConfigManager)
```

### Layer Dependency Boundaries
* **Gameplay Decoupling**: The Gameplay layer must not depend on the UI or Rendering layers. Gameplay simulation classes must contain zero graphics calls or screen bounds polling.
* **View References**: The Rendering layer views may read simulation coordinates from Gameplay models (strictly read-only).
* **Persistence Decoupling**:
  * The Gameplay layer must not depend on the Persistence layer.
  * The Persistence layer may read Gameplay data structures for serialization.
