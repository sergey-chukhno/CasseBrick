# Build & Deployment Manual

This manual details compiler requirements, CMake target setups, compiler flag behavior, OS-specific dependencies, and packaging workflows for distributing Cyberpunk Cannon Shooter.

---

## PART A: Build Types & C++ Standard

The engine defines compile behaviors for target environments using CMake configurations.

### 1. Supported Language Standards (C++20)
To leverage modern language primitives that optimize concurrent operations and safe array boundaries, the project requires **C++20**:
* **Concepts**: Formalizes template constraints, improving compile-time error diagnostics.
* **Spans (`std::span`)**: Provides safe bounds-checked non-owning contiguous memory access.
* **Ranges (`std::ranges`)**: Enables functional-style compositions for pipeline arrays.
* **Chrono**: Provides detailed calendar and time zone configurations for metrics.
* **Atomic Wait / Notify**: Efficient lock-free synchronization primitives for worker threads.

### 2. Build Type Profiles
* **Debug**:
  * Optimization: `-O0` (disabled) to ensure step-through execution matches source lines.
  * Debug Symbols: `-g` (enabled) to generate execution maps.
  * Assertions: Active (standard `assert` macro evaluates at runtime).
  * Logging: Active (all debug logging outputs print to console and logs).
  * Sanitizers: Optional compiler integrations (AddressSanitizer, UndefinedBehaviorSanitizer).
* **Release**:
  * Optimization: `-O3` (high speed, vectorization enabled).
  * Debug Symbols: Stripped from final package binaries to reduce footprint.
  * Assertions: Disabled via `NDEBUG` compiler definition.
  * Logging: Release builds keep `WARN` and `ERROR` logs enabled to preserve production diagnostics. `INFO` and `DEBUG` logs are disabled by default but remain configurable via runtime configuration overrides.
  * Sanitizers: Disabled to prevent execution overhead.

### 3. CMake Compiler Configurations
```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure release definitions
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
    # Strip symbols on GCC/Clang compilers
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    endif()
endif()
```

---

## PART B: Platform Support Matrix

Official targets validated by build pipelines:

| Platform | OS Version | Status | Compiler Requirements | Architecture |
| :--- | :--- | :--- | :--- | :--- |
| **Windows** | Windows 11 / 10 | Supported | MSVC (VS 2022) / MinGW-w64 | x64 |
| **Linux** | Ubuntu 24.04 / 22.04 | Supported | GCC 12+ / Clang 14+ | x86_64 |
| **macOS** | macOS 14+ (Sonoma) | Supported | Apple Clang (Xcode 15+) | Apple Silicon / Intel |
| **SteamOS** | Steam Deck | Planned | GCC 12+ | x86_64 |

---

## PART C: Build Configurations

Compile configurations are selected by defining `CMAKE_BUILD_TYPE` during project setup:

### 1. Debug Configuration
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```
* **Use Case**: Active development, step-by-step debugger tracing, and memory sanitizer validation.

### 2. Release Configuration
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
* **Use Case**: Generating optimized final builds for user packaging and deployment.

### 3. Release with Debug Information Configuration
```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build .
```
* **Use Case**: Performance profiling (e.g. running Tracy Profiler) where optimized performance is needed alongside symbol mapping.

---

## PART D: Sanitizer Builds

Sanitizers run as compiler extensions to catch memory access bugs or undefined code execution before changes merge.

### 1. Analyzer Definitions
* **AddressSanitizer (ASan)**:
  * Detects: Use-after-free, double-delete, heap corruption, and stack/buffer overflows.
* **UndefinedBehaviorSanitizer (UBSan)**:
  * Detects: Signed integer overflow, null pointer dereferences, misaligned references, and invalid type casts.

### 2. Compilation Setup
To enable sanitizers, configure CMake with analyzer flag parameters:
```bash
# Configure build with ASan and UBSan active
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON ..
cmake --build .
```
*Note: Sanitizer builds require compatible compilers (GCC, Clang, or MSVC with ASan components) and should not be used for production release distribution due to substantial performance impacts.*

---

## PART E: Packaging Pipeline

Packaging consolidates built binaries and resources into target deployment layouts, resolving paths according to the rules defined in the [Asset Manual](asset_pipeline.md).

```
  Development Build:
    build/bin/
    ├── CyberpunkCannonShooter (Binary)
    ├── assets/                (Loose directories)
    └── config/                (Gameplay JSON configs)

  Production Build:
    dist/
    ├── CyberpunkCannonShooter (Binary)
    └── assets.pack            (Consolidated archive)
```

### 1. Development Build Layout
Loose directory files are read directly from the disk:
* **Executable**: Binary.
* **Resources**: `/assets/` directory (textures, audio, fonts, shaders).
* **Configs**: `/config/` directory (wave maps, balancing values).

### 2. Production Release Layout
Loose resources are compiled into a binary package, as described in the [Asset Manual](asset_pipeline.md#part-f-packaging-pipeline):
* **Executable**: Binary.
* **Binary Pack**: `assets.pack` (contains all textures, audio, fonts, shaders, and configs).
* **Dynamic Files** (Generated on client launch):
  * `player_profile.json` (authoritative progression).
  * `settings.json` (user options).
  * `stats.json` (lifetime statistics).

---

## PART F: Release Artifacts

Packaging compression formats for distribution channels:

* **Windows**: `CyberpunkCannonShooter.zip`
  * Contains the executable (`.exe`), target dynamic libraries (`sfml-*.dll`), and `assets.pack`.
* **Linux**: `CyberpunkCannonShooter.tar.gz`
  * Contains the executable, shared libraries (`sfml-*.so`), `assets.pack`, and a launch shell script (`launch.sh`) exporting `LD_LIBRARY_PATH`.
* **macOS**: `CyberpunkCannonShooter.dmg` (Disk Image)
  * Mounts a standard application bundle wrapper `CyberpunkCannonShooter.app`:
    ```
    CyberpunkCannonShooter.app/
    └── Contents/
        ├── Info.plist
        ├── MacOS/
        │   └── CyberpunkCannonShooter (Binary)
        └── Resources/
            └── assets.pack
    ```

---

## PART G: Versioning Strategy

The project follows **Semantic Versioning (SemVer)** rules to track updates:

```
    MAJOR . MINOR . PATCH
      │       │       │
      │       │       └── Bug fixes and minor hotfixes
      │       └────────── New gameplay features or components
      └────────────────── Breaking engine architectural updates
```

### Versioning Milestones
* **`0.7.3`**: Active development, gameplay systems, and prototype persistence.
* **`0.8.0`**: Phase 1.5 Vertical Slice release (completed standard graphics and C++20 structure).
* **`1.0.0`**: Gold release (full game loops, boss types, and archive packaging).

---

## PART H: Release Checklist

Before releasing a build to distribution channels, the developer or pipeline must verify:

- `[ ]` **CI/CD Validation**: The GitHub Actions build workflow passes successfully on Windows, macOS, and Linux.
- `[ ]` **Unit Tests**: All unit tests pass cleanly (100% success rate).
- `[ ]` **Asset Validation**: `verify_assets.py` passes format, resolution, and licensing audits.
- `[ ]` **Sanitizers**: AddressSanitizer and UndefinedBehaviorSanitizer report zero issues.
- `[ ]` **Versioning**: The project version is incremented in CMake configurations according to SemVer rules.
- `[ ]` **Changelog**: Release entries are updated in `DEV-LOG.md` and related notes.
- `[ ]` **Documentation**: Corresponding user/developer manuals are updated.
- `[ ]` **Package Validation**: The compiled release package (`.zip`, `.tar.gz`, `.dmg`) successfully launches and loads the game from `assets.pack`.

---

## PART I: Crash Symbol Strategy

To enable post-mortem debugging and decode crash traces from user logs, debug symbols must be separated and archived securely during the build phase:

```
  Release Package (User):
    dist/
    ├── CyberpunkCannonShooter
    └── assets.pack

  Developer Symbols Archive:
    symbols/
    ├── CyberpunkCannonShooter.dSYM (macOS)
    └── CyberpunkCannonShooter.pdb  (Windows)
```

### Symbols Management
* **Windows**: MSVC compilation produces a `.pdb` (Program Database) file. Release builds preserve this database in developer storage; only the stripped executable is shipped.
* **macOS**: Xcode builds extract symbols into a `.dSYM` bundle using the `dsymutil` command.
* **Usage**: When a user submits a crash log containing hexadecimal callstack addresses, developers load the raw hex offsets alongside the archived `.pdb` or `.dSYM` files to map the addresses back to exact source file paths and line numbers.

---

## PART J: Installer & Distribution Targets

Planned deployment channels:

* **Steam**: Primary commercial distribution platform. Requires integration with Steam Cloud save profiles and Steam pipe packaging scripts.
* **itch.io**: Platform for early builds and playtests. Distributed via raw compressed packages (`.zip`, `.dmg`).
* **GitHub Releases**: Developer releases containing executable files alongside source tags.
* **Direct Downloads**: Available via static landing pages. Uses platform installer scripts to configure local file directories.

---

## PART K: Build Cache Strategy

To optimize developer build times and accelerate CI build pipelines, intermediate compiler outputs are cached using designated caching tools. This prevents recompiling unchanged source files, reducing build cycles by 60–80%.

### 1. Caching Tools by Platform
* **`ccache` (Linux / macOS)**:
  * Intercepts C++ compiler calls (GCC/Clang) and checks if the preprocessed file has been compiled previously.
  * Installed via system package managers: `brew install ccache` or `sudo apt install ccache`.
* **`sccache` (Windows)**:
  * Developed by Mozilla, supports MSVC compilation caching.
  * Can use local directories or cloud buckets (e.g. AWS S3, Azure Blob) as backend cache storage.

### 2. CMake Integration
CMake discovers caching tools automatically during configuration when `CMAKE_CXX_COMPILER_LAUNCHER` is set:
```bash
# Configure CMake to leverage ccache during compile steps
cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ..
```

---

## PART L: Dependency Security Scanning

To prevent security vulnerabilities from entering the codebase, automated dependency and source auditing tools are configured within GitHub workflows.

### 1. Dependabot Weekly Scan
* **Function**: Monitors external libraries and third-party dependencies (such as CMake actions and submodules).
* **Execution**: Runs weekly in the background. If a library release patches a security vulnerability (CVE), Dependabot automatically generates a Pull Request with updated configurations.

### 2. CodeQL Static Analysis
* **Function**: Executes static analysis on the C++ source code to detect security vulnerabilities, memory injection risks, and logical coding bugs.
* **Execution**: Automatically runs on every Pull Request target run and on merges to the main branch. Any high-risk vulnerability flags fail the quality gate, blocking the code integration.
