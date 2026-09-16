# mini-ecs

A small Entity-Component-System (ECS) implementation in C++, built from scratch to explore
data-oriented design and measure its real performance impact against a traditional
inheritance-based ("OOP") architecture.

<img width="640" height="420" alt="Recording 2026-09-163 120327" src="https://github.com/user-attachments/assets/149ddc0c-f84e-4307-8977-3bd9efb9655b" />

Three collision implementations are benchmarked head-to-head:
- **OOP baseline** — classic `GameObject` base class, virtual dispatch, heap-allocated objects
- **ECS, naive O(n²) collision** — sparse-set component storage, but brute-force pairwise checks
- **ECS, spatial hash grid** — sparse-set storage + broad-phase spatial partitioning

## What is an ECS?

Traditional game object architectures attach data *and* behaviour to a class (`Player: GameObject`,
`Enemy: GameObject`), relying on inheritance and virtual methods. This is intuitive but has two
costs at scale: inheritance hierarchies get awkward as behaviours mix, and object data is scattered
across the heap; iterating thousands of objects means jumping around memory unpredictably (cache
misses).

ECS splits data and behaviour apart entirely:
- **Entity** — just an ID (uint32_t in this case).
- **Component** — pure data (`Position { x, y }`, `Velocity { dx, dy }`). No logic.
- **System** — pure logic (`MovementSystem`: for every entity with Position *and* Velocity, apply
  velocity to position). No data of its own.

Components of the same type are stored contiguously, so systems iterate tight, cache-friendly
arrays instead of scattered heap objects.

## Architecture

```
Entity          = uint32_t (just an ID)
Component       = plain struct (Position, Velocity, Circle)
SparseSet<T>     = O(1) add/remove/lookup via sparse-array indirection into a packed dense array
System           = free function operating on one or more SparseSet<T>s
SpatialHashGrid  = buckets entities into fixed-size grid cells for fast nearby queries
```

Component storage progressed through two implementations, each solving a different cost:

1. **`unordered_map<Entity, T>`** (initial) every `get()` is a hash lookup.
2. **Sparse set** (`sparse[entity] → index into dense[]`) — `get()` becomes direct array indexing,
   and components can be iterated as a flat, contiguous array. Removal uses swap-and-pop to keep
   `dense` gap-free.

Collision detection progressed similarly:

1. **Naive O(n²)** — every entity checked against every other entity.
2. **Spatial hash grid** — entities bucketed into grid cells each frame; each entity only checked
   against the ~9 cells (its own + 8 neighbours) around it, avoiding the vast majority of
   irrelevant pairwise checks.

## Benchmark

All three variants run the identical simulation (same entity count, same RNG seed, same movement/
wall-bounce/collision logic). The OOP and naive ECS versions use pairwise collision detection, 
while the optimised ECS version additionally uses a spatial hash grid. 
Rendering is excluded from timing; only the update step (movement + wall bounce + collision) is measured, using
`std::chrono::high_resolution_clock`, averaged over 300 frames, uncapped frame rate, release build.

| Entities | OOP baseline (ms) | ECS naive O(n²) (ms) | ECS spatial grid (ms) |
|---:|---:|---:|---:|
| 500    | 0.30   | 0.60   | 1.08  |
| 1,000  | 0.95   | 2.08   | 1.45  |
| 2,000  | 3.48   | 7.23   | 2.29  |
| 4,000  | 12.97  | 26.67  | 4.24  |
| 10,000 | 70.28  | 138.61 | 9.79  |


```
Update time at 10,000 entities

OOP baseline        70.28 ms  ████████████████████████████████████
ECS naive O(n²)     138.61 ms ██████████████████████████████████████████████████████████████████████
ECS spatial grid    9.79 ms   █████
```

**At 10,000 entities, the spatial-grid ECS is ~7.2x faster than the OOP baseline and ~14.2x faster
than the naive ECS collision.** The naive and OOP curves both grow superlinearly (roughly
quadratic, as expected for O(n²) pairwise checks); the spatial-grid implementation scales much more gradually with entity count.

### The naive ECS result is slower than OOP; here's the explanation

Sparse-set ECS is *not* faster than OOP here when the collision algorithm itself is still O(n²).
The naive ECS collision system performs four sparse-set lookups per pair checked
(`positions.get(a)`, `positions.get(b)`, `circles.get(a)`, `circles.get(b)`), each an indirection
through a sparse array plus a bounds check. The OOP baseline instead holds raw `GameObject*`
pointers directly in a `vector`, so a pair check is just direct member access, no indirection at
all. ECS's storage layout advantage shows up clearly in straight-line iteration (see
`movementSystem`, which iterates one packed array), but it doesn't automatically help and can
actively hurt for algorithms that do repeated point-to-point lookups across the entity set. The
real win here came specifically from pairing ECS with an algorithmic change (spatial partitioning),
not from switching architecture alone.

## Known simplifications

- Collision response swaps velocities on contact rather than computing a physically accurate
  elastic-collision impulse — a placeholder chosen to keep focus on the ECS/benchmark work rather
  than physics accuracy.
- `World::storage<T>()` uses a `static` instance per component type, which works for a single
  `World` but wouldn't support multiple concurrent worlds. A real ECS would use a per-World
  component registry instead.
- The spatial hash grid is rebuilt from scratch every frame. This is simple and correct but not
  optimal; an incremental update (only moving entities that changed cells) would reduce overhead
  further, particularly noticeable in the fixed cost visible at low entity counts (grid is slower
  than OOP below ~1,500 entities).

## Build & run

Each variant is a separate raylib project (`src/ecs/`, `src/oop/`) built via `make`. See
`.vscode/tasks.json` for build configurations and `.vscode/launch.json` for run configurations
(`Run ECS (release)`, `Run OOP (release)`). Collision mode (naive vs. spatial grid) and entity
count are set as constants at the top of `src/ecs/main.cpp`.

## Possible next steps

- Replace the velocity-swap collision response with a proper impulse-based resolution
- Migrate the spatial grid to incremental updates instead of a full rebuild per frame
- Explore archetype-based storage (grouping entities by exact component signature) as a further
  iteration-speed optimization
