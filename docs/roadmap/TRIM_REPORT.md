# Roadmap trim report

Branch: `trim-roadmap`. Only `phase-04` to `phase-07` were edited, plus this file. All line
numbers refer to the trimmed files on this branch.

## Day counts

| Part | Original | Trimmed |
| --- | --- | --- |
| Phase 00-03 | 53 | 53 |
| Interlude (36a-36f) | 6 | 6 |
| Phase 04 | 18 | 12 (11 kept + Day 69b) |
| Phase 05 | 24 | 18 |
| Phase 06 | 22 | 7 |
| Phase 07 | 33 | 3 (2 kept + Day 120b) |
| **Through Phase 07** | **156** | **99** |
| Phase 08 (untouched, see below) | 20 | 20 |

## Scope questions for you

1. **Phase 00 and Phase 08 were not in the brief.** The repo has `phase-00-setup.md`
   (Days 1-3) and `phase-08-ship.md` (Days 151-170). I left both unchanged. Phase 08 assumes
   three chapters, real art for all of them, and many cut systems; its conflicts are listed
   in the last section.
2. **`README.md` was not edited.** It still says "A 170-day, day-by-day plan", and its phase
   table and progress tracker list the original ranges and cut blocks (e.g. "Chapter 1 |
   126-133", "Menus and save | 106-117", "NPCs and dialogue | 54-63").
3. **A second copy exists** at `~/Projects/sandbox/research/roadmap/` (not a git repo, Phase 01
   boxes unchecked). I ignored it.

## Structural decisions I made (easy to revert)

- **Phase 07 Blocks B, C and D were removed together with their days.** Their headings and
  intro paragraphs ("Chapter 1 teaches the game...", "Chapter 2 deepens...", "Nine days,
  because a finale needs the extra one.") only introduced cut days. Leaving them would have
  put the Vertical slice day under "Block D - Chapter 3 and the ending".
- **"The 8-day chapter template" section in Phase 07 was kept**, because it is not a day.
  It now sits between Day 120 and Day 120b. See the Phase 07 conflicts below.
- **Block headings that lost some days were kept as is**, with their original ranges.
- **Day 88:** only the FP bullet was kept. The six deleted bullets were the badges, the
  three action-command badges, star power, the badge equip UI, "Balance-test three
  different builds" (the builds are badge loadouts), and "Record every badge". The heading,
  goal, new concepts, deliverable and journal are unchanged.
- **Exit checklist items that mix kept and cut days were kept** (three of them, listed below).
- **The "Trimmed:" line** is at the end of each phase intro, after the research notes list
  and before the first `---`.
- **The vertical slice day is numbered "Day 120b"**, following the "69b" pattern after the
  last surviving Phase 07 day.

## Exit checklist items deleted

- Phase 04: conversation in under five minutes (Day 62), validator and F6 hot reload
  (Day 62), cutscenes (Day 63), triggers (Day 64), chests/doors/switches/blocks (Day 65),
  quest journal (Day 66).
- Phase 05: the simulator item (Day 94).
- Phase 06: stat aggregation (98/109), economy (99), party identity (100), XP curves (101),
  distinct skills (102), 20 badges (103), UI framework (106), pause menu (107), menus with
  0/1/200 items (106/108), settings and bindings (111/112), binding prompts (112).
- Phase 07: everything except `--validate` (Day 119) and iteration time / developer
  console (Day 120).

---

## Conflicts in kept days

### phase-03-engine.md (unchanged phase)

- **Day 41**, lines 182-183: "Load bindings from a config file so rebinding is already
  possible before you build / the settings menu in Phase 6"
  → Days 111-112 (settings menu, key rebinding), cut.
- **Day 51**, line 534: "Add `PanTo(target, duration, easing)` using Day 50's tweens, for
  cutscenes later"
  → Day 63 (cutscene system), cut.

### phase-04-overworld.md

- **Day 56**, line 100: "Write, on paper, the five dialogue scenes you know you need: a
  greeting, a shopkeeper,"
  → Day 99 (money and shops), cut.
- **Day 59**, lines 210-211: "`ch1_met_elder`, `ch1_tower_opened`, `item_key_taken_house3`.
  Chaos here becomes / unfixable by chapter 3"
  → chapters (Days 126-150), cut.
- **Day 68**, line 300: "a second (this is also your Paper Mario field hammer/jump move)"
  → Day 70 (field abilities), cut.
- **Day 69**, line 325: "Define `struct BattleResult { Outcome outcome; int xpGained; int
  goldGained; ...`"
  → `goldGained` points to Day 99 (money), cut.
- **Day 69**, lines 328-329: "On defeat: fade to a game over screen with Continue (reload
  last save - stub until / Phase 6) and Title options"
  → Day 69b now replaces this stub, so the "stub until Phase 6" note is outdated. The Phase 6
  save days (113-114) still exist.
- **Day 69**, line 335: "Apply XP and level-ups from the result (a stub is fine; Phase 6 owns
  progression)"
  → Day 101 (levelling and stat growth), cut. Nothing in Phase 6 owns progression now.
- **Day 71**, lines 392-394: "Assemble: an opening cutscene, a town with four talking NPCs
  and a shopkeeper stub, / a house, a puzzle room, a gated dungeon entrance, three field
  enemies, one main quest / with three steps"
  → opening cutscene: Day 63; town conversations: Day 62; shopkeeper: Day 99; house and
  puzzle room: Day 65; gated entrance and main quest: Day 66. All cut.

### phase-05-battle.md

- **Day 72**, lines 45-46: "Persistent data (`CharacterData`: name, level, XP, / learned
  skills, equipment, current HP/FP)"
  → level/XP: Day 101; learned skills: Day 102; equipment: Day 98. All cut.
- **Day 77**, line 221: "3. attacker buffs and badges"
  → badges: Day 88's badge bullets (deleted) and Day 103, cut.
- **Day 81**, line 350: "**New concepts:** outcome handling, reward calculation, level-up
  flow, escape rules."
  → Day 101, cut.
- **Day 81**, line 354: "Victory: a victory pose and jingle, then a results panel listing XP,
  gold, and item"
  → gold: Day 99, cut.
- **Day 81**, line 356: "Apply XP to persistent `CharacterData`, handle multiple level-ups in
  one go, and show"
  → Day 101, cut.
- **Day 82**, line 404: "`int   hits = 1;            // for mash/multi commands`"
  → Day 84 (mash command), cut.
- **Day 82**, line 429: "Write down the accessibility plan now: an option to widen all
  windows, an option to"
  **Day 87**, line 529: "Implement the accessibility options you promised on Day 82 (wider
  windows,"
  → the player-facing toggles for these were in Day 111 (settings menu), cut. Day 87 still
  builds the options, but nothing exposes them to the player.
- **Day 88**, line 540: "### Day 88 - The economy: FP, badges, and star power"
  → badges and star power bullets deleted from this day.
- **Day 88**, line 544: "**New concepts:** resource economies, build customisation, cost
  curves, meaningful choice."
  → build customisation was the badge system, deleted.
- **Day 88**, lines 548-549: "Implement FP (skill points): skills cost FP, FP restores
  partially after battle and / fully at rest points."
  → rest points: Day 104, cut.
- **Day 88**, line 552: "**Deliverable:** an economy where the player's build and their
  execution both matter."
  → builds: badge bullets, deleted.
- **Day 88**, line 554: "**Journal:** which badge combination broke the game, and how did
  you fix it?"
  → badges, deleted.
- **Day 91**, line 601: "Give it a unique, harder action command for its ultimate attack"
  → with Days 84-85 cut, the only offensive command is Day 83's timed press. This line now
  implies building a new command type.
- **Day 95**, line 634: "**Deliverable:** a complete, polished, balanced battle system
  integrated with the overworld."
  → polish: Days 92-93; balance: Day 94. All cut.

### phase-06-systems.md

- **Day 96**, line 39: "Define `struct ItemDef { ... int price; ...`"
  → price: Day 99 (shops), cut.
- **Day 96**, lines 48-50: "Handle key items: not sellable, not discardable, not consumed,
  in their own category. / Selling a key item is a classic soft lock, and you already
  promised on Day 66 to / prevent it"
  → Day 66 (quests and soft locks), Day 99 (selling), Day 108 (discarding). All cut.
- **Day 97**, line 76: "In the field, open a target selection over the party"
  → no menu opens an item in the field: Day 107 (pause menu) and Day 108 (inventory menu)
  are cut.
- **Day 97**, line 77: "Give clear feedback in both contexts: the effect number, a sound, a
  particle, and a"
  → particle system: Day 93, cut.
- **Day 113**, line 93: "**Goal:** design the save format before writing it, because format
  mistakes are permanent."
  **Day 113**, line 109: "Add a `version` field from the very first save you ever write."
  → Day 69b (added) now writes the first save, with a version field.
- **Day 113**, lines 101-103: "party (levels, XP, HP/FP, skills, equipment, / badges),
  inventory, gold, flags, quest state, bestiary, current map and position, / playtime,
  settings (separately), and per-map object states"
  → levels/XP: 101; skills: 102; equipment: 98; badges: 88/103; gold: 99; quest state: 66;
  bestiary: 110; settings: 111; object states: 65. All cut.
- **Day 113**, line 112: "(chapter name, location, playtime, party levels)"
  → chapters: 126-150; levels: 101. Cut.
- **Day 113**, line 114: "Decide save points: anywhere, or only at rest points?"
  → rest points: Day 104, cut.
- **Day 114**, lines 133-134: "file select UI reading only the metadata block: slot number,
  chapter, / location, party levels"
  → chapters and Day 101, cut.
- **Day 114**, line 135: "Restore correctly on load: rebuild the map, place the player,
  restore object states,"
  → object states: Day 65, cut.
- **Day 114**, line 138: "Then do it mid-dungeon with a half-finished quest"
  → Day 66, cut.
- **Day 114**, line 139: "Wire up the Day 104 \"game over returns to the last save\" flow"
  → Day 104, cut. Day 69b already wires Continue to the save.
- **Day 115**, lines 172-173: "You will need it the first time you change the schema
  mid-development, / which will happen in Phase 7"
  → Phase 07 content days, cut.
- **Day 116**, lines 193-194: "a menu of New Game, / Continue (greyed out with no saves),
  Settings, and Quit"
  → Settings: Day 111, cut.
- **Day 116**, line 196: "Implement New Game: initialise a fresh session, then run the Day 63
  opening cutscene"
  → Day 63, cut.
- **Day 116**, lines 201 and 203: "load config, create the window in the right mode" / "(no
  config, no saves)"
  → settings persistence: Day 111, cut. A bindings config file still exists from Day 41.
- **Day 117**, lines 218-219: "Full playthrough: boot, new game, cutscene, town, shop,
  dialogue, dungeon, battles, / boss, level up, equip, save, quit"
  → cutscene: 63; shop: 99; level up: 101; equip: 98. All cut.
- **Day 117**, line 223: "You now have roughly 20 / systems"
  → far fewer systems remain.
- **Day 117**, line 227: "**Write down every system that is now finished**, because from
  Day 118 you shift from"
  → Day 118, cut.

### phase-07-content.md

- **Day 119**, line 33: "Write a validator for every data type"
  → depends on Day 118's "one file per content type" standardisation, cut.
- **Day 119**, lines 36-37: "every shop's stock exists, every dialogue jump resolves, every
  quest condition / names a flag that something writes"
  → shops: Day 99; quests: Day 66. Cut.
- **Day 119**, lines 38-39: "an enemy with zero XP, an item that costs 0 / gold, a skill
  nothing can learn"
  → gold: Day 99; skill learning: Day 102. Cut.
- **Day 120**, line 60: "Extend the Day 62 dialogue hot reload to every data type, on one key
  (F6)"
  → Day 62, cut. No dialogue hot reload exists to extend.
- **Day 120**, line 65: "Hot reload textures and sounds, so replacing art in Phase 8 needs no
  restart"
  → Phase 08, not covered by the brief.
- **Day 120**, lines 68-69: "set party level, start any encounter, trigger any cutscene"
  → party level: Day 101; cutscenes: Day 63. Cut.

---

## Conflicts in non-day text (intros, block headings, checklists)

- **phase-04**, lines 3-6 (intro): "This phase builds everything that / happens outside
  battle: NPCs you talk to, a dialogue system with branching and commands, / cutscenes,
  world state flags, interactive objects"
  → cutscenes: 63; interactive objects: 65. Cut.
- **phase-04**, line 8 (intro): "The dialogue system is the biggest single piece here (Days
  55-62)"
  → Days 61-62 cut.
- **phase-04**, line 25: "## Block A - NPCs and dialogue (Days 54-62)" → 61-62 cut.
- **phase-04**, line 255: "## Block B - Cutscenes, world interaction, encounters (Days
  63-71)" → 63-66 and 70 cut.
- **phase-04 checklist**, line 410 (kept, mixed): "Dialogue supports branching, conditions,
  commands, and inline text effects"
  → inline text effects: Day 61, cut. The rest comes from Days 58-60.
- **phase-05**, lines 3-5 (intro): "Twenty-four days: ten to / build a solid turn-based
  battle, eight to build the Paper Mario action-command layer that / makes it
  *interactive*, and six to make it feel good."
  → now 18 days: 10, 6 and 2.
- **phase-05**, line 373: "## Block B - Action commands (Days 82-89)" and line 375: "Eight
  days on the mechanic that defines your game."
  → 84-85 cut, so six days remain.
- **phase-05**, line 582: "## Block C - Battle depth and polish (Days 90-95)"
  → only 91 and 95 remain.
- **phase-05 checklist**, line 648 (kept, mixed): "Four offensive command types plus guard
  and superguard all feel good"
  → mash, hold/release, rotate and sequence were Days 84-85, cut. Only the timed press
  remains.
- **phase-05 checklist**, line 651 (kept, Day 88): "FP, badges, and star power make builds
  meaningfully different"
  → badges and star power were deleted from Day 88.
- **phase-06**, lines 3-4 (intro): "Twenty-two days on the systems that surround combat:
  items, equipment, shops, party / management, levelling, menus, settings, and saving."
  → now 7 days; equipment, shops, party, levelling, menus and settings are cut.
- **phase-06**, line 9 (intro): "**Menus are gameplay.** A player will open the inventory
  hundreds of times."
  → inventory menu: Day 108, cut.
- **phase-06**, line 25: "## Block A - Items, equipment, party, progression (Days 96-105)"
  → only 96-97 remain.
- **phase-06**, line 89: "## Block B - Menus, settings, and saving (Days 106-117)"
  → menus and settings (106-112) cut.
- **phase-06 checklist**, line 241: "The game boots correctly on a machine with no config and
  no saves"
  → settings config: Day 111, cut. A bindings config still exists from Day 41.
- **phase-07**, line 1: "# Phase 07 - Content pipeline and chapters (Days 118-150)"
  → chapters cut.
- **phase-07**, lines 8-9 (intro): "this phase's first block: eight days spent making content
  / production *fast*"
  → two days remain (119-120).
- **phase-07**, line 25: "## Block A - The content pipeline (Days 118-125)"
  → only 119-120 remain.
- **phase-07**, lines 80-101: "## The 8-day chapter template" (whole section)
  → was written out by Day 125 and used by the chapter days, all cut. Its table refers to
  "item and badge rewards" (badges), "chests, doors, switches, save points, shops,
  triggers" (Days 65, 99, 64), "dialogue and cutscenes" (Day 63), "the tier table" (Day
  124) and "batch balance simulation" (Days 94/124). Its hard rules ("No new engine
  systems") still match Day 120b.

---

## Notes on the two additions

- **Day 69b saves "inventory" before the inventory system exists.** `class Inventory` is
  Day 96. At Day 69b the inventory is whatever Day 60's `give_item` / `take_item` write to.
- **Day 69b saves "party HP" before battles exist.** HP at that point is the persistent
  party data passed into Day 68's stub battle. Real damage arrives in Phase 05.
- **Day 69b and Day 114 define two save APIs**: `SaveGame(path)` / `LoadGame(path)` versus
  `Save(int slot)` / `Load(int slot)`. Day 114 does not say it replaces the earlier one.
- **Day 69b's debug keys** (F10, Shift+F10) don't clash with any key used elsewhere (F1,
  F5-F9).
- **Day 120b** covers in one day what the original plan spread over an 8-day template per
  chapter. It only references kept days: 52, 56, 59, 60, 67, 68, 83, 86, 88, 91, 116 (title
  screen), 119 (`--validate`) and 120 (hot reload, developer console).

---

## Phase 08 (unchanged, not covered by the brief)

- Intro, line 7: "At Day 150 you have a playable game" → Day 150, cut.
- **Day 152**: "Draw the tilesets for all three chapters' environments" → chapters.
- **Day 153**: "Draw the player and party members" → Day 100 (party), cut.
- **Day 155**: "level-up rays" → Day 101.
- **Day 156**: "three chapter overworld themes, a town theme, a dungeon / theme" → chapters.
- **Day 157**: "level-up spectacle, chest-opening sparkle" → Days 101, 65; "a subtle drift
  in cutscenes" → Day 63.
- **Day 159**: "pool / particles and damage numbers" and "every particle effect / firing at
  once" → Day 93 (particle system).
- **Day 160**: "a full inventory, zero gold, max level" → Days 99, 101.
- **Day 161**: "Verify everything promised on Day 111 works throughout the finished game" →
  Day 111; "add the large-font option to every screen" → Day 111; "since mash / commands
  are a genuine accessibility barrier" → Day 84.
- **Day 163**: "time per chapter" → chapters.
- **Day 164**: "any chapter taking twice as long / as intended" → chapters.
- **Day 165**: "Do a full balance pass with the batch simulator" → Days 94/124.
- **Day 167**: "Confirm saves and config write to an appropriate per-user location" →
  settings config, Day 111.
- **Day 168**: "Write the description around your three pillars" → pillars were defined on
  Day 121, cut.
- **Day 169**: "every settings option, rebinding, every chapter, every boss" → Days 111, 112,
  chapters.
