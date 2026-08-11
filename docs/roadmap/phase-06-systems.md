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

### Day 98 - Equipment

**Goal:** gear that changes how a character plays, not just their numbers.

**New concepts:** equipment slots, stat aggregation, swap semantics, set effects.

**Tasks**

- [ ] Define slots: `Weapon`, `Armour`, `Accessory` (plus badges from Day 88, which are a
      separate capacity-based system - keep them distinct)
- [ ] Extend the accessor layer from Day 72 so `Atk(combatant)` sums base, equipment,
      badges, and status modifiers, in a documented order. Every formula must read through
      the accessors
- [ ] Add non-stat properties: an element on a weapon, a status resistance on armour, an
      extra action command tier on a legendary weapon
- [ ] Handle swapping: unequip returns the old item to the inventory; equipping something
      already worn by another character moves it; restrict equipment by character class
- [ ] Handle the max-HP edge case: if armour granting +20 max HP is removed while HP is
      above the new maximum, clamp it. Cases like this are why one accessor layer matters
- [ ] Write 12 pieces of equipment with a clear progression, and two with interesting
      trade-offs (higher attack but lower speed, and one that widens action command windows
      at the cost of damage)
- [ ] Show a stat comparison preview when browsing gear (green up arrows, red down arrows).
      Players expect this and its absence is felt

**Deliverable:** 12 equipment pieces with correct stat aggregation and a comparison preview.

**Journal:** the aggregation order, written down where a future formula bug can be checked
against it.

---

### Day 99 - Money and shops

**Goal:** an economy loop: fight, earn, spend, get stronger.

**New concepts:** shop inventories, price curves, sell values, stock limits, flag-gated stock.

**Tasks**

- [ ] Add currency to the session, with gold rewards from battles and chests
- [ ] Define shops in data: an item list with optional stock counts, a price multiplier, and
      flag conditions so a shop's stock can change after story events
- [ ] Build the shop UI as a pushed state: Buy and Sell modes, a scrolling list with icons,
      prices, descriptions, quantity selection with left/right, a running total, and a
      confirmation step
- [ ] Set sell value at 50% of the price, and refuse key items
- [ ] Handle every failure clearly: not enough gold, inventory full, stock exhausted
- [ ] Balance the economy against your Day 94 simulator output: a normal battle should pay
      for roughly one potion, and a full gear upgrade should cost several hours of casual
      play. Write the target numbers in the design doc
- [ ] Add a shopkeeper conversation using the dialogue system so the shop has personality
      rather than being a raw menu

**Deliverable:** a working shop with a balanced economy and no exploitable edge cases.

**Journal:** how many battles does a mid-tier weapon cost? Is that the right answer?

---

### Day 100 - Party management

**Goal:** multiple characters, each mechanically distinct.

**New concepts:** roster vs active party, swapping, per-character identity, partner
mechanics.

**Tasks**

- [ ] Model the roster: every recruited character, with an active subset used in battle
- [ ] Give each character a mechanical identity, not just different numbers: one with the
      highest damage and the hardest action command, one with healing and support, one with
      field utility. Write each character's one-line identity in the design doc
- [ ] Implement recruiting a party member through a story flag and a cutscene
- [ ] Build the party menu: view the roster, swap active members, reorder the formation
- [ ] Decide the Paper Mario question and record it: does the game use a single hero plus one
      swappable partner (allowing mid-battle swapping as a tactical mechanic), or a full
      party of three? The partner model is more distinctive and less work to balance
- [ ] If you chose partners, implement mid-battle swapping with a turn cost
- [ ] Handle KO'd characters: excluded from battle, revivable, and never allowing an empty
      active party
- [ ] Give inactive members a share of XP (usually reduced) so the player is not punished for
      experimenting

**Deliverable:** three characters with distinct identities, swappable in and out.

**Journal:** your party model decision and the reason.

---

### Day 101 - Levelling and stat growth

**Goal:** progression that feels rewarding and stays balanced.

**New concepts:** XP curves, growth curves, per-character growth profiles, level caps.

**Research first (25 min):** [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)
sections 11-13.

**Tasks**

- [ ] Define the XP curve in data. Compare a few shapes in a spreadsheet before choosing:
      linear, quadratic (`base * level^1.5`), and stepped. Plot the cumulative XP needed to
      reach level 20
- [ ] Define per-character growth profiles in data (per-level increments, or an explicit
      table per level). Explicit tables are more work but give total control - and for a
      20-level game they are entirely reasonable
- [ ] Award XP from enemy defs, scaled by relative level so grinding low-level enemies stops
      paying
- [ ] Implement the level-up sequence: a fanfare, stat gains shown as old to new, any newly
      learned skill announced, and multiple level-ups handled one at a time
- [ ] Decide whether levelling grants a player choice (Paper Mario's HP/FP/badge-capacity
      choice is a great model) or is automatic. A choice is more interesting; record your
      decision
- [ ] Set a level cap and design encounters against expected levels, writing an expected
      level per chapter in the design doc
- [ ] Verify against the simulator: a player 2 levels under should find a fight hard but
      winnable; 2 levels over should find it easy but not trivial

**Deliverable:** a plotted XP curve, growth tables, and a satisfying level-up sequence.

**Journal:** the curve you chose and what it means for pacing.

---

### Day 102 - Skills and learning

**Goal:** new abilities that arrive at the right pace.

**New concepts:** skill definitions, unlock conditions, learning sources, a progression tree.

**Tasks**

- [ ] Define skills fully in data: name, description, FP cost, power, element, target rule,
      action command type and difficulty, animation sequence ID, effects
- [ ] Implement learning by three routes: automatically at a level, from an item or trainer,
      and from a story event. Multiple routes keep pacing flexible
- [ ] Build a small skill tree or path per character (6-8 skills each), authored in data with
      prerequisites
- [ ] Add a skill point currency if your tree needs choices, or keep it linear if not - do not
      build a tree because trees are expected
- [ ] Ensure every skill is mechanically distinct. If two skills differ only in a number,
      cut one. Fewer, more distinct skills is better design and less balancing work
- [ ] Build the skill menu: list, description, FP cost, action command preview, and a
      practice mode that runs the action command in isolation (your Day 82 harness, exposed
      to players - a genuinely great feature)
- [ ] Verify pacing: the player should learn something new every 20-30 minutes of play

**Deliverable:** 6-8 distinct skills per character with a documented learning schedule.

**Journal:** which skill did you cut for being a duplicate?

---

### Day 103 - Badges and builds

**Goal:** finish the customisation layer you started on Day 88.

**Tasks**

- [ ] Build the badge menu: equipped list, capacity bar, available list, descriptions, and
      warnings for conflicts
- [ ] Add badge capacity as a level-up reward or a collectible upgrade
- [ ] Expand to 20 badges across categories: stat boosts, action command modifiers, on-hit
      effects, economy effects (more gold, more drops), and trade-offs
- [ ] Add three "build-defining" badges strong enough to organise a whole loadout around
- [ ] Add badges as findable rewards: hidden chests, side quests, boss drops, shop stock
- [ ] Simulate three builds with the Day 94 harness and confirm they are all viable
- [ ] Watch for the dominant-badge trap: if one badge is always equipped, either it is too
      strong or the alternatives are too weak. Both are fixable numbers

**Deliverable:** 20 badges where the best loadout depends on playstyle.

**Journal:** the badge you had to nerf, and by how much.

---

### Day 104 - Rest, healing, and checkpoints

**Goal:** the pacing valve between dungeons.

**Tasks**

- [ ] Implement rest points: an inn (costing gold) and free save/heal points in dungeons
- [ ] Restore HP and FP, cure statuses, and add a rest animation or fade with a day/night
      touch if you want one
- [ ] Decide FP recovery rules outside rest points - this single number controls how tense
      dungeon exploration feels. Record it and tune it later during playtesting
- [ ] Place healing items and save points deliberately: a dungeon should have exactly one
      mid-point relief, so attrition matters without being punishing
- [ ] Add a "game over returns you to the last rest point" rule, which needs the save system
      from Day 114 - stub it now, wire it then
- [ ] Verify pacing with the simulator: can a player clear the dungeon with the FP available?
      Can they clear it comfortably if they play well?

**Deliverable:** rest points that make dungeon resource management meaningful.

**Journal:** how tense is your dungeon now, on a scale you define?

---

### Day 105 - Systems integration

**Goal:** all the RPG systems working together, tested against each other.

**Tasks**

- [ ] Full loop test: fight, earn gold and XP, level up, buy equipment, equip it, change
      badges, rest, fight harder enemies. Confirm every step's numbers land where you expect
- [ ] Hunt cross-system bugs specifically: equipment stats surviving a battle, badges applying
      in battle, inventory shared correctly across the party, XP applied to the right
      characters, gold never going negative
- [ ] Audit for soft locks again with the new systems: sold something essential, spent all
      gold with no way to earn more, equipped a badge that makes a fight unwinnable
- [ ] Run the sanitizer build
- [ ] Update the architecture and design docs
- [ ] Commit: `day 105: rpg systems integrated`

**Deliverable:** the full progression loop, verified end to end.

---

## Block B - Menus, settings, and saving (Days 106-117)

### Day 106 - The UI framework

**Goal:** stop writing every menu from scratch.

**New concepts:** retained vs immediate mode UI, widgets, layout, focus, navigation graphs.

**Research first (35 min):** [ui-systems.md](../research/ui-systems.md) sections 9-12.

**Tasks**

- [ ] Take stock: you have built the dialogue box, the battle menu, the shop, the badge
      screen, and the party screen. List what they share. That list is your framework's
      specification - derived from real use, exactly as it should be
- [ ] Build the smallest framework covering it: `Panel` (a bordered box with a title),
      `ListView` (scrolling, selection, per-item drawing callback), `Label`, `Bar`,
      `IconGrid`, and `Tabs`
- [ ] Implement focus and navigation once, properly: directional movement, wrapping,
      confirm/cancel, and a focus stack for nested panels
- [ ] Implement scrolling correctly: keep the selection in view, show scroll indicators, and
      handle a page up/down. Nearly every homemade menu gets scroll edge cases wrong -
      test with 1, 2, exactly-one-page, and 100 items
- [ ] Add consistent animation: panels slide in, cursors ease, selections highlight. Consistency
      is what makes a UI feel professional
- [ ] Refactor **two** existing menus onto the framework, not all of them. If it does not
      simplify them, the framework is wrong and it is cheaper to learn that now
- [ ] Keep it small: under about 600 lines total. A UI framework is a game inside your game
      if you let it be

**Deliverable:** a small framework, proven by refactoring two existing menus onto it.

**Journal:** did the refactor make those menus shorter? If not, what would you change?

---

### Day 107 - The pause menu and status screen

**Goal:** the hub the player reaches from anywhere.

**Tasks**

- [ ] Build the pause menu as a pushed state with tabs: `Status`, `Items`, `Equip`,
      `Badges`, `Journal`, `System`
- [ ] Make it open instantly (no long animation) and close to exactly where the player was.
      Anything slower than about 0.15s feels sluggish when opened 300 times
- [ ] Build the status screen: portrait, level, HP/FP, all stats with their equipment
      contributions broken out, XP to next level as a bar, current statuses, and equipped
      gear. Show derived values, not just base ones
- [ ] Add character switching within the screen (left/right shoulder buttons)
- [ ] Show playtime, gold, current location, and step count if you want the classic touch
- [ ] Confirm the field is visible but frozen underneath, and that battle is fully paused if
      opened mid-battle (or forbid it there - decide and be consistent)

**Deliverable:** a pause hub that opens instantly and answers every question about a
character.

**Journal:** how fast does it open? Measure it.

---

### Day 108 - The inventory menu

**Goal:** the menu the player uses most.

**Tasks**

- [ ] Category tabs: Consumables, Equipment, Badges, Key Items, with counts per tab
- [ ] A scrolling list with icons, names, counts, and a description panel
- [ ] Sorting: by category, name, and most recently obtained. Add a manual sort mode if you
      enjoy the idea, but automatic sorting covers 95% of the need
- [ ] Use an item from the menu, with party target selection where applicable
- [ ] Discard with a confirmation, blocked for key items
- [ ] Handle an empty inventory and an empty category gracefully ("Nothing here yet")
- [ ] Show capacity, and handle the full-inventory case at pickup time with a clear message
      rather than a silent loss
- [ ] Test with 1 item and with 200 items, checking scroll behaviour at both extremes

**Deliverable:** an inventory that is fast and pleasant with any number of items.

**Journal:** what did testing with 200 items reveal?

---

### Day 109 - Equipment and badge menus

**Goal:** customisation that is easy to understand and quick to change.

**Tasks**

- [ ] Equipment screen: character portrait, slots on the left, available items for the
      selected slot on the right, and a live stat comparison for the highlighted item
- [ ] Show the comparison clearly: current value, arrow, new value, coloured by direction,
      including derived stats the change affects
- [ ] Add an "optimise" button that equips the best available gear by a simple heuristic. It
      is 20 lines and players love it
- [ ] Badge screen: the capacity bar, equipped grid, available list, and warnings when a
      badge conflicts with or is redundant against another
- [ ] Add loadout presets (save and load up to three badge sets), which turns
      experimentation from a chore into a feature
- [ ] Handle the shared-equipment case: two characters cannot wear the same unique item, and
      the UI must show who has it
- [ ] Test swapping gear 50 times in a row and confirm no stat drift accumulates. Stat drift
      from repeated equip/unequip is a real and nasty bug class

**Deliverable:** equipment and badge screens with previews, presets, and no stat drift.

**Journal:** how did you verify no stat drift?

---

### Day 110 - Journal and bestiary

**Goal:** reward the player's curiosity and give them a way to catch up on the story.

**Tasks**

- [ ] Upgrade the Day 66 journal: active quests with current steps, completed quests, and a
      story recap of major beats so a player returning after two weeks knows what to do
- [ ] Build a bestiary populated by your Day 90 scan action: stats, weaknesses, drops,
      description, and a defeat counter
- [ ] Add a collection completion percentage. It is a cheap, effective hook for exploration
- [ ] Add an items-discovered log if you want a second collectible axis
- [ ] Write bestiary descriptions with personality - the tattle text is a signature Paper
      Mario pleasure, and it is pure writing with no engineering cost
- [ ] Verify bestiary data persists in the save file

**Deliverable:** a journal that catches a returning player up, and a bestiary worth filling.

**Journal:** did writing the bestiary text teach you something about your enemies' designs?

---

### Day 111 - The settings menu

**Goal:** respect the player's setup and needs.

**Tasks**

- [ ] Audio: master, music, and SFX sliders, applied live so the player hears the change as
      they drag
- [ ] Video: window mode (windowed, borderless, fullscreen), resolution scale, integer
      scaling toggle, and a VSync/frame cap option. raylib 6.0 redesigned fullscreen and DPI
      handling, so read the current docs
- [ ] Gameplay: text speed, battle speed, skip battle animations, damage number toggle
- [ ] Accessibility: action command window width (100% / 150% / 200%), auto-succeed at `Ok`,
      screen shake toggle, flash reduction, colour-blind-safe status indicators (shape plus
      colour), and a larger font option
- [ ] Persist everything to a config file, and load it before the window is created so the
      window opens in the right mode
- [ ] Add a reset-to-defaults option with a confirmation
- [ ] Test with every setting at its extreme and confirm the game remains playable

**Deliverable:** a settings menu that persists and is applied at startup.

**Journal:** which accessibility option turned out to also be your favourite to play with?

---

### Day 112 - Key rebinding

**Goal:** finish what you started on Day 41.

**Tasks**

- [ ] Build a rebinding UI listing every action with its current keyboard and gamepad bindings
- [ ] Capture the next input on selection and assign it, with Escape to cancel
- [ ] Detect and resolve conflicts: warn, and either swap or refuse. Do not allow a state
      where an action is unbound and the menu cannot be exited
- [ ] Protect essential bindings (confirm and cancel must always exist), with a safe fallback
      binding
- [ ] Show glyphs and prompts using the actual bound key everywhere in the game. A tutorial
      that says "Press A" after rebinding is worse than no tutorial - this is why the Day 41
      input layer exists
- [ ] Persist bindings to the config file and reload them
- [ ] Test with a deliberately absurd binding set and confirm the game is still completable

**Deliverable:** full rebinding, with in-game prompts that reflect the player's bindings.

**Journal:** how many places needed changing to show the correct glyph? (If it was more than
one, consolidate.)

---

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
- [ ] Stat aggregation has one documented order and no drift over repeated equipping
- [ ] The economy is tuned against simulator numbers, not guesses
- [ ] Each party member has a distinct mechanical identity
- [ ] XP and growth curves are plotted, not invented
- [ ] Every skill is mechanically distinct
- [ ] 20 badges with no single dominant choice
- [ ] A small UI framework proven by refactoring real menus onto it
- [ ] The pause menu opens in under 0.15s
- [ ] Menus handle 0, 1, one page, and 200 items correctly
- [ ] Settings and key bindings persist and apply at startup
- [ ] Prompts show the player's actual bindings everywhere
- [ ] Saves are versioned, atomic, backed up, and survive all five corruption tests
- [ ] The game boots correctly on a machine with no config and no saves
