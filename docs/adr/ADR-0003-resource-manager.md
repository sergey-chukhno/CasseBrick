# ADR-0003: Dependency-Injected Resource Manager

* **Status**: Accepted
* **Date**: 2026-05-31
* **Author**: Senior Systems Engineer
* **Deciders**: Engineering Lead, AI Architecture Reviewer

---

## 1. Context & Problem Statement

In the early prototype, assets were loaded on-demand using static singleton patterns (e.g. `TextureManager::getInstance()`). This created several architectural issues:
1. **Global Mutable State**: Global managers made it difficult to predict resource lifetimes, leading to dangling references or memory leaks at exit.
2. **Path Hardcoding**: File paths (e.g., `"assets/textures/cannon.png"`) were hardcoded directly in gameplay or view classes, making updates difficult if folders were restructured.
3. **Testing Obstacles**: Mocking resources was impossible because the static managers always searched physical disk paths.
4. **Hitch Hazards**: Loading textures directly inside drawing steps led to frame drops (hitches) during active gameplay.

---

## 2. Decision Drivers

* **Zero Hardcoded Paths**: Code must load resources strictly via metadata IDs (e.g., `"cannon_body"`), decoupling disk layouts from C++ compilations.
* **Test Isolation**: Subsystems must support dependency mocking (e.g., injecting a fake mock cache during test runs).
* **Deterministic Lifetimes**: Asset caching lifecycles must be tied to the application states (loading during transition phases, clearing on exits).

---

## 3. Considered Options

### Option 1: Global Singleton Cache (Prototype Pattern)
Retain global static instances of managers (`TextureCache::getInstance()`) but add pre-loading phases.
* **Pros**: Easy to invoke from anywhere in the codebase.
* **Cons**: Introduces global state. Creates order-of-destruction conflicts at shutdown. Prevents injecting fake managers during testing.

### Option 2: Service Locator Pattern
Use a centralized static registry (`ServiceLocator::getTextureCache()`) where managers register themselves at startup.
* **Pros**: Decouples interface from concrete implementations.
* **Cons**: Still relies on global registry states. Hidden dependencies (classes retrieve managers internally without exposing them in constructors).

### Option 3: Dependency-Injected `ResourceManager` Instances
Define a templated `ResourceManager<T>` class. Instantiated managers (e.g., textures, fonts) are owned by the root application shell (`Game` class) and passed as references or shared pointers into constructors of views or states. Resources are resolved via manifest registries (`assets_manifest.json`).
* **Pros**: Deterministic lifetime bounds. No global mutable state. Simple constructor injection clearly documents class dependencies. Facilitates mock testing.
* **Cons**: Minor boilerplate overhead of passing manager references through constructors.

---

## 4. Proposed Decision & Rationale

We selected **Option 3: Dependency-Injected `ResourceManager` Instances**.

Under this design, `ResourceManager<sf::Texture>` and `ResourceManager<sf::Font>` instances are owned by the `Game` context. When a state (like `PlayingState`) is constructed, the `Game` object injects references to these managers.

At startup, the managers read `assets_manifest.json` to cache the mapping between string IDs and disk paths. Views request assets using string IDs:
```cpp
// Correct manifest ID load
auto& texture = textureCache.get("cannon_base");
```
This isolates path alterations to the JSON manifest. Lifetime is strictly bound: when `Game` is destroyed, all cached assets are freed in a controlled sequence, avoiding static destruction conflicts.

---

## 5. Consequences & Implications

* **Positive Impact**:
  - Direct compliance with our [Coding Standards](coding_standards.md#part-e-memory-const-correctness--move-semantics): global singletons are banned.
  - Zero static initialization or destruction order bugs.
  - Clean unit testing (fake/mock caches can be injected easily).
  - Clearer class APIs (dependencies are visible in constructors).
* **Negative Impact**:
  - Passing references down state hierarchies increases constructor parameters slightly.
* **Architectural Shifts**:
  - Every view must receive its resource cache reference during initialization.

---

## 6. Compliance Checklist

- [ ] **No Singleton Managers**: Classes must not implement static `getInstance()` or `sharedInstance()` resource cache accesses.
- [ ] **Constructor Injection**: Any view or state requiring asset resources must receive references to the appropriate `ResourceManager` instances via its constructor.
- [ ] **Path Separation**: Raw file path strings (e.g., `"assets/..."`) are prohibited in C++ files. Resources must be requested strictly using manifest IDs.

---

## Related Documents

- [asset_pipeline.md](../asset_pipeline.md#part-d-resource-manager-design)
- [reviewing-guidelines.md](../reviewing-guidelines.md#8-sfml-compliance-resource-caching)
- [testing_strategy.md](../testing_strategy.md#part-m-resource-cache-verification)

