# Architecture

A living map of your own codebase. Update it the same day you add a system. When you come
back after a two-week break, this file is what saves you.

For each system, record: **what it owns**, **who it talks to**, and **the decision you
made and why**. That last part is the valuable one.

---

## Target shape

This is where the roadmap is heading. You will not have most of it for months.

### System hierarchy (owns)

```
main.cpp
└── Game
    ├── StateStack
    │   ├── TitleState
    │   ├── FieldState
    │   │   ├── TileMap
    │   │   ├── EntityList
    │   │   ├── Camera
    │   │   ├── DialogueRunner
    │   │   └── CutsceneQueue
    │   ├── BattleState
    │   │   ├── Actors (battle view)
    │   │   ├── TurnQueue
    │   │   ├── ActionCommand
    │   │   ├── BattleUI
    │   │   └── Sequencer
    │   └── MenuState
    ├── AssetManager
    ├── InputMap
    └── AudioManager
```

### Shared data (who touches what)

| Data | Owner | FieldState | BattleState | MenuState | SaveSystem |
| --- | --- | --- | --- | --- | --- |
| PartyData | GameSession* | uses | uses | uses | reads/writes |
| GameFlags | GameSession* | uses | uses | uses | reads/writes |
| Textures / audio | AssetManager | uses | uses | uses | — |

\*Party and flags live in `GameSession` once it exists; today they are still local in sandbox `main`.

### Battle vs party (the rule)

```
  OWNS (creates + destroys)          USES ONLY (borrow)
  ─────────────────────────          ──────────────────

  GameSession ──owns──> Party        BattleState ──borrows──> Party
       │                                      │
       │                                      │ Actor& / Actor*
       │                                      │ (damage, heal, turn order)
       v                                      v
  survives battle end                  destroyed when battle ends
```

Key ownership rule to aim for: **game data (party, inventory, flags) outlives states**.
A battle must not own the party, or the party dies when the battle ends.

---

## Ownership graph (Day 18)

Current sandbox (Days 14–17) vs target game. **Owns** = creates and destroys.
**Uses** = non-owning pointer or reference only.

### Target game (keep this current as you build)

**Tree — who owns what**

```
Game
├── owns Window, AudioDevice              (RAII wrappers)
├── owns AssetManager                     (textures, fonts, sounds)
├── owns InputMap
├── owns GameSession                      (party, inventory, flags, quests — survives all states)
└── owns StateStack
    ├── owns FieldState
    │   ├── owns MapData
    │   ├── owns EntityWorld              (entities by value; refer by ID, not stored pointers)
    │   └── uses GameSession, AssetManager
    └── owns BattleState
        ├── owns Combatants               (battle-only wrappers; non-owning refs into GameSession party)
        ├── owns TurnQueue, BattleUI, Sequencer
        └── uses GameSession, AssetManager
```

**Chart — ownership vs borrowing**

| System | Relationship | Target | Lifetime |
| --- | --- | --- | --- |
| Game | owns | GameSession | whole program |
| Game | owns | StateStack | whole program |
| Game | owns | AssetManager | whole program |
| GameSession | owns | Party, inventory, flags | whole program |
| StateStack | owns | FieldState, BattleState, … | whole program |
| FieldState | owns | MapData, EntityWorld | while field is active |
| FieldState | **uses** | GameSession (party) | non-owning |
| FieldState | **uses** | AssetManager | non-owning |
| BattleState | owns | TurnQueue, Combatants, BattleUI | **battle only** |
| BattleState | **uses** | GameSession (party) | non-owning — **must not own** |
| BattleState | **uses** | AssetManager | non-owning |
| Combatants | **uses** | `Actor&` into party | non-owning pointers/refs |

**Pointer rules (Day 18)**

| Role | Type to use | Example |
| --- | --- | --- |
| Owner | `unique_ptr` or value | `vector<unique_ptr<Skill>>`, `vector<Actor> party` |
| Borrower | raw pointer or reference | `void UseSkill(const Skill* s)`, `Actor& user` |
| Rare shared ownership | `shared_ptr` + document why | avoid in this project |
| Break cycles | `weak_ptr` on back-link | `sandbox/day18/cycle.cpp` |

### Who owns what (reference table)

| Object | Owner | Users (non-owning) |
| --- | --- | --- |
| Party, inventory, quest flags | `GameSession` | `FieldState`, `BattleState`, `MenuState` |
| Battle turn order, enemy slots | `BattleState` | — (dies with battle) |
| Combatant HP during fight | `GameSession` party actors (mutated in place) | `BattleState` reads/writes via `Actor*` / `Actor&` |
| Skills on an actor/enemy | That actor's `vector<Skill>` (by value) | Battle code calls `execute` with `Skill*` or `const Skill&` |
| Polymorphic skill list (sandbox) | `vector<unique_ptr<Skill>>` | Functions take `Skill*` via `.get()` |
| Textures, fonts, audio | `AssetManager` | All states |
| Map tiles, entities on field | `FieldState` | — |

### Load-bearing rules

1. **Battle does not own the party.** `BattleState` borrows `Actor&` / `Actor*` from `GameSession`.
   When battle ends, party data must still exist for the field and save system.
2. **Default owner is `unique_ptr` or value.** `vector<Actor>` for party; `vector<Skill>` for data-driven skills.
3. **`shared_ptr` only when you can draw the graph and explain every ref count.** Prefer one owner + observers.
   Cycles need `weak_ptr` on the back-reference — see `sandbox/day18/cycle.cpp`.
4. **Never store raw pointers to `vector` elements across `push_back`.** Use indices/IDs or re-lookup each frame.

### Sandbox today (Day 17)

`main` owns `vector<Actor> party` locally. No `GameSession` yet — when battle code grows,
lift party into a session object instead of letting `BattleState` copy or own the vector.

---

## Decision log

Append only. Never delete an entry, even when you reverse it - add a new entry that says
you reversed it and why.

### D001 - raylib over SFML
**Date:** _(fill in)_
**Decision:** use raylib 6.0 as the multimedia layer.
**Why:** minimal abstraction means the architecture is visibly mine, not the library's.
Everything the game needs (sprites, tilemap rendering, input, audio, fonts, Camera2D,
shaders) is covered.
**Cost accepted:** no built-in UI, no built-in scene graph, no built-in ECS. Building
those is the point.

### D002 - _(your first real decision, around Day 40)_
**Date:**
**Decision:**
**Why:**
**Cost accepted:**

---

### D002 - where you will allow inheritance (interfaces with behaviour: `Skill`, `GameState`, `AiBehaviour`) and where you will not (enemy types, item types)
**Date**:8/28/2026
**Decision**: Enemies and items are composed structs + data, not class hierarchies. Inheritance is reserved for behavior interfaces like GameState, ActionCommand,
    and maybe AiBehaviour.
**Why**: Content variety is mostly stats/flags/skills, not new algorithms. Deep trees break when traits combine. Data scales without recompiling.
**Cost accepted**: Slightly more boilerplate now; skills may move from classes to structs later (task 5).

---

## System inventory

Fill a row in each day you finish a system.

| System | Files | Owns | Depends on | Day built |
| --- | --- | --- | --- | --- |
| Smart pointers sandbox | `sandbox/day18/smart.cpp` | `vector<unique_ptr<Skill>>` | Day 14 skill hierarchy | 18 |
| Shared_ptr cycle demo | `sandbox/day18/cycle.cpp` | `NodeA` → B via `shared_ptr`; B observes A via `weak_ptr` | — | 18 |
| Turn order | `sandbox/day17/*` | `main` owns `vector<Actor> party` | `Actor`, `Stats` | 17 |

---

## Invariants

Rules that must never be broken. Add to this list every time you fix a bug caused by
breaking an unwritten rule.

- Textures are loaded once by `AssetManager` and never unloaded mid-state.
- Update never draws; draw never mutates game state.
- No system calls raylib input functions directly except `InputMap`.
- `GameSession` owns the party; states only borrow (`Actor*` / `Actor&`). Battle must not own the party.
- Owners hold `unique_ptr` or value; users take raw pointers, references, or IDs.
- No `new`/`delete` in game code; no `shared_ptr` unless the ownership graph documents why.
