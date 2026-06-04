# CI/CD Pipeline & Quality Gates

This manual defines the automated continuous integration and continuous delivery (CI/CD) pipelines, code quality gates, security scanners, and release deployment policies for Cyberpunk Cannon Shooter.

---

## Part A: CI/CD Pipeline Overview

To enforce code quality and prevent regressions, all contributions are validated by automated pipelines prior to branch merging.

```
  [Developer Pull Request]
             │
             ▼
  [Linting & Code Formatting]  ──► fails PR if style check fails (clang-format)
             │
             ▼
  [Static Analysis Audits]     ──► fails PR if compiler warnings/tidy bugs occur
             │
             ▼
  [Multi-platform Compile]     ──► compiles in parallel (Windows, Linux, macOS)
             │
             ▼
  [Catch2 Test Execution]      ──► runs unit tests, checks coverage targets
             │
             ▼
  [CodeQL & Dependabot Scan]   ──► scans source and libraries for vulnerabilities
             │
             ▼
  [CodeRabbit AI Review]       ──► evaluates architecture and naming guidelines
             │
             ▼
     [Merge to Main]
             │
             ▼
 [v* Release Tag Trigger]      ──► packages, strips symbols, publishes bundles
```

---

## Part B: GitHub Actions Workflows

Automated build verification pipelines are configured in GitHub Action workflows under `.github/workflows/ci.yml`.

### 1. Trigger Policies
The validation workflow is triggered on:
* **Pull Requests**: Every PR targeting the `main` branch.
* **Push Commits**: Every direct merge commit to the `main` branch.
* **Tag Pushes**: Any tag push matching the release pattern (e.g. `v1.2.0`).

### 2. Multi-Platform Build Runners
The build matrix compiles the C++ codebase using parallel runners to ensure cross-platform compatibility:
* **Windows Runner (`windows-latest`)**: Compiles via MSVC (Visual Studio compiler tools).
* **Linux Runner (`ubuntu-latest`)**: Compiles via GCC and Clang compilers.
* **macOS Runner (`macos-latest`)**: Compiles via Apple Clang.

---

## Part C: CodeRabbit Integration

CodeRabbit is configured via `.coderabbit.yaml` in the repository root to perform automated AI-assisted code reviews.

### 1. Review Directives
* **Architectural Alignment**: CodeRabbit queries [reviewing-guidelines.md](reviewing-guidelines.md) to inspect PR contributions for architectural separation (e.g., ensuring no SFML draw calls leak into simulation models, or verifying reference-based dependency injection).
* **Review Tone**: Configured to provide neutral, direct, and constructive feedback focusing on code safety, memory allocation traps, and performance bottlenecks.
* **Path Filters**: Excludes internal configurations, test data, and build scripts to focus reviews strictly on source files under `/src` and `/include`.

---

## Part D: Formatting Quality Gate (`clang-format`)

Code format consistency is audited automatically during the initial stage of the CI pipeline:
* **Tooling**: Uses `clang-format` based on the style configurations defined in `.clang-format`.
* **Gate Behavior**:
  * The CI runner scans modified C++ source and header files (`.cpp`, `.hpp`).
  * If any code modifications violate format style constraints, the formatting step fails, blocking the PR from merging.
  * Developers can resolve format errors locally prior to pushing using:
    ```bash
    clang-format -i -style=file src/**/*.cpp include/**/*.hpp
    ```

---

## Part E: Static Analysis Quality Gate (`clang-tidy`)

Following formatting verification, compiler checks and static analysis tools audit the codebase:
* **Warnings as Errors**: The CMake build configuration enforces warning-free compilations under GCC/Clang and MSVC:
  ```cmake
  # Treat compiler warnings as errors to prevent code debt
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options(-Wall -Wextra -Werror -Wpedantic)
  elseif(MSVC)
      add_compile_options(/W4 /WX)
  endif()
  ```
* **`clang-tidy` Auditing**: The pipeline runs static analysis checks checking the profiles defined in [coding_standards.md](coding_standards.md):
  * `modernize-*`: Enforces modern C++ standard traits (such as `nullptr`, `auto`, and range loops).
  * `performance-*`: Flags performance inefficiencies (such as unnecessary object copies).
  * `readability-*`: Checks readability style metrics and identifier casing rules.
  * `bugprone-*`: Highlights structural coding bugs (such as potential null dereferences or uninitialized variables).

---

## Part F: Unit Test Execution (Catch2)

* **Test Framework**: Automated testing uses Catch2.
* **Execution Workflow**:
  * During the compile stage, CMake configures the test binary target `RunTests`.
  * The CI pipeline executes the test target using CTest:
    ```bash
    ctest --output-on-failure
    ```
  * Any failed test assertions (`REQUIRE`, `CHECK`) fail the Catch2 test stage, blocking the Pull Request.

---

## Part G: Code Coverage Thresholds

To verify code test depth, the CI pipeline compiles coverage metrics using coverage instrumentation (e.g. GCC `gcov` and `lcov` profiles) and uploads results to analysis portals (such as Codecov or SonarCloud).

### 1. Minimum Coverage Gate Thresholds
Test suites must satisfy the coverage boundaries defined in the [Coding Standards](coding_standards.md#part-i-testing-standards--complexity-limits):

| Subsystem / Layer | Minimum Line Coverage | Target Components |
| :--- | :--- | :--- |
| **Persistence Layer** | **$\ge 80\%$** | JSON serializations, atomic writes, path resolutions, and schema version migrations. |
| **Gameplay Simulation** | **$\ge 70\%$** | Collision calculations, AABB physics solvers, health deductions, and scoring math. |

### 2. Coverage Diff Gate (PR Quality Gate)
While total coverage thresholds check the health of the overall codebase, they can mask newly introduced or modified code that contains zero tests.
* **Diff Coverage Target**: New or modified code introduced in any Pull Request must maintain at least **$80\%$ diff coverage** (as calculated via Codecov or SonarCloud).
* **Gate Behavior**: If a Pull Request passes compilation and total coverage targets but fails to cover at least $80\%$ of its own modified lines, the quality gate fails, blocking merge integration.

---

## Part H: Security Scanning

The GitHub repository configures security scanning suites to secure source files and dependencies:

### 1. CodeQL Static Analysis
* CodeQL parses C++ source code during CI runs to identify security vulnerabilities.
* The scanner audits the codebase for:
  * Buffer overflows and memory leaks.
  * Integer overflows or divisions by zero.
  * Path traversal risks or unsafe file system writes.
  * Cryptographic flaws and logical safety issues.

### 2. Dependabot Alerts & Scans
* Dependabot runs weekly scans of the repository configuration files (such as GitHub action actions and submodules).
* If a third-party dependency is flagged with a known security vulnerability (CVE), Dependabot logs a warning and automatically creates a Pull Request updating the dependency to a safe version.

### 3. GitHub Secret Scanning
* Secret Scanning checks the commit history on every push to detect credentials before they reach the repository.
* Flags and scans for:
  * API keys and database passwords.
  * Steam developer credentials and integration tokens.
  * AWS keys and private deployment certificates.
* **Gate Behavior**: If a secret is identified in any pushed commit, the scanning service triggers a high-priority alert, blocks the associated PR runner, and triggers automated remediation protocols (e.g., notifying the security owner and starting token revocation).

---

## Part I: Branch Protection Rules

The `main` branch is protected by repository access controls to guarantee that only validated code is integrated:
1. **Require Pull Requests**: Direct pushes to the `main` branch are disabled. All changes must be proposed via Pull Requests.
2. **Require Status Checks to Pass**: Code cannot be merged unless all status checks pass successfully:
   * Compilations on Windows, Linux, and macOS.
   * `clang-format` and `clang-tidy` quality gates.
   * Catch2 unit test executions.
   * Coverage targets and the **80% Coverage Diff Gate**.
   * CodeQL and GitHub Secret Scanning runs.
3. **Require Approvals**: A Pull Request requires at least one approval from a designated code owner before merging.
4. **Require Linear History**: Merges must use squash merges or rebase updates to maintain a clean linear git history.
5. **Disable Force Pushes**: Force pushing (`git push --force`) and deleting the `main` branch are blocked.

---

## Part J: Automated Release Packaging

When a release version is finalized, the pipeline automates the generation and distribution of binaries:

### 1. Release Trigger
Pushed git tags matching version patterns (e.g., `v1.0.0`, `v0.8.0`) trigger the release packaging pipeline.

### 2. Packaging Pipeline Execution
When triggered, the release runner compiles release configurations in parallel across Windows, Linux, and macOS:
1. Compiles optimized binaries under **C++20** standard with `NDEBUG` enabled.
2. Runs the Python pack tool (`packer.py`) to compile the binary resource package `assets.pack`, as defined in the [Asset Manual](asset_pipeline.md#part-f-packaging-pipeline).
3. Strips debugging symbols from the release binary, archiving symbol databases (`.pdb` and `.dSYM` packages) inside developer storage as described in the [Build Manual](build_deployment.md#part-i-crash-symbol-strategy).
4. Compresses deployment files:
   * Windows: Packages `.exe`, libraries, and `assets.pack` into `CyberpunkCannonShooter.zip`.
   * Linux: Packages binary, libraries, wrapper launch script, and `assets.pack` into `CyberpunkCannonShooter.tar.gz`.
   * macOS: Compiles libraries and `assets.pack` into a bundled application wrapper (`.app`) inside a `CyberpunkCannonShooter.dmg` disk installer.
5. Automatically creates a new GitHub Release draft containing the target version tag, inserts changelogs from `DEV-LOG.md`, and uploads the compressed release packages.
6. Deploys package updates to external distribution targets (such as itch.io or Steam staging pipes).
