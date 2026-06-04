# ADR-0005: Decoupled Persistence Architecture

* **Status**: Accepted
* **Date**: 2026-06-01
* **Author**: Senior Systems Engineer
* **Deciders**: Engineering Lead, AI Architecture Reviewer

---

## 1. Context & Problem Statement

The prototype save game implementation directly invoked `std::ofstream` streams inside the game loop to write raw highscore strings to disk. This coupled gameplay classes to standard C++ filesystem utilities, hardcoded specific platform file paths, and risked save corruption if the game crashed mid-write.

We need a structured persistence pipeline that:
1. Prevents gameplay code from accessing the filesystem directly.
2. Supports atomic write guarantees.
3. Allows mocking the filesystem during testing so unit tests do not read/write to local workspace directories.

---

## 2. Decision Drivers

* **Test Isolation**: Serialization logic must be testable without reading or writing to the physical disk.
* **Atomic Integrity**: Save actions must be atomic (preventing half-written files).
* **Platform Independence**: Directory routing must support XDG standards on Linux, AppData on Windows, and Application Support on macOS.

---

## 3. Considered Options

### Option 1: Direct File Serialization (Prototype Pattern)
Inject file paths into state objects, which handle JSON serialization and write directly to disk streams.
* **Pros**: Simple, minimal C++ abstraction code.
* **Cons**: Impossible to unit-test without disk access. Violates the Single Responsibility Principle. Fails platform path isolation guidelines.

### Option 4: Static Persistence Managers
A static manager class (`SaveManager`) wraps filesystem accesses. States access it globally: `SaveManager::save(data)`.
* **Pros**: Decouples save formats from state logic.
* **Cons**: Relies on singletons. Hard to test in parallel. Mocking disk interactions requires swapping global configurations.

### Option 3: Interface-Driven Persistence Provider (`IPersistenceProvider`)
A decoupled architecture consisting of three layers:
1. `SaveManager`: High-level domain logic (credits, settings maps).
2. `PersistenceService`: Handles JSON parsing and atomic file rename guarantees.
3. `IPersistenceProvider`: Abstract filesystem interface (with a concrete `DiskPersistenceProvider` for production and `FakePersistenceProvider` for testing).
* **Pros**: Complete separation. Injects memory stubs during unit tests, bypassing disk access. Transparent atomic rename testing.
* **Cons**: Increased file and interface boilerplate.

---

## 4. Proposed Decision & Rationale

We selected **Option 3: Interface-Driven Persistence Provider (`IPersistenceProvider`)**.

This model encapsulates file logic completely. Subsystems do not access the disk. Instead, `SaveManager` calls `PersistenceService`, which coordinates the serializations. The raw writes are routed through `IPersistenceProvider`.

By mocking the provider (`FakePersistenceProvider` storing data in `std::unordered_map<std::string, std::string>`), we can assert that progression saves register correctly without creating physical files. For production, `DiskPersistenceProvider` resolves directories dynamically using platform guidelines (macOS, Windows, Linux/XDG) and writes files atomically via temporary rename strategies.

---

## 5. Consequences & Implications

* **Positive Impact**:
  - Test suites compile and verify saving behaviors with zero local filesystem modifications.
  - Failures and schema migration paths are tested repeatably.
  - Safe against file corruptions (atomic writes ensure old files are preserved if construction fails).
* **Negative Impact**:
  - Requires writing interfaces and test fakes, increasing code volume slightly.
* **Architectural Shifts**:
  - The `SaveManager` is constructor-injected into state managers.

---

## 6. Compliance Checklist

- [ ] **No Direct I/O in Gameplay**: Gameplay simulation files must never include `<fstream>` or call filesystem utilities directly.
- [ ] **Abstract Provider Reference**: `SaveManager` and `PersistenceService` must reference filesystem tasks via `IPersistenceProvider` base references, never concrete disk targets.
- [ ] **Atomic renaming**: Disk providers must implement the Temp Write -> Flush -> Verify -> Atomic Rename pipeline.

---

## Related Documents

- [save_system.md](../save_system.md)
- [reviewing-guidelines.md](../reviewing-guidelines.md#2-architectural-layer-validation)
- [testing_strategy.md](../testing_strategy.md#part-c-saveload-verification)
