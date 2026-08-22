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

---

## Block A - The content pipeline (Days 118-125)

### Day 118 - Content audit

**Goal:** find and eliminate everything still hardcoded.

**Tasks**

- [ ] Grep for string literals and magic numbers across the codebase. Every one is a
      candidate for a data file
- [ ] Make a table of every content type and where it currently lives: enemies, items,
      skills, badges, equipment, encounters, arenas, maps, dialogue, cutscenes, quests,
      shops, characters, growth curves, drop tables, music cues
- [ ] Move every remaining hardcoded value into `assets/data/`, with one file per type
- [ ] Standardise on a consistent shape across all files (an object keyed by ID, with a
      `version` field) so your loaders and validators stay uniform
- [ ] Remove any content-specific `if` statements from engine code. `if (enemyId == "boss1")`
      anywhere outside a data file is a design failure - express it as data instead
- [ ] Confirm you can change any balance number, rename any item, or retune any enemy without
      touching C++

**Deliverable:** zero content in code, one file per content type.

**Journal:** what was the worst hardcoded thing you found?

---

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

### Day 121 - The design document, properly

**Goal:** decide what the game actually is, now that you know what your engine can do.

**Tasks**

- [ ] Fill in [../design/game-design-doc.md](../design/game-design-doc.md) completely.
      Every section, no placeholders
- [ ] Write the one-line pitch and the three pillars first, and be strict: every piece of
      content from here must serve a pillar
- [ ] Write the full chapter outline: location, story beat, new mechanic, boss per chapter.
      Three chapters is the plan; resist expanding it
- [ ] Set the content budget as hard numbers: maps, enemies, bosses, items, skills, badges,
      dialogue scenes, music tracks, and target playtime. Write them down and treat them as a
      ceiling, not a floor
- [ ] Write the cut list explicitly, including every good idea you are choosing not to build.
      A written cut list is what lets you say no in month seven without feeling like a failure
- [ ] Sanity check the budget against your Day 120 iteration times. If the numbers do not fit
      in the remaining days, cut now rather than discovering it on Day 148
- [ ] Read the whole document out loud. Anything that sounds boring when spoken will be boring
      to play

**Deliverable:** a complete design document with a hard content budget and a cut list.

**Journal:** what did you cut today, and did it hurt?

---

### Day 122 - Writing day

**Goal:** write the story and the script structure before building the maps that carry them.

**Tasks**

- [ ] Write the full story in one page. If it does not fit, it is too complicated for a first
      game
- [ ] Write each chapter as a beat sheet: opening, complication, discovery, setback, boss,
      resolution. Six beats per chapter is a comfortable rhythm
- [ ] Write every named character's one-line identity, their speech quirk, and what they want
- [ ] Draft the actual dialogue for the opening scene, the three chapter openings, and the
      three boss confrontations. These are the scenes that carry the game
- [ ] Write the ending. Knowing where you are going makes every earlier scene easier to write
- [ ] Keep the total word count modest. 3000-5000 words of dialogue is a full small game and
      is much more than it sounds
- [ ] Read every line as if you were a player who skips text. Does the plot still work? Does
      each scene still convey what it must?

**Deliverable:** a complete beat sheet and drafted dialogue for the eight key scenes.

**Journal:** which character wrote themselves most easily? Give them more screen time.

---

### Day 123 - Map authoring workflow

**Goal:** make map building fast and consistent.

**Tasks**

- [ ] Organise your placeholder tileset properly, grouped by terrain and with clearly marked
      collision tiles. A tidy tileset makes mapping several times faster
- [ ] Set up Tiled properly: object templates for every entity type with their properties
      pre-filled, custom property types so you cannot typo a field name, and a saved
      workspace layout
- [ ] Build reusable prefab groups: a house exterior, a shop interior, a dungeon room, a save
      point, a treasure alcove. Copy-paste composition is how small teams build many maps
- [ ] Write your map conventions in the design doc: layer names, standard room dimensions,
      how doors are named, where spawns go. Consistency lets you build maps on autopilot
- [ ] Time yourself building a complete small map from scratch. Target 20-30 minutes for a
      basic room. If it is over an hour, fix the workflow before making 20 maps
- [ ] Author two throwaway maps purely to practise the workflow, then delete them

**Deliverable:** a Tiled setup, prefabs, written conventions, and a measured map build time.

**Journal:** your measured time per map, and your total map budget in hours.

---

### Day 124 - Encounter and enemy authoring

**Goal:** produce balanced enemies quickly instead of agonising over each one.

**Tasks**

- [ ] Build an enemy stat template table in a spreadsheet: for each chapter and tier
      (weak, normal, strong, boss), the expected HP, ATK, DEF, SPD, XP, and gold. Derive it
      from your Day 94 simulator targets
- [ ] Author new enemies by starting from the tier row and then adding one distinctive
      mechanic. This is how you avoid 20 enemies that are all the same enemy
- [ ] Write the encounter composition rules down: how many enemies per encounter by chapter,
      which mixes are interesting, which mixes are unfair (three flyers when the player has
      one aerial attack)
- [ ] Add a batch simulation mode: validate **every** encounter in the game against the
      simulator and report any whose win rate falls outside target. Running this after each
      chapter's content is authored keeps balance from drifting
- [ ] Author three new enemies with the workflow and time it. Target under 20 minutes each
      including a simulated balance check
- [ ] Build a "content status" table in the design doc, tracking every planned enemy, map,
      and scene as planned, authored, or tested

**Deliverable:** an enemy tier table, encounter rules, batch validation, and three enemies
authored in under an hour.

**Journal:** your per-enemy authoring time.

---

### Day 125 - The chapter template

**Goal:** turn "build a chapter" into a checklist you can execute without deciding anything.

**Tasks**

- [ ] Write out the 8-day chapter template below in your own words, adjusted to your measured
      timings from Days 123-124
- [ ] Build a per-chapter checklist file (`docs/design/chapter-01.md` and so on) from the
      template, so each chapter starts with a filled-in plan rather than a blank page
- [ ] Decide your definition of done for a chapter: authored, validated, balance-simulated,
      played start to finish twice, and no known bugs
- [ ] Prepare the Chapter 1 checklist fully, ready to start tomorrow
- [ ] Commit: `day 125: content pipeline ready`

**Deliverable:** a chapter template and a prepared Chapter 1 checklist.

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

## Block B - Chapter 1 (Days 126-133)

Chapter 1 teaches the game. It is the chapter most players judge you on and the one you will
revise the most, so build it first and revisit it in Phase 8.

### Days 126-133 - Chapter 1

Follow the 8-day template. Chapter-specific requirements:

- [ ] **Teach without a tutorial screen.** The first battle should be against one weak enemy
      that cannot kill you, so the player can experiment with the action command safely
- [ ] Introduce exactly one mechanic at a time, each with a safe practice space before it is
      tested. Order suggestion: move, talk, timed press attack, guard, item, skill, superguard
- [ ] The first 5 minutes must contain: a hook, player control, a first successful action
      command, and a clear goal. Anything else can wait
- [ ] Include a town (shop, inn, 5-6 NPCs), a small dungeon (3-5 maps), 4 enemy types, and a
      boss that examines the timed press and guard
- [ ] Recruit the first party member here, with a cutscene establishing their identity
- [ ] Target 30-40 minutes of play
- [ ] After Day 133, have someone else play Chapter 1 while you watch silently. Write down
      every confusion. Do not defend your design; just record

**Deliverable:** a complete, tested Chapter 1 that teaches the game without a tutorial wall.

**Journal:** what confused your playtester in the first five minutes?

---

## Block C - Chapter 2 (Days 134-141)

Chapter 2 deepens. The player knows the basics, so this is where the game gets interesting.

### Days 134-141 - Chapter 2

Follow the template. Chapter-specific requirements:

- [ ] Introduce the new mechanic promised in the design doc (a field ability that gates
      progression works well - it retroactively opens up Chapter 1's map, which players love)
- [ ] Introduce a second party member with a genuinely different battle role
- [ ] Escalate the enemy design: enemies requiring specific counters (flying, spiked,
      armoured) so the Day 90 mechanics finally matter
- [ ] Add a mid-chapter twist beat so the story is not a straight line
- [ ] Include a side quest with a badge reward, so exploration pays
- [ ] Introduce the mash command and the hold/release command in safe contexts first
- [ ] Target 40-50 minutes
- [ ] Run the batch balance simulation across Chapters 1 and 2 together, since the player's
      level curve now spans both
- [ ] Playtest the whole game so far (Chapters 1-2, roughly 80 minutes) in one sitting. Note
      where your attention drifts - that is where the pacing is wrong

**Deliverable:** a complete Chapter 2, and a verified 80-minute continuous playthrough.

**Journal:** where did your attention drift during the full playthrough?

---

## Block D - Chapter 3 and the ending (Days 142-150)

Nine days, because a finale needs the extra one.

### Days 142-149 - Chapter 3

Follow the template. Chapter-specific requirements:

- [ ] The chapter must **test** rather than teach: combine every mechanic the player has
      learned, with no new systems introduced
- [ ] Introduce the rotation and sequence commands as advanced options, not requirements
- [ ] Build the final dungeon with real difficulty, a mid-point save, and optional side rooms
      with strong rewards
- [ ] Build the final boss as your best work: multiple phases, a gimmick per phase, dialogue
      escalating through the fight, a desperation phase, and a unique ultimate attack with its
      own action command
- [ ] Make the final boss beatable by a player who has been ignoring optimisation. Difficulty
      should come from execution, not from a build check
- [ ] Target 50-60 minutes plus the boss

### Day 150 - The ending and phase close

**Goal:** finish the game's arc and confirm the whole thing plays through.

**Tasks**

- [ ] Write and build the ending sequence: a final scene, resolution for each party member,
      and a payoff for the pillars from your design doc
- [ ] Build a credits sequence naming yourself, every asset you used with its licence, and
      every tool. Reading your own credits after eight months is a real moment
- [ ] Add a post-credits save or a "cleared" flag if you want a New Game Plus hook, but do
      not build New Game Plus itself
- [ ] Play the **entire game** start to finish in one sitting, taking notes. This is the first
      time your game exists as a whole thing
- [ ] Run `--validate`, the batch balance simulation, and the sanitizer build across the full
      playthrough
- [ ] Update the content status table: everything should read tested
- [ ] Commit: `day 150: content complete - the game is finishable`

**Deliverable:** a complete, playable game from title screen to credits.

**Journal:** how long is the full playthrough, and how did it feel to reach the credits?

---

## Phase 07 exit checklist

- [ ] No content lives in code; every content type has a data file
- [ ] `--validate` catches broken content with clear, actionable messages
- [ ] Content iteration takes under 10 seconds, and there is a developer console
- [ ] The design document is complete, with a hard content budget and a cut list
- [ ] The story, beat sheets, and key scenes are written
- [ ] Map authoring takes 20-30 minutes per map with prefabs and conventions
- [ ] Enemies are authored from a tier table and batch balance-simulated
- [ ] Three chapters exist, each built with the same template
- [ ] Chapter 1 teaches the game with no tutorial wall
- [ ] Someone else has played Chapter 1 while I watched in silence
- [ ] The game plays from title screen to credits in one sitting
