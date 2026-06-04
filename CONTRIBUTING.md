# Contributing to Cyberpunk Cannon Shooter (CONTRIBUTING.md)

Welcome to the team! This guide establishes our development workflows, code styling rules, and pull request procedures to ensure codebase consistency.

---

## 1. Branch Naming Guidelines

To keep our repository history clean and easily searchable, we enforce structured branch prefixes:

* **Features**: `feat/` — for new gameplay systems, weapons, or UI features.
  * *Example*: `feat/laser-sight-powerup`
* **Bug Fixes**: `fix/` — for fixing collision bugs, memory leaks, or build issues.
  * *Example*: `fix/aabb-reflection-nan`
* **Documentation**: `docs/` — for updating manuals, ADRs, or READMEs.
  * *Example*: `docs/adding-event-system-guide`
* **Refactoring**: `refactor/` — for code restructuring without behavioral shifts.
  * *Example*: `refactor/decouple-brick-view`
* **Performance**: `perf/` — for profiling optimization improvements.
  * *Example*: `perf/projectile-pool-allocations`

---

## 2. Commit Message Guidelines

We use **Semantic Commit Messages** (Conventional Commits) to format commit messages. This is enforced by our review gates:

```
<type>(<scope>): <short description>
```

### Allowed Types
* `feat`: A new feature (e.g., `feat(gameplay): implement boss rotating shield`).
* `fix`: A bug fix (e.g., `fix(save): enforce atomic write rename`).
* `docs`: Documentation updates (e.g., `docs(adr): add state stack record`).
* `style`: Code style improvements (formatting, missing semicolons) with no behavior change.
* `refactor`: Structural code changes (e.g., `refactor(physics): decouple collision checks`).
* `perf`: Performance optimizations (e.g., `perf(particles): minimize deque capacity`).
* `test`: Adding or correcting tests (e.g., `test(collision): add Catch2 unit tests`).
* `chore`: Maintenance tasks, dependencies, or CMake updates (e.g., `chore(cmake): update Catch2 to v3`).

---

## 3. Quickstart Development Setup & Local Workflows

Before submitting code, you must fetch dependencies, build, format, lint, and verify all tests pass locally.

### 1. Prerequisites
Ensure you have the following installed:
* A C++20 compliant compiler (GCC 11+, Clang 13+, MSVC 2022+).
* CMake v3.15+ and Ninja (preferred) or Make.
* SFML 3.0.0+ library frameworks.
* Catch2 v3 for tests execution.

### 2. Local Build & Test Setup
Run the following standard sequence to get your environment configured and verified:

```bash
# 1. Clone the repository
git clone https://github.com/sergey-chukhno/CasseBrick.git
cd CasseBrick

# 2. Configure build environment (Debug configurations with Ninja)
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -B build

# 3. Build all targets (compiles both gameplay targets and unit tests)
cmake --build build

# 4. Execute all tests to verify setup integrity
cd build && ctest --output-on-failure
```

### 3. Running Executables Direct
After successful compilation, you can run either the main game executable or the test suite binary from the build directory:
* Main Game: `./build/BrickBreaker`
* Test Suite: `./build/tests_run`


---

## 4. Code Formatting & Static Analysis

All code must compile warning-free and adhere to our style standard.

### 1. Code Formatting
We enforce formatting checks using `clang-format` based on the project configuration:

```bash
# Run formatting check locally on a file
clang-format -i include/gameplay/Collision.hpp src/gameplay/Collision.cpp
```

### 2. Static Analysis Checks
We use `clang-tidy` to identify modernizations, performance hazards, and bugs:

```bash
# Run clang-tidy static checks
clang-tidy src/gameplay/Collision.cpp -- -std=c++20 -Iinclude/
```
In CI pipelines, any formatting deviations or static analysis violations will block your PR from merging.

---

## 5. Pull Request Merge Checklist

Before merging a branch into `main`, it must meet our **Quality Gates**:

1. **Warning-Free Compilation**: The code must compile warning-free across all target platforms (Linux, macOS, Windows).
2. **Passed CI Target**: The GitHub Actions runner must succeed on all jobs.
3. **80% Coverage Diff Gate**: Any new or modified gameplay or persistence code must be covered by unit/integration tests with $\ge 80\%$ line coverage.
4. **Clean Sanitizers**: Local and CI sanitizer runs (ASan/UBSan) must show zero memory access errors, leaks, or undefined behaviors.
5. **Doxygen & Dev Log Parity**:
   - Newly added public methods/classes are documented with Doxygen comments.
   - A concise description of the changes is appended to [DEV-LOG.md](DEV-LOG.md).
6. **Code Review Approval**: At least one senior reviewer (and/or CodeRabbit audits) must approve the changes.

---

## 6. Pull Request Size Guidance

To ensure high-quality reviews and quick integrations, we restrict the size of individual pull requests:

* **Small (< 300 Lines of Code)**: Preferred. High priority for reviews. Covers small features, standalone bug fixes, documentation modifications, or single configuration additions.
* **Medium (< 800 Lines of Code)**: Standard. Covers typical features or minor refactors. Requires descriptive PR annotations and testing logs.
* **Large ($\ge 800$ Lines of Code)**: Disapproved unless strictly justified. PRs of this scale must be split into incremental commits or branches. Requires pre-commit architectural review and synchronization of Architecture Decision Records (ADRs).

