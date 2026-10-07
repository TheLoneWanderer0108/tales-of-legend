# Phase 07 - Content pipeline and chapters (Days 118-150)

Your job changes today. For 117 days you were a programmer. From here you are a **content
producer** who occasionally fixes engine bugs. This is a genuinely different skill and most
hobby projects die at exactly this transition, because building systems is more immediately
satisfying than filling them.

The defence against that is this phase's first block: eight days spent making content
production *fast*. If adding an enemy takes 40 minutes, you will ship six enemies. If it
takes 4 minutes, you will ship forty.

**The rule for this phase:** if you catch yourself building a new system, stop and ask
whether the content could be authored with what already exists. The answer is usually yes.

Research notes for this phase:
[data-driven-design.md](../research/data-driven-design.md),
[jrpg-game-design.md](../research/jrpg-game-design.md),
[tilemaps.md](../research/tilemaps.md),
[rpg-math-and-balance.md](../research/rpg-math-and-balance.md).

Trimmed: Days 118, 121, 122, 123, 124, 125, 126-133, 134-141, 142-149, 150 removed.

---

## Block A - The content pipeline (Days 118-125)

### Day 119 - Validation

**Goal:** catch content mistakes at load time, not during a playthrough three hours later.

**Tasks**

- [ ] Write a validator for every data type: required fields present, types correct, numeric
      ranges sane, referenced IDs existing (items, skills, sounds, animations, maps, nodes)
- [ ] Add cross-file reference checking: every encounter's enemies exist, every enemy's drops
      exist, every shop's stock exists, every dialogue jump resolves, every quest condition
      names a flag that something writes
- [ ] Add semantic warnings, not just errors: an enemy with zero XP, an item that costs 0
      gold, a skill nothing can learn, a map with no exits, an unreachable dialogue node
- [ ] Report everything with file, line, and ID, and continue validating after the first error
      so one run tells you about all 30 problems
- [ ] Add a `--validate` command line mode that checks everything and exits with a status
      code, so you can run it before every commit
- [ ] Break a data file on purpose in five different ways and confirm the messages are clear
      enough to act on without reading the validator's source

**Deliverable:** `./build/game --validate` catches every class of content error with a clear
message.

**Journal:** how many real errors did the validator find in content you thought was fine?

---

### Day 120 - Hot reload everywhere

**Goal:** change content and see it instantly. This is the highest-leverage day of the phase.

**Tasks**

- [ ] Extend the Day 62 dialogue hot reload to every data type, on one key (F6)
- [ ] Reload safely: validate first, and keep the old data if validation fails, so a typo
      never crashes a session
- [ ] Hot reload maps: reload the current map's tiles and objects while keeping the player's
      position, so map editing becomes a 2-second loop
- [ ] Hot reload textures and sounds, so replacing art in Phase 8 needs no restart
- [ ] Optionally add file watching so a save in Tiled or a text editor reloads automatically.
      Even polling modification times every half second is enough
- [ ] Build the developer console or debug menu (F9): warp to any map, set any flag, give any
      item, set party level, start any encounter, trigger any cutscene, heal the party, toggle
      invincibility
- [ ] Time it: measure how long it takes to change an enemy's HP and see the result in a
      battle. Target under 10 seconds. Write both the before and after times in your journal

**Deliverable:** a sub-10-second content iteration loop and a developer console.

**Journal:** iteration time before and after. This number determines how much game you ship.

---

## The 8-day chapter template

Every chapter follows this. The point is that by Chapter 3 you are executing a known process
rather than inventing one, which is what makes finishing possible.

| Day | Work |
| --- | --- |
| 1 | **Plan.** Beat sheet into a concrete map list, encounter list, item and badge rewards, new mechanic, boss concept. Fill in the chapter checklist completely. No content built today. |
| 2 | **Maps.** Build every map for the chapter as greybox layouts: correct size, collision, and connections, with no decoration. Walk the whole chapter's geography end to end. |
| 3 | **Populate.** NPCs, chests, doors, switches, save points, shops, triggers. The chapter is now walkable and interactive but silent. |
| 4 | **Write.** All dialogue and cutscenes for the chapter, authored and validated. Play through reading everything. |
| 5 | **Enemies.** Author the chapter's new enemies from the tier table, place encounters, run batch balance simulation, and fix outliers. |
| 6 | **Boss.** Build the boss: phases, gimmick using the chapter's new mechanic, telegraphs, dialogue at transitions, unique action command, intro and defeat presentation. |
| 7 | **Playtest.** Play the whole chapter start to finish three times: once carefully, once rushing, once playing badly on purpose. Write down every problem without fixing anything. |
| 8 | **Fix and decorate.** Fix the playtest list in priority order. Decorate the maps (the pass that makes greybox feel like a place). Verify saves work at every point. Mark the chapter done. |

Hard rules for chapter days:

- **No new engine systems.** Write the idea on a list and move on. If a chapter genuinely
  cannot work without one, cut the chapter feature instead.
- **Do not decorate before Day 8.** Decorating a layout you later change is wasted work.
- **Never skip Day 7.** An unplayed chapter is not content, it is a hypothesis.

---

### Day 120b - Vertical slice [ADDED]

**Goal:** a small, complete, playable slice of the game, built only from systems that already
exist.

**New concepts:** none. That is the point of the day.

**Tasks**

- [ ] Write the slice's content list in [../design/game-design-doc.md](../design/game-design-doc.md)
      as a hard ceiling: two areas, a couple of enemies, one boss, one or two NPCs, and about
      10 moves
- [ ] Build the two areas in Tiled and connect them with Day 52's map transitions
- [ ] Place one or two NPCs with conversations in your Day 56 dialogue format, using Day 59
      flags and Day 60 commands for anything that changes
- [ ] Author a couple of field enemies with Day 67 behaviours, each leading to a Day 68
      `EncounterDef`
- [ ] Author about 10 moves across skills and items, all in data, each using a command that
      already exists: the Day 83 timed press on offence, Day 86 guard and superguard on
      defence, with Day 88 FP costs
- [ ] Put the boss at the end of the second area, reusing your Day 91 boss work
- [ ] Run `--validate` and fix everything it reports, then tune with hot reload and the
      developer console
- [ ] **No new engine systems.** If the slice seems to need one, write it on a list and cut
      the feature instead
- [ ] Play from the title screen to the boss's defeat, saving and loading along the way. Then
      have someone else play it while you watch in silence
- [ ] Commit: `day 120b: vertical slice complete`

**Deliverable:** a slice someone else can play from title screen to boss defeat without
instruction.

**Journal:** which part of the slice wanted a new system, and how did you do without it?

---

## Phase 07 exit checklist

- [ ] `--validate` catches broken content with clear, actionable messages
- [ ] Content iteration takes under 10 seconds, and there is a developer console
