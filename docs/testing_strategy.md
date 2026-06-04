# Testing Strategy & Verification Manual (testing_strategy.md)

This manual defines the testing architecture, testing levels, execution procedures, and quality gates for the Cyberpunk Cannon Shooter codebase. It serves as the single source of truth for writing, running, and enforcing tests across local workstations and CI/CD pipelines.

---

## PART A: Unit Testing Architecture

To keep gameplay simulation isolated from test runners, unit tests execute in a dedicated, decoupled target separate from the main application loop.

### 1. Framework Selection
We use **Catch2 v3** as our primary testing framework. Catch2 is configured as a standalone test runner that compiles test files into a single binary (`tests_run`).

### 2. CMake Integration
The tests are configured under `tests/CMakeLists.txt`. We search for the Catch2 package and link it as follows:

```cmake
# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(BrickBreakerTests LANGUAGES CXX)

find_package(Catch2 3 REQUIRED)

add_executable(tests_run
    main.cpp
    gameplay/TestCollision.cpp
    gameplay/TestEntityUpdates.cpp
    persistence/TestAtomicSave.cpp
    persistence/TestMigration.cpp
    fakes/FakePersistenceProvider.cpp
)

target_compile_features(tests_run PRIVATE cxx_std_20)
target_link_libraries(tests_run PRIVATE Catch2::Catch2WithMain GameplayCore PersistenceCore)

# Automatically register Catch2 tests with CTest
include(Catch)
catch_discover_tests(tests_run)
```

### 3. Directory Layout
The testing directory mirrors the main codebase module taxonomy:
```
tests/
  ├── main.cpp                  # Test entrypoint (handled by Catch2WithMain)
  ├── gameplay/                 # Unit tests for simulation logic
  │     ├── TestCollision.cpp
  │     └── TestEntityUpdates.cpp
  ├── persistence/              # Unit tests for file operations
  │     ├── TestAtomicSave.cpp
  │     └── TestMigration.cpp
  ├── integration/              # Event dispatcher & interaction tests
  │     └── TestGameLoopEvents.cpp
  └── fakes/                    # Hand-written test stubs and mocks
        ├── FakePersistenceProvider.hpp
        └── FakePersistenceProvider.cpp
```

---

## PART B: Gameplay Simulation Testing

Gameplay simulation code is tested in a complete rendering-free environment. Since gameplay entities do not own SFML drawables or font handlers, we can instantiate and step the world simulation without instantiating an OpenGL context.

### 1. Math Invariant Checks
Verify math computations, including AABB bounding calculations, intersection points, normal reflections, and vector operations:

```cpp
#include <catch2/catch_test_macros.hpp>
#include "gameplay/Collision.hpp"

TEST_CASE("AABB Intersections are computed correctly", "[gameplay][math]") {
    AABB boxA{0.f, 0.f, 10.f, 10.f};
    
    SECTION("Boxes overlapping are detected") {
        AABB boxB{5.f, 5.f, 15.f, 15.f};
        REQUIRE(checkIntersection(boxA, boxB) == true);
    }

    SECTION("Boxes touch borders are detected") {
        AABB boxB{10.f, 0.f, 20.f, 10.f};
        REQUIRE(checkIntersection(boxA, boxB) == true);
    }

    SECTION("Boxes separated are not intersecting") {
        AABB boxB{12.f, 12.f, 22.f, 22.f};
        REQUIRE(checkIntersection(boxA, boxB) == false);
    }
}
```

### 2. Isolated Simulation Ticks
Instantiate a raw `GameplayWorld` container, insert test models, run update ticks with explicit time intervals, and assert simulation states:

```cpp
TEST_CASE("Entity positions update correctly over time ticks", "[gameplay][simulation]") {
    GameplayWorld world;
    auto projectile = world.spawnProjectile({0.f, 0.f}, {100.f, 0.f}); // spawn at origin, moving right
    
    // Simulate 0.1 seconds (100ms)
    world.update(0.1f);

    // Position must be at X = 10
    REQUIRE(projectile->getPosition().x == Catch::Approx(10.f));
    REQUIRE(projectile->getPosition().y == Catch::Approx(0.f));
}
```

---

## PART C: Save/Load Verification

Persistence testing focuses on validating data schema serialization/deserialization, ensuring file writing atomicity, and testing data migrations.

### 1. JSON Schema Validation
Ensure that configurations and progression saves parse safely, invalid/missing keys are assigned safe defaults, and schema fields match specifications.

### 2. Atomic Rename Validation
Verify that save actions write to a temporary file before renaming to prevent data loss on crashes:

```cpp
TEST_CASE("File writes are atomic and secure", "[persistence][save]") {
    std::filesystem::path targetFile = "test_profile.json";
    std::filesystem::remove(targetFile); // Clean state
    
    PersistenceService persistence(targetFile);
    ProfileData data{ 1500 /* credits */, 5 /* highscore */ };
    
    bool saveSuccess = persistence.writeSaveAtomic(data);
    REQUIRE(saveSuccess == true);
    REQUIRE(std::filesystem::exists(targetFile) == true);
    
    // Verify file content is intact and valid JSON
    auto loadedData = persistence.loadSave();
    REQUIRE(loadedData.has_value());
    REQUIRE(loadedData->credits == 1500);
}
```

### 3. Schema Version Migration Tests
Assert that older formats migrate cleanly to the current version:

```cpp
TEST_CASE("Older save schemas migrate to version 2 without data loss", "[persistence][migration]") {
    std::string v1Json = R"({ "save_version": 1, "credits": 500 })";
    
    SaveMigrator migrator;
    auto migratedJson = migrator.migrate(v1Json, 2);
    
    // Assert version and new default fields are appended
    REQUIRE(migratedJson["save_version"] == 2);
    REQUIRE(migratedJson["unlocked_weapons"].is_array() == true);
}
```

---

## PART D: Deterministic Replay Testing

Deterministic verification guarantees that running the game with the same inputs and identical timesteps produces bit-perfect replication.

### 1. Fixed Timestep Injection
The testing wrapper feeds a constant delta-time step (e.g. `1/60`s) into the update loop:
```cpp
constexpr float kFixedDeltaTime = 1.f / 60.f;
for (int i = 0; i < 600; ++i) {
    world.update(kFixedDeltaTime);
}
```

### 2. Input Logging & Replay Pipeline
- **Recording Phase**: During gameplay, user inputs (e.g., Cannon angle adjustments, weapon triggers, timestamp offsets) are stored sequentially in an input log structure:
  ```cpp
  struct InputFrame {
      int tick;
      float cannonAngle;
      bool fireButtonPressed;
  };
  std::vector<InputFrame> recordedInputs;
  ```
- **Replay Phase**: The test runner instantiates a fresh `GameplayWorld`, reads the `recordedInputs` array, and applies the input frame values directly to the simulation state at each tick.

### 3. Determinism Assertions via Hashing
At predefined milestones (e.g., tick 100, tick 500, tick 1000), compute a hash of the entire gameplay simulation state (including player coordinates, active projectile arrays, brick health tables) and assert it matches the reference hash:

```cpp
TEST_CASE("Gameplay simulation is deterministic under replayed inputs", "[gameplay][replay]") {
    GameplayWorld worldA;
    GameplayWorld worldB;
    
    auto inputs = loadReplayLog("test_runs/level1_input.log");
    
    applyInputsAndTick(worldA, inputs);
    applyInputsAndTick(worldB, inputs);
    
    // The final state hashes must match perfectly
    REQUIRE(worldA.computeStateHash() == worldB.computeStateHash());
}
```

---

## PART E: Integration Testing

Integration tests verify component relationships, checking how events propagate across decoupled modules.

### 1. Event Dispatcher Validations
Ensure event triggers propagate to their respective receivers without manual coupling:

```cpp
TEST_CASE("Brick destruction triggers score adjustments and audio events", "[integration]") {
    EventDispatcher dispatcher;
    ScoreSystem scoreSystem(dispatcher);
    AudioSystemMock audioSystem(dispatcher);
    
    // Trigger brick destruction event
    dispatcher.emit<BrickDestroyedEvent>({100 /* points */, BrickType::Standard});
    
    // Assert systems received and processed the event
    REQUIRE(scoreSystem.getCurrentScore() == 100);
    REQUIRE(audioSystem.hasPlayedSound("brick_shatter") == true);
}
```

### 2. State Integration Loop
Verify that states register and route correctly inside the central coordinator (`StateStack`), and transitions execute cleanly:

```cpp
TEST_CASE("State transitions execute in sequence", "[integration][states]") {
    StateStack states;
    states.pushState(StateType::MainMenu);
    states.update(0.1f);
    
    REQUIRE(states.activeState() == StateType::MainMenu);
    
    // Transition to playing
    states.changeState(StateType::Playing);
    states.update(0.1f);
    
    REQUIRE(states.activeState() == StateType::Playing);
}
```

---

## PART F: Performance Benchmarking

Performance tests run microbenchmarks to identify performance regressions and enforce allocation budgets.

### 1. Catch2 Microbenchmarks
We use Catch2's `BENCHMARK` blocks to test code blocks:

```cpp
TEST_CASE("Collision system performance benchmark", "[benchmark]") {
    GameplayWorld world;
    setupHeavyBrickGrid(world); // Spawn 500 bricks
    setupProjectileStorm(world); // Spawn 100 projectiles
    
    BENCHMARK("AABB collision check and response") {
        return world.runCollisionUpdate();
    };
}
```

### 2. Allocation Budget Audit
To verify the "zero heap allocations on the hot path" coding rule, we avoid overriding global allocation operators (such as overriding `operator new` globally, which is highly dangerous and prone to breaking Catch2 internals, the STL, or third-party dynamic libraries).

Instead, we recommend using dedicated diagnostic and profiling tools:
* **Tracy Profiler**: Integrated directly into the engine, using memory zone instrumentation (`TracyAlloc` / `TracyFree`) to track allocation counts and call stacks in real time.
* **Valgrind Massif** / **heaptrack**: Used in development and test environments to trace allocations over active gameplay cycles.
* **Visual Studio Diagnostics**: Used on Windows to capture dynamic allocations during debug target execution.

Alternatively, if programmatic assertion inside test targets is required, we use a **custom allocator** wrapper on specialized simulation collections (e.g., custom container implementations or tracking memory buffers) to capture allocator calls safely:

```cpp
template <typename T>
class TrackingAllocator {
public:
    using value_type = T;
    
    TrackingAllocator() noexcept = default;
    template <typename U> TrackingAllocator(const TrackingAllocator<U>&) noexcept {}
    
    T* allocate(std::size_t n) {
        if (trackingActive) {
            allocationsCount++;
        }
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }
    
    void deallocate(T* p, std::size_t n) noexcept {
        std::free(p);
    }
    
    static inline std::size_t allocationsCount = 0;
    static inline bool trackingActive = false;
};
```

We assert that no allocations occur during active simulation frames using tracking containers inside test mocks:
```cpp
TEST_CASE("Hot-path updates trigger zero container-level heap allocations", "[performance][allocation]") {
    // Enable tracking allocator constraints
    TrackingAllocator<int>::trackingActive = true;
    TrackingAllocator<int>::allocationsCount = 0;
    
    // Run hot path simulation frame...
    
    TrackingAllocator<int>::trackingActive = false;
    REQUIRE(TrackingAllocator<int>::allocationsCount == 0);
}
```

---

## PART G: Coverage Enforcement

We use `gcov` and `lcov` to capture and verify line-coverage thresholds.

### 1. Target Thresholds
- **Persistence Core (`PersistenceCore`)**: $\ge 80\%$ line coverage.
- **Gameplay Core (`GameplayCore`)**: $\ge 70\%$ line coverage.

### 2. 80% Coverage Diff Gate
Any code modifications introduced in a pull request must maintain at least **80% coverage on new or modified lines**.
- The CI pipeline executes the test target, generates coverage reports, and runs a coverage checker (e.g. `diff-cover` or SonarCloud integrations) comparing modified files against the base branch.
- Pull requests failing the Coverage Diff Gate are blocked from merging.

---

## PART H: CI Test Execution Strategy

The continuous integration pipeline automatically runs our tests on every commit, using AddressSanitizer and UndefinedBehaviorSanitizer to capture runtime anomalies.

### 1. Multi-Runner Matrix
GitHub Actions configures three platforms to ensure cross-compiler compliance:
- **Ubuntu Linux**: Runs `gcc` and `clang` compilers.
- **macOS latest**: Runs Apple Clang.
- **Windows latest**: Runs Microsoft Visual C++ (`msvc`).

### 2. Sanitizer Checks
CI compiles a dedicated test target configuration with memory checkers:
```bash
# ASan / UBSan target compilation
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON -B build
cmake --build build --target tests_run
```
The test run will abort and output diagnostics if a memory leak, buffer overflow, boundary violation, or undefined pointer execution occurs.

---

## PART I: Mock/Fake/Stub Rules

To prevent complex test setups and compiler bloat, **third-party dynamic mocking frameworks (e.g. GoogleMock) are prohibited**. Instead, we write lightweight stubs and fakes using pure virtual interfaces.

### 1. Interface Decoupling Principle
All external subsystems, such as persistence layers or network adapters, are accessed through base interfaces:

```cpp
// include/persistence/IPersistenceProvider.hpp
class IPersistenceProvider {
public:
    virtual ~IPersistenceProvider() = default;
    virtual bool saveString(const std::string& key, const std::string& data) = 0;
    virtual std::optional<std::string> loadString(const std::string& key) = 0;
};
```

### 2. Hand-Written Fakes
To test a component that depends on `IPersistenceProvider` without writing to disk, write an in-memory fake inside the test directory:

```cpp
// tests/fakes/FakePersistenceProvider.hpp
#include "persistence/IPersistenceProvider.hpp"
#include <unordered_map>

class FakePersistenceProvider : public IPersistenceProvider {
public:
    bool saveString(const std::string& key, const std::string& data) override {
        storage_[key] = data;
        return true;
    }
    
    std::optional<std::string> loadString(const std::string& key) override {
        auto it = storage_.find(key);
        if (it != storage_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

private:
    std::unordered_map<std::string, std::string> storage_;
};
```

Using this provider, we test the high-level `SaveManager` without hitting the file system:
```cpp
TEST_CASE("SaveManager stores data correctly via persistence provider", "[persistence]") {
    auto fakeProvider = std::make_unique<FakePersistenceProvider>();
    SaveManager saveManager(std::move(fakeProvider));
    
    saveManager.saveCredits(1200);
    
    // Verify value was captured inside the fake in-memory mapping
    REQUIRE(saveManager.getCredits() == 1200);
}
```

---

## PART J: Property-Based Testing

While standard unit tests verify known inputs against known outputs, property-based testing verifies general invariants (properties) against a large set of randomized inputs. This is valuable for math operations and collision physics where edge cases are difficult to manually write.

### reflection property validation
We run simulation updates across randomized velocity vectors and collision normal directions, verifying that reflection procedures preserve the base magnitude velocity:

```cpp
TEST_CASE("Projectile reflection preserves velocity magnitude", "[gameplay][math][property]") {
    for (int i = 0; i < 10000; ++i) {
        sf::Vector2f velocity = generateRandomVelocity();
        sf::Vector2f normal = generateRandomNormal();

        sf::Vector2f reflected = reflect(velocity, normal);

        // Verify magnitude is preserved
        REQUIRE(magnitude(reflected) == Catch::Approx(magnitude(velocity)));
    }
}
```

---

## PART K: Rendering Regression Testing (Golden Master)

To ensure rendering passes remain stable and free of layering regressions or layout breakages, the test suite executes Golden Master rendering checks.

### 1. Offscreen Render Target
Because views (`CannonView`, `BrickView`) and `RenderSystem` submit draw primitives to an generic `sf::RenderTarget`, we can direct rendering outputs to an offscreen buffer (`sf::RenderTexture`) instead of the active window:

```cpp
TEST_CASE("Gameplay screen rendering Golden Master comparison", "[rendering][regression]") {
    sf::RenderTexture offscreenBuffer;
    offscreenBuffer.create(1920, 1080);

    // Setup active mock scene with components
    GameplayWorld world;
    setupMockScene(world);

    RenderSystem renderSystem;
    renderSystem.render(offscreenBuffer, world);
    offscreenBuffer.display();

    // Export generated frame to image
    sf::Image frameImage = offscreenBuffer.capture();
    std::string frameHash = computeImageHash(frameImage);

    // Compare against pre-recorded golden master image hash
    std::string goldenHash = loadGoldenMasterHash("golden/gameplay_base.hash");
    REQUIRE(frameHash == goldenHash);
}
```

### 2. Regression Tracking
- **Golden Reference Generation**: Executed with a command-line flag (`--generate-goldens`), exporting reference PNG images to the `tests/golden/` directory.
- **Hash Comparisons**: CI runs compare SHA-256 hashes of the exported buffer images to catch layout shifts, alignment errors, sorting regressions, or missing sprites.

---

## PART L: State Machine Verification

State transitions drive the high-level application flow. We write state integration tests to ensure that valid paths are allowed, state data passes correctly, and invalid transition sequences are rejected.

### 1. Transition Checks
Assert that transitions propagate correctly through the `StateStack`:
```cpp
TEST_CASE("Valid state transitions propagate cleanly", "[integration][states]") {
    StateStack states;
    
    // Check initial state
    states.pushState(StateType::MainMenu);
    REQUIRE(states.activeState() == StateType::MainMenu);
    
    // MainMenu -> Playing
    states.changeState(StateType::Playing);
    states.update(0.1f);
    REQUIRE(states.activeState() == StateType::Playing);
    
    // Playing -> Pause
    states.changeState(StateType::Pause);
    states.update(0.1f);
    REQUIRE(states.activeState() == StateType::Pause);
    
    // Pause -> Playing
    states.changeState(StateType::Playing);
    states.update(0.1f);
    REQUIRE(states.activeState() == StateType::Playing);
    
    // Playing -> GameOver
    states.changeState(StateType::GameOver);
    states.update(0.1f);
    REQUIRE(states.activeState() == StateType::GameOver);
}
```

### 2. Invalid Transition Rejections
Assert that the state manager correctly intercepts and blocks illegal paths:
```cpp
TEST_CASE("Invalid state transitions are rejected", "[integration][states]") {
    StateStack states;
    states.pushState(StateType::GameOver);
    
    // GameOver -> Pause is illegal
    REQUIRE_THROWS_AS(states.changeState(StateType::Pause), std::invalid_argument);
    REQUIRE(states.activeState() == StateType::GameOver);
}
```

---

## PART M: Resource Cache Verification

The asset loading loop must operate deterministically without leakage or redundant disk reading calls. We test `ResourceManager` and `AudioManager` explicitly.

### Verification Checklist
- **Single Load Guarantee**: Verify that requesting the same asset resource ID twice returns identical pointer addresses, asserting that the resource is cached and not re-loaded from disk:
  ```cpp
  TEST_CASE("ResourceManager caches texture instances", "[asset][cache]") {
      ResourceManager<sf::Texture> cache;
      auto& ref1 = cache.load("textures/brick.png");
      auto& ref2 = cache.load("textures/brick.png");
      
      REQUIRE(&ref1 == &ref2); // Bit-perfect address equivalence
  }
  ```
- **Clear & Release Validation**: Assert that invoking `clear()` unloads the cached memory map:
  ```cpp
  TEST_CASE("ResourceManager releases assets on clear", "[asset][cache]") {
      ResourceManager<sf::Texture> cache;
      cache.load("textures/brick.png");
      REQUIRE(cache.size() == 1);
      
      cache.clear();
      REQUIRE(cache.size() == 0);
  }
  ```
- **Safe Fallback Recovery**: Verify that attempting to load a missing or non-existent asset configuration resolves to a predefined safe placeholder asset (e.g. a hot pink default texture) instead of throwing an unhandled exception or returning `nullptr`:
  ```cpp
  TEST_CASE("ResourceManager returns safe fallback on missing assets", "[asset][cache]") {
      ResourceManager<sf::Texture> cache;
      auto& fallback = cache.load("textures/non_existent.png");
      
      REQUIRE(cache.isFallback(fallback) == true);
  }
  ```

---

## PART N: Fuzz Testing

Save game profiles, level JSON files, and configuration files represent unsafe boundary inputs. We write fuzzing loops that supply random, corrupt byte arrays directly to parser methods to assert they fail gracefully rather than crash.

### Fuzzing Invariant Check
The file loading pipeline must capture corrupted streams, throw recovery errors, or load defaults, and must **never** trigger dynamic memory segmentation faults:

```cpp
TEST_CASE("Corrupted save file data is handled without crashing", "[persistence][fuzz]") {
    SaveManager saveManager;
    
    for (int i = 0; i < 1000; ++i) {
        std::vector<uint8_t> corruptedBytes = generateCorruptedBytes();
        
        // Assert parser throws a standard exception or returns false, never crashing
        REQUIRE_NOTHROW(saveManager.loadFromMemory(corruptedBytes));
    }
}
```

---

## PART O: Advanced Sanitizer Strategy

To guarantee memory safety and trace data race behaviors, compiler memory checkers are integrated into development builds and CI pipelines.

### 1. Memory Sanitizers Matrix
- **AddressSanitizer (ASan)**: Enabled via `-fsanitize=address` to identify out-of-bounds array checks, dangling reference pointers, use-after-free conditions, and double-free exceptions.
- **UndefinedBehaviorSanitizer (UBSan)**: Enabled via `-fsanitize=undefined` to track undefined executions (e.g., integer overflows, division by zero, null pointer dereferencing).
- **LeakSanitizer (LSan)**: Enabled automatically alongside ASan on Linux compilation loops to track memory block allocation leaks during exit.

### 2. ThreadSanitizer (TSan)
Because profile serialization and assets loading occur asynchronously on worker threads, developers should execute test sweeps under **ThreadSanitizer (TSan)** (`-fsanitize=thread`) locally to ensure shared state locks or message queues are free from data race conditions:

```bash
# Compile and run with ThreadSanitizer
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TSAN=ON -B build
cmake --build build --target tests_run
./build/tests_run
```

