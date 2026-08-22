# Ideas for the next game

Every good idea that does not belong in Tales of Legend goes here. This file is not a to-do list -
it is a pressure valve. Writing an idea down here lets you stop thinking about it without losing it.

Use it from Day 1. You will have ideas for systems, mechanics, and stories constantly, and the
discipline of parking them is what keeps the roadmap achievable. During Phase 8 (Days 151-170) this
file is the *only* legal destination for a new feature idea.

Format: one line, plus a sentence on why it did not fit this time.

---

## Mechanics

-

## Systems and engineering

-

## Story and content

-

## Things I would do differently

Fill this in as you go, not at the end. The lessons are sharpest the day they happen.

-

## Rewrite candidates

Systems in Tales of Legend I would build differently now. After shipping, pick the one I am least
proud of and rewrite it over a weekend as an exercise (see the end of
[roadmap/phase-08-ship.md](roadmap/phase-08-ship.md)).

-

---

## If the next game is 3D

Recorded during Phase 0 so the decision is not re-litigated mid-project. Short version: going 3D
*after* this roadmap is a good idea, and going 3D *instead of* it is not. Tales of Legend stays 2D.

Note also that Paper Mario itself is 2.5D - flat 2D characters standing in 3D rooms - so the look of
the reference game does not actually require full 3D.

### What transfers unchanged

Roughly 60% of what this roadmap teaches is dimension-agnostic:

- **C++ and tooling.** Memory and ownership, CMake, git, gdb, sanitizers. A third axis changes none
  of it.
- **Engine architecture.** Game loop, fixed timestep, state stack, asset manager, input layer, event
  bus, entity handling, timers and tweens. The designs survive; only the drawing calls underneath
  change.
- **Game systems.** Battle engine, action commands, dialogue, quests, inventory, equipment, save and
  load, menus, balance. Identical in 3D.
- **Content pipeline.** Data-driven design, validation, hot reload, authoring discipline.
- **Shipping.** Packaging, storefront, QA, playtesting, trailer. Identical - and by then done twice,
  counting the interlude.
- **The meta-skill.** Scoping, finishing, and knowing my own real velocity.

### What is genuinely new

- **3D maths.** Matrices, transform hierarchies, the model-view-projection chain, quaternions for
  rotation. Two to three weeks of focused study to become functional; longer to stop being confused.
  This is the biggest new chunk and it is unavoidable.
- **Rendering concepts.** Depth buffer, lighting, materials, shadows, frustum culling, transparency
  ordering. The shader day in Phase 8 is a toehold, not a foundation.
- **Collision and physics.** Mesh collision and a character controller, realistically leaning on a
  physics library (Jolt or Bullet) rather than hand-rolling it. Character controllers in 3D are
  notoriously fiddly.
- **Camera.** Orbit and follow behaviour, plus handling geometry that blocks the view.

Debugging is also harder in a way worth expecting: "why is this in the wrong place" is usually
matrix multiplication order or a parent transform, and it is much less visible than a 2D arithmetic
error.

Estimate: a 3D game of similar scope is three to four months of new learning stacked on top of what
this roadmap leaves me with - against well over a year starting cold.

### The actual bottleneck: art

This roadmap teaches nothing about Blender, and modelling, UV mapping, rigging, skinning, and
animating remain the single largest cost in solo 3D. It is the reason most solo 3D projects stall.
Two ways to handle it, both legitimate:

- Learn Blender as a **separate low-intensity track**, either alongside Phases 7-8 (more authoring
  than hard engineering, so there is spare attention) or immediately after shipping. Do not try to
  learn it mid-engineering.
- Use **bought or free asset packs** for the first 3D project and keep the focus on code.

3D does claw some cost back: one model covers every viewing angle, so there is no drawing four
directions of every animation, and pixel-snapping shimmer, y-sorting, and sprite sheet management
disappear entirely.

### One thing to do *during* this project to make the switch cheap

Keep raylib drawing calls confined to a thin rendering layer instead of sprinkling them through the
game systems. If the battle system never mentions `DrawTextureRec`, it ports to 3D nearly unchanged.
Establish this in Phase 3 and hold the line through Phases 4-6; it is a good habit regardless of
whether the next game is 3D.

### Middle paths worth considering

- **2.5D.** 2D sprites billboarded in a 3D scene, the authentic Paper Mario approach. Much cheaper
  than full 3D, but still adds camera and coordinate complexity.
- **An existing engine.** After finishing this roadmap, reaching for Godot or Unreal for a 3D
  project is not a shortcut around learning - the C++ and architecture knowledge is already banked,
  and an engine solves exactly the rendering and asset machinery that would otherwise eat the whole
  budget.

### The rule

Learn 3D maths and Blender *or* learn architecture and how to finish - not both at once. Attempting
both simultaneously is the classic way to learn neither.
