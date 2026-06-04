# ADR-0001: Screen Routing via State Stack

* **Status**: Accepted
* **Date**: 2026-05-31
* **Author**: Senior Systems Engineer
* **Deciders**: Engineering Lead, AI Architecture Reviewer

---

## 1. Context & Problem Statement

In the initial prototype, transitions between screens (Main Menu, Active Playing, Pause Screen, Game Over) were handled by nesting conditional loops or having states directly instantiate and invoke one another. This created high coupling, led to memory leak hazards during state exits, made it difficult to pause state ticks without losing active contexts, and prevented unit testing individual states in isolation.

---

## 2. Decision Drivers

* **Encapsulation**: State transitions should be driven by a centralized manager rather than individual screens owning their successors.
* **Pause Context Preservation**: Pausing the game should overlay a Pause screen on top of the playing state without destroying active gameplay coordinates, physics caches, or entity positions.
* **Zero Coupling**: Game states should be completely independent of one another.

---

## 3. Considered Options

### Option 1: Direct State Instantiation
Each state class holds a unique pointer to its potential successors and invokes state changes inline.
* **Pros**: Simple to write for small projects.
* **Cons**: Extreme class coupling. Exit and entry logic is scattered, leading to memory leaks and invalid state contexts.

### Option 2: State Machine (Single Active State)
A centralized manager owns a single active state pointer. Transitioning between states deletes the old state and instantiates the new one.
* **Pros**: Decouples states from each other. Centralizes transition logic.
* **Cons**: Inability to overlay states. Opening a Pause menu or Settings page requires destroying the active Gameplay state, meaning the session cannot be resumed without complex serialization save/load cycles.

### Option 3: State Stack (`StateStack`)
A centralized coordinator manages an array/vector stack of active state interfaces (`State`). States can be pushed or popped. The stack update and draw loops execute from the top down, allowing translucent overlays.
* **Pros**: Allows pausing (pushing a Pause state on top of the Playing state stops updates below it but continues drawing it in the background). Decouples states completely.
* **Cons**: Minor overhead of managing the stack queue (push, pop, clear requests are deferred to the end of the frame tick to prevent stack corruption).

---

## 4. Proposed Decision & Rationale

We selected **Option 3: State Stack (`StateStack`)**. 

The `StateStack` acts as the coordinator. States request changes via deferred command buffers (e.g. `requestStackPush(StateType::Pause)`), which are processed safely at the end of the frame update step. This prevents modifying the stack contents while actively iterating through its update loops. It also enables beautiful cyberpunk overlays: when the Pause state is active on top, the Gameplay state below continues to draw (providing a blurred background) but skips its physics and entity updates, preserving the exact simulation state.

---

## 5. Consequences & Implications

* **Positive Impact**:
  - Clean separation: states are completely decoupled and share no direct header dependencies.
  - Safe transitions: exit and entry routines execute deterministically.
  - Interactive overlays: support for translucent HUDs, pause screens, and warning indicators.
* **Negative Impact**:
  - Actions are deferred, requiring states to use a callback or dispatcher interface to request stack operations.
* **Architectural Shifts**:
  - The application context (`Game`) owns the `StateStack` and delegates the main `update()` and `render()` ticks to it.

---

## 6. Compliance Checklist

- [ ] **Deferred Stack Changes**: States must never mutate the stack hierarchy directly during update loops; all changes must be submitted via deferred push/pop requests.
- [ ] **Inheritance Constraint**: Every state screen must inherit from the base `State` interface and implement its pure virtual `update(float dt)`, `draw(sf::RenderTarget& target)`, and `handleEvent(const sf::Event& event)` functions.
- [ ] **View Transparency**: Translucent states (like `PauseState`) must return `true` for `draw` containment queries to allow rendering the underlying gameplay state in the background.

---

## Related Documents

- [coding_standards.md](../coding_standards.md#part-l-architectural-layering-rules)
- [reviewing-guidelines.md](../reviewing-guidelines.md#2-architectural-layer-validation)
- [testing_strategy.md](../testing_strategy.md#part-l-state-machine-verification)

