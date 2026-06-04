# Event System Specification (event_system.md)

This manual defines the architecture, dispatch rules, lifetime policies, and payload schemas for the decoupled Event System in the Cyberpunk Cannon Shooter engine. 

The Event System acts as the primary messaging bus, enabling communications between gameplay simulation updates and side-effect modules (such as audio players, particle emitters, scorekeepers, and achievement trackers) without hard class coupling.

---

## 1. Dispatcher Architecture

The messaging core is a single-threaded synchronous event dispatcher (`EventDispatcher`). 

```
  [GameplayWorld (Emitter)]
             │
             ▼ emit<BrickDestroyedEvent>(...)
    [EventDispatcher]
       ├─► ScoreSystem (Listener 1)  ──► adds points
       ├─► AudioSystem (Listener 2)  ──► plays shatter sound
       └─► ParticleSys (Listener 3)  ──► spawns spark deque
```

### Core Interface
Subscribers register handler callbacks via templates mapping specific event types. The dispatcher stores callback vectors for each event key:

```cpp
// include/events/EventDispatcher.hpp
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

class EventDispatcher {
public:
    template <typename T, typename Receiver>
    void subscribe(Receiver* receiver, void (Receiver::*memberFunction)(const T&)) {
        auto typeKey = std::type_index(typeid(T));
        auto callback = [receiver, memberFunction](const void* eventData) {
            (receiver->*memberFunction)(*static_cast<const T*>(eventData));
        };
        subscriptions_[typeKey].push_back(callback);
    }

    template <typename T>
    void emit(const T& event) {
        auto typeKey = std::type_index(typeid(T));
        auto it = subscriptions_.find(typeKey);
        if (it != subscriptions_.end()) {
            for (const auto& callback : it->second) {
                callback(&event);
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> subscriptions_;
};
```

---

## 2. Dispatch Policies & Guarantees

To prevent synchronization defects, the event system enforces strict execution guarantees:

1. **Synchronous Execution**: Event emissions are processed **synchronously**. Calling `emit<T>()` instantly iterates and invokes the registered subscriber callbacks on the calling thread. There are no asynchronous delays or thread boundary crossings during emissions.
2. **Main Thread Constraint**: Event dispatching executes strictly on the main application thread where `GameplayWorld` ticks occur. 
3. **No State Mutation During Iteration**: Subscribers must never trigger nested event emissions that modify the dispatcher's listener registrations during an active dispatch loop (which causes iterator invalidation). Any subscription modifications must occur outside active dispatch scopes.
4. **No Ordering Guarantees**: Callbacks are executed in the order they were registered. However, code designs must not rely on execution order; all subscribers should process events independently.

---

## 3. Subscription & Lifetime Management

Dangling callback references are a common source of crashes. We enforce safe lifetime policies:

* **Subscription Bounds**: Listener classes (such as `ScoreSystem` or `AudioSystem`) register their subscriptions in their constructor and must unregister or be destroyed before the `EventDispatcher` itself is deallocated.
* **Scope Injection**: The `EventDispatcher` instance is owned by the parent state (e.g. `PlayingState`) and injected by reference into the constructors of gameplay and auxiliary systems.
* **Auto-Unsubscribe Pattern**: For complex entities with dynamic lifetimes, subscribers use an RAII connection wrapper (a "subscription token") that automatically removes the callback registration from the dispatcher upon destruction.

---

## 4. Core Event Payloads

Event payloads are simple, plain old data (POD) structures containing read-only context variables:

### 1. `BrickDestroyedEvent`
Emitted by the brick matrix when a brick's health reaches zero:
```cpp
struct BrickDestroyedEvent {
    int pointsValue;           // Raw score value associated with the brick
    BrickType type;            // Standard, Shield, GoldenCore, etc.
    sf::Vector2f position;     // Spawning origin coordinates for particle explosions
};
```

### 2. `ProjectileFiredEvent`
Emitted by the Cannon when a projectile is spawned:
```cpp
struct ProjectileFiredEvent {
    sf::Vector2f position;     // Muzzle spawning coordinates
    float angle;               // Firing angle trajectory
    WeaponType weaponType;     // Cannon variant category (Standard, Heavy, Laser)
};
```

### 3. `CanisterCollectedEvent`
Emitted when a power-up canister intersects the Cannon base:
```cpp
struct CanisterCollectedEvent {
    PowerUpType type;          // MultiShot, LaserSight, tractorBeam, etc.
    float durationSeconds;     // Action duration threshold
};
```

### 4. `PlaySoundEvent`
Emitted to trigger audio feedback:
```cpp
struct PlaySoundEvent {
    std::string soundId;       // Manifest cache identifier (e.g., "explosion_heavy")
    float pitch;               // Dynamic pitch modifier (e.g., increased on high combos)
    float volume;              // Sound volume scalar (0.0 to 1.0)
};
```
