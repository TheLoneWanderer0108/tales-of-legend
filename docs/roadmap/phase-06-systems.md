# Phase 06 - RPG systems (Days 96-117)

Twenty-two days on the systems that surround combat: items, equipment, shops, party
management, levelling, menus, settings, and saving. These are the systems players interact
with constantly and notice immediately when they are bad.

Two things make this phase different from the previous ones:

- **Menus are gameplay.** A player will open the inventory hundreds of times. Ten extra
  minutes spent on cursor feel is worth more than a new enemy type.
- **Saving is not optional and not simple.** A save system that corrupts files or silently
  loses progress destroys trust permanently. Days 113-115 exist for good reason.

Research notes for this phase:
[ui-systems.md](../research/ui-systems.md),
[save-systems.md](../research/save-systems.md),
[data-driven-design.md](../research/data-driven-design.md),
[rpg-math-and-balance.md](../research/rpg-math-and-balance.md),
[audio.md](../research/audio.md).

Trimmed: Days 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112 removed.

---

## Block A - Items, equipment, party, progression (Days 96-105)

### Day 96 - Items and inventory

**Goal:** one item model that works everywhere.

**New concepts:** item definitions vs instances, stacking, categories, key items, capacity
rules.

**Research first (20 min):** [data-driven-design.md](../research/data-driven-design.md)
sections 5-7.

**Tasks**

- [ ] Define `struct ItemDef { std::string id, name, description, icon; ItemCategory category; int price; bool stackable; int maxStack; bool consumable; TargetRule target; std::vector<Effect> effects; UseContext usableWhere; };`
      loaded from `assets/data/items.json`
- [ ] Model effects as data, not code: `{ "type": "heal_hp", "amount": 20 }`,
      `{ "type": "cure_status", "status": "poison" }`,
      `{ "type": "buff", "stat": "atk", "amount": 5, "turns": 3 }`
- [ ] Write one effect applier used by items, skills, and dialogue commands alike. Three
      separate implementations of "heal 20 HP" is how bugs are born
- [ ] Implement `class Inventory` with `Add`, `Remove`, `Count`, `Has`, and iteration by
      category. Enforce stack limits and an overall capacity
- [ ] Handle key items: not sellable, not discardable, not consumed, in their own category.
      Selling a key item is a classic soft lock, and you already promised on Day 66 to
      prevent it
- [ ] Decide where the inventory lives (the persistent session, shared by the whole party)
      and record it
- [ ] Write 20 items into the data file, including edge cases: a full-heal, a revive, an
      all-target bomb, a stat-permanent booster, and a key item

**Deliverable:** 20 data-defined items with one shared effect system.

**Journal:** why one effect applier instead of one per system?

---

### Day 97 - Using items

**Goal:** items work in the field and in battle, through the same code.

**New concepts:** context-sensitive availability, unified use paths, feedback.

**Tasks**

- [ ] Implement `bool UseItem(const ItemDef&, Combatant* user, std::vector<Combatant*> targets, UseContext)`
      shared by both contexts
- [ ] Restrict by context via the item's `usableWhere`: a battle-only smoke bomb, a
      field-only tent, and items usable in both
- [ ] In battle, using an item consumes the turn and can carry its own action command
      (Paper Mario style)
- [ ] In the field, open a target selection over the party
- [ ] Give clear feedback in both contexts: the effect number, a sound, a particle, and a
      message. Then handle refusal properly - using a potion at full HP should either be
      prevented with a reason or allowed with a warning, never silently wasted
- [ ] Handle the multi-target and revive-a-dead-ally cases correctly
- [ ] Test every one of your 20 items in both contexts and tick them off in a list

**Deliverable:** all 20 items verified working in every context they claim to support.

**Journal:** which item exposed a hole in your effect system?

---

## Block B - Menus, settings, and saving (Days 106-117)

### Day 113 - Save system design

**Goal:** design the save format before writing it, because format mistakes are permanent.

**New concepts:** serialisation, versioning, save scope, forward compatibility, atomicity.

**Research first (40 min):** [save-systems.md](../research/save-systems.md) in full.

**Tasks**

- [ ] Inventory exactly what must be saved: party (levels, XP, HP/FP, skills, equipment,
      badges), inventory, gold, flags, quest state, bestiary, current map and position,
      playtime, settings (separately), and per-map object states
- [ ] Inventory what must **not** be saved: derived stats, loaded assets, camera state,
      anything reconstructible. A save file storing derived data will eventually disagree
      with itself
- [ ] Choose the format and justify it. JSON is recommended here: human-readable saves are
      an enormous debugging advantage, and file size is irrelevant at this scale
- [ ] Add a `version` field from the very first save you ever write. Version 1 costs nothing
      now and is impossible to add retroactively
- [ ] Design the file layout on paper, with the version at the top and a metadata block
      (chapter name, location, playtime, party levels) readable **without** loading the whole
      save, so the file select screen is cheap
- [ ] Decide save points: anywhere, or only at rest points? Record the decision and the reason
- [ ] Write the schema into [save-systems.md](../research/save-systems.md) or the design doc

**Deliverable:** a written save schema with a version field and a metadata block.

**Journal:** what did you decide **not** to save, and why?

---

### Day 114 - Save and load

**Goal:** working saves across multiple slots.

**Tasks**

- [ ] Implement `bool Save(int slot)` and `bool Load(int slot)` walking your schema
- [ ] Serialise by ID, never by index: item IDs, skill IDs, map IDs. Index-based saves break
      the moment you reorder a data file, and you will reorder data files
- [ ] Implement three slots plus an autosave slot
- [ ] Build the file select UI reading only the metadata block: slot number, chapter,
      location, party levels, playtime, and an empty-slot state
- [ ] Restore correctly on load: rebuild the map, place the player, restore object states,
      rebuild derived stats from scratch, resume the right music
- [ ] Test the full round trip rigorously: save, quit the process entirely, relaunch, load,
      and verify every single value. Then do it mid-dungeon with a half-finished quest
- [ ] Wire up the Day 104 "game over returns to the last save" flow
- [ ] Add autosave on map transitions and after battles, with a small non-intrusive indicator

**Deliverable:** three slots plus autosave, verified across a full process restart.

**Stuck?** If something is wrong after loading, the cause is usually a value you forgot to
save that happened to be correct in memory during testing. Test only after a real restart.

**Journal:** what did you forget to save the first time?

---

### Day 115 - Save robustness

**Goal:** never lose a player's progress. This is a trust issue, not a feature.

**New concepts:** atomic writes, corruption detection, migration, backups.

**Research first (20 min):** [save-systems.md](../research/save-systems.md) sections 7-10.

**Tasks**

- [ ] Write atomically: write to `save1.tmp`, flush, then rename over `save1.json`. A rename
      is atomic on every platform you care about, so a crash mid-save cannot corrupt an
      existing file
- [ ] Keep a backup of the previous save (`save1.bak`) and fall back to it if the main file
      fails to parse
- [ ] Detect corruption: validate the version, validate required fields, and catch parse
      errors. Report a clear message and refuse to load rather than loading a half-state
- [ ] Test corruption on purpose: truncate a save file, edit it to invalid JSON, delete a
      required field, set a future version number, and set HP above max. Handle all five
      gracefully
- [ ] Write a migration function skeleton: `MigrateSave(json&, int fromVersion)` with a
      chain of steps. You will need it the first time you change the schema mid-development,
      which will happen in Phase 7
- [ ] Handle unknown item and skill IDs on load (from data you have since renamed): drop them
      with a logged warning instead of crashing
- [ ] Handle a full disk and a read-only directory without losing the in-memory game state
- [ ] Add a debug key that dumps the current session as JSON to the console, for comparing
      against what was saved

**Deliverable:** a save system that survives all five corruption tests and can migrate
versions.

**Journal:** which corruption case did you not anticipate?

---

### Day 116 - Title screen and boot flow

**Goal:** a proper front end - the first thing anyone sees.

**Tasks**

- [ ] Build the title screen: a logo, an animated background, and a menu of New Game,
      Continue (greyed out with no saves), Settings, and Quit
- [ ] Add title music and a confirm sting
- [ ] Implement New Game: initialise a fresh session, then run the Day 63 opening cutscene
- [ ] Add a name entry screen if the hero is named by the player, keeping it short and
      gamepad-friendly
- [ ] Implement Continue jumping to the most recent save, and a Load option for the file
      select
- [ ] Ensure a clean boot order: load config, create the window in the right mode, load
      assets with a simple loading indicator, then show the title
- [ ] Test the full boot path from a completely clean state (no config, no saves) to confirm
      first-run behaviour is correct

**Deliverable:** a complete front end that works on a machine that has never run the game.

**Journal:** what broke on the clean first run?

---

### Day 117 - Phase consolidation

**Goal:** confirm the whole game structure is sound before content production begins.

**Tasks**

- [ ] Full playthrough: boot, new game, cutscene, town, shop, dialogue, dungeon, battles,
      boss, level up, equip, save, quit, relaunch, load, continue. Fix everything that breaks
- [ ] Verify every menu with keyboard only, then with gamepad only
- [ ] Run the sanitizer build for a full session
- [ ] Check the save file by hand in a text editor and confirm you can read and understand it
- [ ] Update the architecture doc with the full system inventory. You now have roughly 20
      systems; the doc is what keeps them navigable
- [ ] Do an honest code health pass: delete dead code, fix long functions, rename anything
      you hesitate to read out loud
- [ ] **Write down every system that is now finished**, because from Day 118 you shift from
      engineering to content production, and knowing what is done is what stops you from
      rebuilding it
- [ ] Commit: `day 117: all systems complete - content production next`

**Deliverable:** a feature-complete game engine with no content in it yet.

---

## Phase 06 exit checklist

- [ ] One effect system serves items, skills, and dialogue commands
- [ ] All items work in every context they claim to support
- [ ] Saves are versioned, atomic, backed up, and survive all five corruption tests
- [ ] The game boots correctly on a machine with no config and no saves
