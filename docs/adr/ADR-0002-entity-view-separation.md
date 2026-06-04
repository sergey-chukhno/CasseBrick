# ADR-0002: Entity-View Separation for Graphics Isolation

* **Status**: Accepted
* **Date**: 2026-05-31
* **Author**: Senior Systems Engineer
* **Deciders**: Engineering Lead, AI Architecture Reviewer

---

## 1. Context & Problem Statement

In the initial prototype, gameplay objects (`Cannon`, `Brick`, `Projectile`) were tightly coupled with SFML rendering classes. Each object stored raw texture references, owned sprite data, and executed direct draw commands. This created significant architectural debt:
1. Physics updates and rendering were bound together, preventing headless simulation execution.
2. Unit tests could not instantiate a `Brick` or `Projectile` without raising a window and establishing an OpenGL context.
3. Decoupling the visual representation (e.g. adding shaders, trails, animations) required modifying gameplay calculation code.

---

## 2. Decision Drivers

* **Testability**: The gameplay simulation (`GameplayWorld`) must compile and run unit tests without any window context or graphics driver requirements.
* **Separation of Concerns**: Gameplay rules (collisions, movement, health) must be decoupled from graphic renderings (sprites, text positioning, particles, shaders).
* **Maintainability**: Designers should be able to update animations, assets, or shaders without modifying C++ simulation routines.

---

## 3. Considered Options

### Option 1: Direct Component Rendering (Inline SFML)
Keep the original model where objects inherit from `sf::Drawable` and maintain private sprite variables.
* **Pros**: Simple code layout.
* **Cons**: Impossible to test headlessly. Any change to the graphic representation requires recompiling the gameplay core.

### Option 2: Component-Based Architecture (ECS)
Migrate the codebase to an Entity Component System (e.g., using `EnTT` or custom ECS wrappers) dividing objects into raw entity IDs holding separate `PositionComponent` and `RenderComponent` structs.
* **Pros**: High performance, extreme flexibility.
* **Cons**: Overhead of refactoring the entire prototype. Unnecessary complexity for a game of this scale.

### Option 3: Model-View Decoupling (View Wrappers)
Gameplay objects are pure mathematical models (`Cannon`, `Brick`, `Projectile`) stored inside a simulation coordinator (`GameplayWorld`). Views are decoupled wrapper classes (`CannonView`, `BrickView`, `ProjectileView`) belonging strictly to the rendering module.
* **Pros**: Complete separation. Gameplay code contains zero SFML graphics headers. Extremely easy to test headlessly. Modest refactoring footprint.
* **Cons**: View structures must observe and mirror the model positions every frame, introducing minor coordinate translation boilerplate.

---

## 4. Proposed Decision & Rationale

We selected **Option 3: Model-View Decoupling (View Wrappers)**.

Under this pattern, simulation objects inside `include/gameplay/` maintain only gameplay state parameters (e.g., coordinates, velocities, base geometry bounds). They have no graphic variables and include no SFML rendering headers.

The rendering loop operates as a separate pipeline:
1. The gameplay tick executes on `GameplayWorld`, calculating movements and resolving collision reflections.
2. The renderer maps views to active entities (e.g. query active projectiles).
3. The View wrapper queries its model state (read-only), retrieves the associated assets from the resource cache, sets coordinate offsets, and submits drawings to the render system.

This design enables compiling the gameplay simulation as an independent static library (`GameplayCore`) that is fully unit-testable headlessly.

---

## 5. Consequences & Implications

* **Positive Impact**:
  - Headless unit testing is fully supported (Catch2 runs fast math/collision checks with zero OpenGL initialization).
  - Renderer changes (such as switching from sprites to vector animations or adding shaders) have zero impact on gameplay math.
  - Reduced build dependencies.
* **Negative Impact**:
  - Introduces view synchronizer overhead: each view must map to its entity model and query positions on every render tick.
* **Architectural Shifts**:
  - Gameplay classes must never include headers like `<SFML/Graphics.hpp>` or store `sf::Sprite`/`sf::Texture` objects.

---

## 6. Compliance Checklist

- [ ] **Zero Graphics in Gameplay**: Gameplay headers must not include SFML rendering types (`sf::Drawable`, `sf::Sprite`, `sf::Shape`, `sf::RenderWindow`).
- [ ] **Read-Only Models**: View classes must only query the coordinate and state variables of gameplay models using `const` references. Views must never mutate gameplay state during rendering.
- [ ] **Isolated Simulation Updates**: The update tick of `GameplayWorld` must remain completely rendering-free.

---

## Related Documents

- [coding_standards.md](../coding_standards.md#part-h-performance--sfml-rules)
- [reviewing-guidelines.md](../reviewing-guidelines.md#1-architectural-vigilance--separation-of-concerns-soc)
- [testing_strategy.md](../testing_strategy.md#part-b-gameplay-simulation-testing)

