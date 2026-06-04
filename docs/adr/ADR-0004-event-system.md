# ADR-0004: Synchronous Event Dispatcher for Decoupled Communication

* **Status**: Accepted
* **Date**: 2026-06-01
* **Author**: Senior Systems Engineer
* **Deciders**: Engineering Lead, AI Architecture Reviewer

---

## 1. Context & Problem Statement

As gameplay simulation grows, side-effects (such as score accumulation, achievement checks, audio triggers, and particle emissions) must execute in response to simulation milestones (e.g., a brick shattering or a projectile firing).

Direct invocation creates highly coupled code (e.g., `Brick` carrying references to `ScoreSystem`, `AudioManager`, and `ParticleSystem`). This coupling violates the Single Responsibility Principle, ruins headless testing, and complicates additions of future systems.

---

## 2. Decision Drivers

* **Zero Direct Coupling**: Simulation entities must remain isolated from side-effect systems.
* **Maintainability & Extensibility**: Adding a new reactor (e.g., telemetry analytics or achievements) must require zero modifications to simulation entity classes.
* **Deterministic Synchronous Flow**: For physics simulation repeatability, event side-effects must execute deterministically on the calling thread without introducing thread crossings or deferred execution gaps during ticks.

---

## 3. Considered Options

### Option 1: Direct Callback Nesting (Delegate Arrays)
Entities maintain callback arrays (e.g., `std::vector<std::function<void()>> onDestroyed_`) that systems bind to upon construction.
* **Pros**: Simple, zero central coordinator required.
* **Cons**: High setup boilerplate. Constructing a gameplay entity requires manual callback binding. Lifetimes are difficult to track, leading to crashes if entities outlive listener targets.

### Option 2: Asynchronous Message Queue
An event system where emitted events are appended to a queue and processed asynchronously at the end of the frame or by background worker threads.
* **Pros**: Decouples emission timing. Prevents call stack overflow.
* **Cons**: Introduces frame delays. Non-deterministic side-effects. Extremely difficult to debug and unit test repeatably. Mutex locking overhead on multithreaded queues.

### Option 3: Synchronous Event Dispatcher (`EventDispatcher`)
A centralized coordinator registry where listener classes subscribe callbacks to event types using template keys. Emitting an event instantly invokes all registered callbacks on the calling thread.
* **Pros**: Immediate execution. Deterministic replication. High decoupling. Easy mock testing (can assert events emitted).
* **Cons**: Risk of recursive loops (a callback emitting an event of the same type, causing iterator invalidation or stack overflow).

---

## 4. Proposed Decision & Rationale

We selected **Option 3: Synchronous Event Dispatcher (`EventDispatcher`)**.

The synchronous `EventDispatcher` satisfies our objectives of keeping the simulation decoupled while ensuring bit-perfect replay determinism. When a brick shatters, it emits a `BrickDestroyedEvent` payload to the dispatcher. Decoupled auxiliary systems (`ScoreSystem`, `AudioSystem`, `ParticleSystem`) register callbacks for this event and execute their reactions immediately.

By executing synchronously on the main thread, we guarantee that all state mutations occur in a defined order, which is a key requirement for our [Deterministic Replay Testing](../testing_strategy.md#part-d-deterministic-replay-testing).

---

## 5. Consequences & Implications

* **Positive Impact**:
  - Gameplay entities are 100% decoupled from auxiliary subsystems.
  - Adding new observers (e.g., an Achievement System) requires zero changes to simulation code.
  - Replay logs run deterministically because events execute immediately in order.
* **Negative Impact**:
  - Care must be taken to prevent circular cascades (e.g. Event A triggers Event B, which triggers Event A).
  - Listener classes must manage their subscription lifetimes to avoid calling deallocated target objects.
* **Architectural Shifts**:
  - The `EventDispatcher` is injected by reference into the constructors of all systems.

---

## 6. Compliance Checklist

- [ ] **Constructor Injection**: Systems responding to events must receive `EventDispatcher&` in their constructors and handle subscription registration internally.
- [ ] **No Nested Listener Mutations**: Listeners must not register or unregister themselves during an active event emission update loop.
- [ ] **POD Event Payloads**: Event data structures must be simple plain old data (POD) structs containing only read-only values.

---

## Related Documents

- [event_system.md](../event_system.md)
- [reviewing-guidelines.md](../reviewing-guidelines.md#1-architectural-vigilance--separation-of-concerns-soc)
- [testing_strategy.md](../testing_strategy.md#part-e-integration-testing)
