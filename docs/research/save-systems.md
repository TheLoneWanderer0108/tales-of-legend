# Save systems

**Used on:** Days 113-115 (design, implementation, robustness), Day 116 (file select), Day 165
(migration testing).

Why this matters: a save bug is the worst kind of bug you can ship. A crash costs a player a few
minutes; a corrupted save costs them everything and they will never trust your game again. This is also
a system where the *format* decisions are effectively permanent once real players have saves, so the
design day matters more than the implementation day.

---

## 1. What a save must contain

Be systematic. Anything missing produces the classic "it worked in testing" bug, because in testing the
value was already correct in memory.

**Must save:**

- Party: for each character - level, XP, current HP/FP, learned skills, equipped items, equipped badges
- Roster and active party composition
- Inventory contents and gold
- All flags and world state
- Quest progress (if not fully derived from flags)
- Bestiary and collection data
- Current map ID and player position and facing
- Per-map object states (opened chests, thrown switches, defeated permanent enemies)
- Playtime, save count, and the chapter or location name for the file select
- A `version` number

**Must not save:**

- Derived stats (total ATK) - recompute from base plus equipment plus badges on load
- Loaded assets or any pointer
- Camera state, animation state, particle state
- Settings and key bindings (these belong in a **separate** config file, shared by all saves)
- Anything reconstructible

Storing derived data invites the save and the runtime to disagree, and it also means a balance change
does not apply to existing saves - which is usually not what you want.

**Questions to answer**

- If you change a potion's heal amount, should existing saves see the new value? (Yes - which is why you
  store IDs, not copies.)
- What should happen to statuses when saving mid-dungeon? (Almost certainly clear them; decide.)
- Where in the game is saving allowed, and why?

---

## 2. Format

**JSON, indented and human-readable.** For a project like this it is the right choice, and the reason is
debugging: when a load goes wrong you can open the file and *see* the problem. That advantage outweighs
file size and parse speed entirely at this scale.

Binary saves are smaller and faster, resist casual cheating, and are miserable to debug. You do not need
any of their advantages.

Do not obfuscate or encrypt. Players editing their own single-player save file is not a threat, and
obfuscation makes your own bug reports unreadable.

---

## 3. Layout: metadata first

Put a metadata block at the top that can be read **without** parsing the whole file:

```json
{
  "version": 1,
  "meta": {
    "chapter": "Chapter 2: The Sunken Road",
    "location": "Wellspring Town",
    "playtimeSeconds": 7245,
    "partyLevels": [8, 7, 7],
    "savedAt": "2026-08-10T18:00:00Z"
  },
  "party": { ... },
  "world": { ... }
}
```

The file select screen (Day 116) then reads only `meta` for each slot, which is fast and cannot fail on
a corrupt body. Include enough for the player to recognise the save: where they are, how far in, and how
long they have played.

---

## 4. Versioning and migration

**Add `version` to the very first save you ever write.** It costs nothing now and cannot be added
retroactively.

You *will* change the schema during Phase 7 - you will add a field, rename an item ID, or restructure
quests. Write the migration skeleton on Day 115 before you need it:

```
MigrateSave(json& save):
    while save.version < CURRENT_VERSION:
        apply the step from save.version to save.version + 1
        save.version += 1
```

Each step is a small function. Keep them forever, even when they seem obsolete - a save from three
months ago must still load.

Handle a **future** version (a save from a newer build) by refusing to load with a clear message rather
than attempting it and producing nonsense.

---

## 5. Serialise by ID, never by index

The rule that prevents the nastiest class of save bug.

If you save "item index 7" and later reorder `items.json`, every existing save now has a different item.
Nothing crashes; the player simply finds their sword has become a potion. Save `"iron_sword"` instead.

The same applies to skills, badges, maps, quests, characters, and spawn points. String IDs are slightly
larger and infinitely safer, and they make the save file readable while debugging.

---

## 6. Atomic writes

The failure to prevent: a crash or a power cut *during* a save leaves a truncated file, and the player
loses everything.

The fix is simple and non-negotiable:

1. Write the new data to a temporary file (`save1.tmp`).
2. Flush and close it.
3. **Rename** it over the real file (`save1.json`).

Rename is atomic on every platform you care about, so at every instant the real file is either the
complete old save or the complete new one - never a half-written one.

Additionally keep a backup: before renaming, move the existing `save1.json` to `save1.bak`. If the main
file ever fails to parse, fall back to the backup and tell the player.

---

## 7. Corruption and validation

Assume every save file may be broken - truncated, hand-edited, from an older version, or written by a
buggy build of your own game.

On load, check in order:

1. The file exists and is readable.
2. It parses as JSON.
3. `version` is present and not from the future.
4. Required top-level sections exist.
5. Values are sane: HP within range, level within bounds, gold not negative, inventory counts positive.
6. Referenced IDs exist in the current content (drop unknown ones with a warning rather than crashing).

If any check fails: **refuse to load, keep the current game state intact, and show a clear message.**
Loading a half-valid save is worse than refusing, because the player will keep playing and save again
over the top.

Test all of these deliberately (Day 115): truncate a file, corrupt the JSON, delete a required field,
set a future version, set HP above max, and rename an item that a save references.

---

## 8. Slots, autosave, and where files live

- **Three manual slots plus one autosave slot** is a good shape for a game this size.
- **Autosave** on map transitions and after battles, into its own slot so it never overwrites a manual
  save. Show a small non-intrusive indicator - a save that happens invisibly worries players.
- **Never autosave over a manual save**, and never autosave into a state the player cannot escape (do
  not autosave immediately before an unwinnable fight).
- **Location**: write saves and config to a per-user directory, not next to the executable. The install
  directory may be read-only, and multiple users may share a machine. raylib does not give you this
  path, so use the platform convention (`$XDG_DATA_HOME` or `~/.local/share/<game>` on Linux,
  `%APPDATA%` on Windows). Verify this before shipping (Day 167).
- Handle a full disk and a read-only directory without losing the in-memory game.

---

## 9. Restoring correctly

Loading is not just parsing - it is rebuilding a world:

1. Parse and validate.
2. Reset the session to a clean state (do not load *into* an existing session; stale values are exactly
   how "it worked in testing" bugs happen).
3. Restore the party, inventory, gold, flags, and quest state.
4. **Recompute all derived stats** from scratch.
5. Load the saved map and apply per-map object states.
6. Place the player at the saved position and facing.
7. Start the correct music and enter the field state.

The only test that proves this works: save, **quit the process entirely**, relaunch, load, and verify
every value. Testing a load without restarting will pass while the real thing is broken, because values
still linger in memory.

---

## 10. Save points and design

Where the player may save is a design decision with real consequences:

- **Anywhere** is the most player-respectful choice, and it is what most modern games do. It requires
  that every state is safely saveable, which is a good constraint on your architecture anyway.
- **At save points only** creates tension in dungeons and gives you defined recovery states, at the cost
  of respecting the player's time less.
- **A hybrid** (anywhere in the overworld, save points in dungeons) is a reasonable compromise, and it
  pairs naturally with your Day 104 rest points.

Whatever you choose: **never allow a save state from which the game cannot be completed.** Audit for it
(Days 66 and 105). Sold a key item, saved before an unwinnable fight with no escape, saved with zero
gold and no way to earn more - each is an unrecoverable trap, and each is entirely preventable.

Also: game over must return to a real state, either the last save or a checkpoint. A game over that
loses an hour of play will lose you players permanently.

---

## Exercises

1. Write your save schema on paper before any code, including the metadata block.
2. Implement save and load, then test only after a full process restart. Note what you forgot.
3. Implement atomic writes and test by killing the process mid-save (a sleep in the middle makes this
   easy to hit).
4. Break a save file in five ways from section 7 and confirm each is handled gracefully.
5. Write a version-1-to-2 migration for a field you add, and load an old save through it.
6. Save mid-dungeon with a half-finished quest, restart, load, and verify the quest state exactly.

## Done criteria

- [ ] My schema is written down and has a `version` field
- [ ] Metadata can be read without parsing the whole file
- [ ] Everything is serialised by string ID, never by index
- [ ] Derived stats are recomputed on load, never saved
- [ ] Writes are atomic, with a backup fallback
- [ ] All five corruption cases are handled with clear messages
- [ ] A migration path exists and has been tested with a real old save
- [ ] Saves live in a per-user directory and work with a read-only install
- [ ] Round trips are verified after a full process restart
- [ ] No save state can be unwinnable

## Sources

- Search "atomic file write rename pattern" for the technique's details
- [nlohmann/json documentation](https://github.com/nlohmann/json) for the serialisation API
- Search "save game versioning migration" - the strategy discussion applies to any format
- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/latest/) for
  where files belong on Linux
