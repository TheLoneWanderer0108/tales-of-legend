# Tales of Legend - Learning Roadmap

A 113-day, day-by-day plan to learn C++ by building a custom JRPG engine, showcased by a small
demo JRPG with Paper Mario-style interactive battles, using
[raylib 6.0](https://github.com/raysan5/raylib) as the window/rendering/audio/input layer and
writing every engine system yourself.

**The engine is the product; the demo is the proof that it works.** The demo is deliberately
small: two areas, a couple of enemies, one boss, one or two NPCs, and about 10 moves, built on
Day 120b with no new engine systems.

Sized for **2-3 hours per day, about 5 days per week**. That is roughly 5 months of work days -
budget more calendar time than that, because some days will take two or three sittings and that is
normal, not failure.

Day numbers come from the original 170-day plan and were never renumbered, because days refer to
each other. So there are gaps (Day 61 does not exist, Day 69b does). See
[TRIM_REPORT.md](TRIM_REPORT.md) for what was cut and why.

---

## The rules

These exist because the goal is not "a finished game as fast as possible". The goal is
"a finished engine and demo **and** a programmer who can build the next one".

1. **Never copy code you cannot explain line by line.** This roadmap deliberately gives
   you function signatures, goals and constraints - not finished function bodies. If you
   paste something in, you owe yourself a rewrite from memory afterwards.
2. **Placeholder art until Phase 8.** Colored rectangles are fine. Enemies are red
   rectangles, the player is a blue rectangle, chests are yellow. Hunting for art packs
   is the single most effective way to spend three weeks making zero progress.
3. **Commit every day, even broken days.** Message format:
   `day 47: tile collision - learned why you resolve axes separately`.
   Your git log becomes a record of your learning.
4. **Two lines in [../journal.md](../journal.md), every day.** What broke, what fixed it.
   In four months you will read Day 12 and not believe how confused you were. That
   feeling is the whole point.
5. **The 30-minute rule.** Stuck for 30 minutes with zero forward motion? Take the
   "Stuck?" fallback listed in the day. Stuck for 60? Write the question down in the
   journal, skip to the next task, come back tomorrow. Do not spend a whole session
   frozen.
6. **Days are units of work, not calendar days.** If Day 47 takes you three sittings,
   it is still Day 47. Never renumber, never feel behind. The only failure mode is
   stopping.
7. **Run it every day.** Never end a session with code that does not compile. If you
   have to comment out your work-in-progress to get a clean build, do that, then commit.
8. **Type the code out.** Do not use autocomplete-heavy AI assistance to write your game
   systems during Phases 1-6. Use documentation, use search, ask *why* questions freely.
   Muscle memory for C++ syntax only comes from typing C++ syntax.
9. **Do not skip the interlude.** The six days after Phase 2 exist to make you release
   something small and finish it. Finishing is a separate skill from building, and this is
   where you practise it cheaply. It will feel like a detour from the real project. It is
   the insurance policy on the real project.

---

## How each day is written

Every one of the 113 days follows the same shape, so you never open the file and wonder
what to do:

```markdown
### Day 47 - Tile collision resolution

**Goal:** the player cannot walk through walls and slides along them instead of stopping.

**New C++ concepts:** early return, `const&` parameters, integer division with negatives.

**Research first (20 min):** [collision-detection.md](../research/collision-detection.md)
sections 3-4.

**Tasks**

- [ ] Add `bool TileMap::IsSolid(int tx, int ty) const`
- [ ] Convert a world position to tile coordinates, handling negative coordinates
- [ ] Resolve the X axis and the Y axis separately
- [ ] Draw a debug overlay of solid tiles, toggled with F1

**Deliverable:** walk into a wall from all four directions with no tunnelling, even at
10x speed.

**Stuck?** Hardcode a single solid tile at (5, 5) and get *that* working before loading
solidity from the map.
```

Read the research note **before** writing code. That is the part that turns "I got it
working" into "I understand it".

---

## Phases

| Phase | Days | Work days | Title | You will have built |
| --- | --- | --- | --- | --- |
| [00](phase-00-setup.md) | 1-3 | 3 | Setup and tooling | A compiling C++ project, git, a debugger you can drive |
| [01](phase-01-cpp.md) | 4-24 | 21 | C++ fundamentals (console only) | A complete text-only turn-based battle |
| [02](phase-02-raylib.md) | 25-36 | 12 | raylib fundamentals | A sprite moving in a room, colliding with walls |
| [--](interlude-ship-something.md) | 36a-36f | 6 | **Interlude: ship something tiny** | A finished, released micro-game, and proof you can finish |
| [03](phase-03-engine.md) | 37-53 | 17 | Engine architecture | State machine, assets, entities, tilemaps, camera |
| [04](phase-04-overworld.md) | 54-71 | 13 | Overworld and dialogue | NPCs, branching dialogue, flags, cutscenes, field encounters, a minimal save |
| [05](phase-05-battle.md) | 72-95 | 20 | Battle system and action commands | The Paper Mario combat core, particles and effects, one boss |
| [06](phase-06-systems.md) | 96-117 | 8 | Items, UI and saving | Items, a UI framework, versioned save/load, a title screen |
| [07](phase-07-content.md) | 118-120b | 3 | Content pipeline and vertical slice | Validation, hot reload, the demo JRPG |
| [08](phase-08-ship.md) | 151-170 | 16 | Polish and ship | Juice, real art, a downloadable demo |

---

## Progress tracker

Fill in the date when you finish each block. Nothing motivates like a filling table.

| Block | Days | Finished on |
| --- | --- | --- |
| Setup | 1-3 | |
| C++ basics | 4-10 | |
| C++ objects | 11-17 | |
| C++ modern | 18-20 | |
| Console battle capstone | 21-24 | |
| raylib basics | 25-30 | |
| raylib intermediate | 31-36 | |
| **Interlude: shipped a tiny game** | 36a-36f | |
| Engine core | 37-45 | |
| Tilemaps and camera | 46-53 | |
| NPCs and dialogue | 54-60 | |
| Cutscenes, encounters and minimal save | 63-71 | |
| Battle core | 72-81 | |
| Action commands | 82-89 | |
| Boss, effects and consolidation | 91-95 | |
| Items | 96-97 | |
| UI framework, save and boot flow | 106-117 | |
| Content pipeline | 119-120 | |
| **Vertical slice: the demo** | 120b | |
| Polish | 151-160 | |
| Ship | 163-170 | |

---

## Repository layout you will end up with

You create these as the roadmap asks for them. Do not create them all on Day 1.

```
tales-of-legend/
  CMakeLists.txt
  src/
    main.cpp
    core/          game loop, states, input, assets, events
    world/         tilemap, entities, camera, collision
    battle/        battle state machine, actors, action commands
    ui/            menus, dialogue box, HUD
    data/          loaders for JSON content
  assets/
    sprites/  audio/  fonts/  maps/  data/
  docs/            this roadmap
  build/           generated, git-ignored
  sandbox/         throwaway learning programs from Phase 1
```

---

## Reference shelf

Keep these five open in tabs. You will use them constantly.

- [raylib cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html) - every function
  raylib has, on one page.
- [raylib examples](https://www.raylib.com/examples.html) - runnable examples for
  almost everything.
- [cppreference](https://en.cppreference.com/w/) - the real C++ reference. Dense at
  first; you grow into it.
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) - free online, and
  the single most useful book for this project.
- [learncpp.com](https://www.learncpp.com/) - the best free structured C++ tutorial.
  Phase 1 maps onto its chapters.

---

## Design documents

Two living documents you update as you go, not up front:

- [../design/game-design-doc.md](../design/game-design-doc.md) - what the game *is*.
  Story, chapters, party, battle rules.
- [../design/architecture.md](../design/architecture.md) - how the code is arranged.
  Update it whenever you add a system, so you never lose the map of your own codebase.
