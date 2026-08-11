# Data-driven design

**Used on:** Day 20 (first data files), Day 44 (JSON and maps), Day 96 (items), Days 118-120 (the
content pipeline), Day 124 (enemy authoring).

Why this matters: this is the single technique that decides whether your game ends up with 8 enemies or
40. Content in code requires a recompile per change; content in data requires a keypress. Over a
33-day content phase, that difference is the difference between a demo and a game.

The principle: **code defines what is possible; data defines what exists.**

---

## 1. What belongs in data

| In data | In code |
| --- | --- |
| Enemy stats, skills, drops | The damage pipeline |
| Item definitions and effects | The effect *appliers* |
| Skill definitions | The action command *types* |
| Dialogue and cutscenes | The dialogue runner |
| Maps and entity placement | Collision resolution |
| XP curves and growth tables | The levelling logic |
| Shop stock and prices | The purchase transaction |
| AI profile weights | The scoring algorithm |
| Balance numbers, all of them | Game rules |

The test: **if changing it would require a recompile, and it is a game design decision rather than a
game rule, it belongs in data.**

The clearest sign you have this wrong: an `if (enemyId == "boss1")` anywhere in your engine code. That
condition is content masquerading as logic, and it should be a property in a data file.

---

## 2. Format choice

**JSON** - recommended for structured data. Human-readable, one good C++ library
(`nlohmann/json`), universally understood, and easy to diff in git. Its weaknesses (no comments,
verbose) are minor for your use.

**A custom line-based format** - recommended for dialogue specifically, where prose in JSON is
miserable. See [dialogue-systems.md](dialogue-systems.md) section 2.

**CSV** - fine for pure tables (an enemy tier table, a growth curve). It is pleasant to edit in a
spreadsheet, which is a real advantage for balance work.

**TOML / YAML** - nicer to write than JSON, but another dependency for little gain.

**Binary** - do not. You lose readability, diffability, and hand-editing for a performance benefit you
do not need.

A reasonable split for this project: JSON for definitions, CSV for balance tables you want to graph, and
your own format for dialogue.

---

## 3. Schema design

Design the file shape before writing 200 entries.

Prefer an object keyed by ID over an array, so lookups are direct and IDs are visibly unique:

```json
{
  "version": 1,
  "items": {
    "potion": { "name": "Potion", "price": 20, "effects": [ { "type": "heal_hp", "amount": 20 } ] }
  }
}
```

Rules worth following:

- **A `version` field from day one.** It costs nothing now and enables migration later.
- **String IDs, not numeric.** `"potion"` survives reordering and is readable in save files; index 7
  does not.
- **Sensible defaults for everything optional**, so a minimal entry is short. If every item needs 15
  fields, authoring 60 items is misery.
- **Composition over special cases**: an effects *array* handles single, multiple, and complex effects
  with one mechanism.
- **Flat is better than deeply nested.** Three levels is usually enough.

---

## 4. Loading defensively

Content files are edited by hand and are therefore always wrong at some point. Every loader must:

- report the **file, the entry ID, and the problem** - never just "parse error";
- continue after an error so one run reports all 30 problems;
- validate required fields and types rather than trusting `operator[]`;
- apply defaults for missing optional fields;
- reject unknown fields with a warning (this catches typos like `"pirce"` that would otherwise silently
  become a default);
- never crash the game because content is wrong.

Then separate **file format** from **runtime format**. Load JSON into your own structs in one place. The
rest of your code should never know that JSON exists - which is what lets you change format later, and
what keeps Tiled's oddities confined to one function (Day 44).

---

## 5. Effects as data: the key pattern

The most valuable pattern in this note. Rather than a subclass per item behaviour, describe effects as
data and register handlers by type:

```json
"effects": [
  { "type": "heal_hp", "amount": 20 },
  { "type": "cure_status", "status": "poison" }
]
```

with a handler table `unordered_map<string, function<void(const Effect&, Context&)>>`.

Why this is powerful:

- New items need no code at all if they compose existing effects.
- One effect applier serves items, skills, dialogue commands, and traps - so "heal 20 HP" has exactly
  one implementation and one place to fix a bug.
- Composite effects (damage plus a status plus a buff) come free.
- A new *kind* of effect is one registration.

The same shape recurs throughout your game: the dialogue command table (Day 60), AI profiles (Day 79),
and action command types (Day 82). Notice the pattern; it is the practical version of "data over code"
from [entity-architecture.md](entity-architecture.md) section 6.

---

## 6. Cross-references and validation

Data files reference each other constantly: an encounter names enemies, an enemy names skills and drops,
a shop names items, a map names dialogue nodes.

Validate every reference at load time (Day 119). A missing reference discovered at startup costs
seconds; the same problem discovered mid-playthrough costs an hour and may corrupt a save.

Beyond errors, add **semantic warnings**: an enemy worth 0 XP, an item priced at 0, a skill nothing can
learn, a dialogue node nothing jumps to, a map with no exits. These are not crashes, they are design
mistakes, and a validator finds them faster than any playtest.

Build `--validate` as a command-line mode that checks everything and exits with a status code, and run
it before every commit.

---

## 7. Hot reload

The highest-leverage feature you will build (Day 120). Content iteration time directly determines how
much content you produce.

Requirements:

- One key (F6) reloads all data files.
- **Validate before swapping.** If the new data is invalid, keep the old and report the error - a typo
  must never crash a session.
- Reload maps while preserving the player's position, so map editing becomes a 2-second loop.
- Reload textures and sounds too, so art replacement in Phase 8 needs no restarts.
- Optionally poll file modification times so saving in Tiled or a text editor reloads automatically.

Things that cannot be hot reloaded need identifying: anything holding a pointer into the old data. This
is another argument for referencing content by string ID rather than by pointer.

Measure your iteration time before and after (Day 120 asks you to write both numbers down). Target under
10 seconds from edit to observed effect.

---

## 8. Authoring ergonomics

Data-driven only pays off if authoring is pleasant. Judge your formats by asking "would I enjoy writing
100 of these?"

- Short entries. Defaults for everything optional.
- Consistent field names across files (`id`, `name`, `description` everywhere).
- Group related content in one file (all fire skills together) so you can copy a neighbour.
- Keep a template entry at the top of each file, commented, to copy from.
- Order fields consistently, so scan reading works.
- Use your editor's JSON schema support if you can - autocomplete on field names removes a whole class
  of typo.

Day 124's target of a 20-minute enemy depends entirely on this.

---

## 9. Data and save files

Save files reference content by ID, which creates a compatibility question: what happens when you rename
an item that appears in an existing save?

- Reference by ID in saves, always.
- On load, handle unknown IDs by dropping them with a logged warning rather than crashing.
- Keep an ID rename map if you must rename something after saves exist.
- Never store *derived* content data in a save (an item's price, an enemy's stats). Store the ID and
  look up the current definition, so balance changes apply to existing saves.

See [save-systems.md](save-systems.md) for the rest.

---

## 10. When not to use data

Data-driven design has costs, and pretending otherwise leads to over-engineering:

- **Indirection.** Behaviour lives in a file, so tracing it requires knowing the file.
- **Weak type checking.** The compiler cannot catch a typo'd field name - only your validator can.
- **Debugging effort.** A logic error in data is harder to step through than one in code.
- **Over-generalisation.** Building a scripting language to express something one enemy needs is a trap;
  write that one thing in code.

The rule: **data for things there are many of; code for things there is one of.** Forty enemies belong
in data. One boss's unique three-phase shield gimmick can be code - with its numbers in data.

---

## Exercises

1. Convert one hardcoded enemy into a JSON entry and load it. Then add three more and note how much
   faster the fourth was.
2. Design your item schema, then hand-author 10 items and note every field that felt like busywork.
   Remove them or give them defaults.
3. Build the effect handler table and implement one new item using only existing effects.
4. Break a data file five ways and confirm the error messages are actionable without reading your
   loader's source.
5. Implement hot reload and time your edit-to-effect loop before and after.
6. Grep your codebase for content strings and count how many remain.

## Done criteria

- [ ] No content-specific conditionals exist in engine code
- [ ] Every content type has its own file with a `version` field and string IDs
- [ ] Loaders report file, entry, and problem, and continue after errors
- [ ] One effect system serves items, skills, and dialogue commands
- [ ] `--validate` checks cross-references and emits semantic warnings
- [ ] F6 hot reloads everything, keeping old data if validation fails
- [ ] My content iteration loop is under 10 seconds

## Sources

- [nlohmann/json documentation](https://github.com/nlohmann/json) - read the README, it is enough
- [Game Programming Patterns: Type Object](https://gameprogrammingpatterns.com/type-object.html) -
  the pattern behind this whole note
- Search "data driven game design content pipeline" for how larger teams approach it
- Look at a modded game's data files (any Paradox game, or Minecraft data packs) to see mature schema
  design
