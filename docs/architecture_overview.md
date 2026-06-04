# Architecture Overview & Navigation Index (architecture_overview.md)

Welcome to the Cyberpunk Cannon Shooter engineering handbook. This document serves as the central directory mapping the software architecture layers, core modules, and decision paths. Use this index to navigate specifications, guidelines, and Architecture Decision Records (ADRs).

---

## 1. Architectural Layers & Dependency Flow

The engine is built on a strict, decoupled hierarchical layering system. Dependencies flow strictly top-down. Lower layers must never depend on or import components from upper layers:

```
  [UI / HUD Layer]        ──► score rendering, menus, widgets
         │
         ▼ (delegates ticks & constructs states)
  [State Layer]           ──► StateStack, State, transitions, overlays
         │
         ▼ (steps simulation & queries read-only coords)
  [Gameplay Layer]        ──► GameplayWorld, Cannon, Projectile, Brick
         ▲
         │ (snapshot serialization only)
  [Persistence Layer]     ──► SaveManager, IPersistenceProvider, JSON schemas
```

### Module Guide
* **[Gameplay Layer](gameplay_systems.md)**: Coordinates the mathematical simulation model, including physics updates, colliders, and level designs. Decoupled from rendering.
* **[Rendering Pipeline](rendering_pipeline.md)**: Manages coordinate conversions, layer sorting, and drawing submissions to viewports.
* **[Asset Pipeline](asset_pipeline.md)**: Handles compiling assets manifest parameters, resource managers, packaging pipelines, and developmental hot-reloads.
* **[Save System](save_system.md)**: Encapsulates atomic save writes, migration transitions, and fallback strategies.
* **[Event System](event_system.md)**: Configures synchronous messaging and dispatcher registrations between gameplay simulation and listeners.

---

## 2. Document Directory Index

Below is the map of our engineering documentation suite:

| Area | Manual | Description |
| :--- | :--- | :--- |
| **Guidelines** | [coding_standards.md](coding_standards.md) | Enforced coding style, parameters passing rules, and pointer limits. |
| **Guidelines** | [reviewing-guidelines.md](reviewing-guidelines.md) | Blueprint check guidelines for CodeRabbit and human reviewers. |
| **Verification** | [testing_strategy.md](testing_strategy.md) | Integration tests, Catch2 unit structures, replay steps, and sanitizers. |
| **Building** | [build_deployment.md](build_deployment.md) | Platform compiler flags, configurations, caching, and security checks. |
| **Building** | [ci_cd_pipeline.md](ci_cd_pipeline.md) | Automating workflows, coverage diff gates, and deployment scripts. |
| **Repository** | [repository_structure.md](repository_structure.md) | Folder tree taxonomies and file allocation guidelines. |

---

## 3. Architecture Decision Records (ADR) Index

ADRs detail why we chose specific patterns and define verification rules. Links to existing ADRs:

### [ADR-0001: Screen Routing via State Stack](adr/ADR-0001-state-stack.md)
* **Status**: Accepted
* **Summary**: Replaced direct state nesting with a deferred state stack overlay coordinator (`StateStack`) supporting paused layouts.

### [ADR-0002: Entity-View Separation for Graphics Isolation](adr/ADR-0002-entity-view-separation.md)
* **Status**: Accepted
* **Summary**: Isolated gameplay physics logic from rendering libraries (`sf::Sprite`, `sf::Shape`) to support headless unit testing.

### [ADR-0003: Dependency-Injected Resource Manager](adr/ADR-0003-resource-manager.md)
* **Status**: Accepted
* **Summary**: Replaced global singletons with constructor-injected resource manager caches resolving assets via string manifest IDs.

### [ADR-0004: Synchronous Event Dispatcher for Decoupled Communication](adr/ADR-0004-event-system.md)
* **Status**: Accepted
* **Summary**: Implemented a synchronous template-based event dispatcher (`EventDispatcher`) to broadcast messages (e.g. brick shattering) to detached side-effects.

### [ADR-0005: Decoupled Persistence Architecture](adr/ADR-0005-persistence-architecture.md)
* **Status**: Accepted
* **Summary**: Separated save file interactions using an abstract provider (`IPersistenceProvider`) to enable fileless testing.
