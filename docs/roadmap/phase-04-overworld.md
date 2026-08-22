# Phase 04 - Overworld and dialogue (Days 54-71)

The field is where a JRPG spends most of its minutes. This phase builds everything that
happens outside battle: NPCs you talk to, a dialogue system with branching and commands,
cutscenes, world state flags, interactive objects, and Paper Mario-style field encounters
that lead into combat.

The dialogue system is the biggest single piece here (Days 55-62) and it is worth the time.
In a story game, dialogue is not a feature, it is the delivery mechanism for the entire
game. A dialogue system that is annoying to author will silently cause you to write less
game.

Research notes for this phase:
[dialogue-systems.md](../research/dialogue-systems.md),
[ui-systems.md](../research/ui-systems.md),
[state-machines.md](../research/state-machines.md),
[enemy-ai.md](../research/enemy-ai.md),
[data-driven-design.md](../research/data-driven-design.md),
[jrpg-game-design.md](../research/jrpg-game-design.md).

---

## Block A - NPCs and dialogue (Days 54-62)

### Day 54 - NPCs and interaction

**Goal:** walk up to someone, press a button, and get a response.

**New concepts:** interaction volumes, facing direction, proximity queries, prompt UI,
one interaction at a time.

**Tasks**

- [ ] Spawn NPC entities from the map's object layer with a `dialogueId` property
- [ ] Give NPCs idle animations and a facing direction. Have them turn to face the player
      when spoken to - a tiny detail that makes a world feel alive
- [ ] Implement `int FindInteractable(const Player&, const EntityWorld&)`: build a small
      probe rectangle in front of the player based on facing, test it against interactable
      entities, and return the nearest match's ID
- [ ] Draw a floating prompt (a bobbing "!" or a button glyph) above the interactable when
      one is in range. Use a Day 50 tween for the bob
- [ ] Handle two overlapping NPCs by choosing the nearest to the player's centre, so
      interaction is never ambiguous
- [ ] Block player movement while interacting, and make sure the interact key does not also
      confirm the first dialogue page in the same frame (consume the press)
- [ ] Add a debug draw of the interaction probe

**Deliverable:** walk up to three NPCs, see a prompt, press Confirm, and get placeholder
text on screen.

**Journal:** the double-input bug, and how consuming input fixes it.

---

### Day 55 - The dialogue box

**Goal:** a polished, reusable text window.

**New concepts:** UI composition, nine-slice boxes, portraits, page queues, box animation.

**Research first (30 min):** [ui-systems.md](../research/ui-systems.md) sections 1-4.

**Tasks**

- [ ] Build `class DialogueBox` with `Show(page)`, `Update(dt)`, `Draw()`,
      `bool IsFinished()`
- [ ] Draw a proper frame: a nine-slice border from a texture, or rounded rectangles with a
      2px inner border if you have no art yet
- [ ] Add a name plate above the box and a portrait area to its left
- [ ] Typewriter reveal at a configurable characters-per-second, with Confirm to fast-forward
      the current page and Confirm again to advance
- [ ] Split a long message into pages that fit exactly three lines, using Day 31's
      measured wrapping
- [ ] Animate the box in and out (scale or slide, `EaseOutBack` in, `EaseInQuad` out)
- [ ] Add a per-character sound with slight pitch variation, skipped for spaces and
      punctuation, and a maximum rate so it never becomes a buzz
- [ ] Add a blinking advance arrow, and a distinct one for the last page

**Deliverable:** a dialogue box you are happy to look at 5000 times.

**Journal:** what made the biggest difference to how good it feels?

---

### Day 56 - Designing your dialogue format

**Goal:** decide how you will *author* dialogue, before you build the machinery.

**New concepts:** authoring ergonomics, external DSL vs data format, tokenising, parsing,
error reporting with line numbers.

**Research first (40 min):** [dialogue-systems.md](../research/dialogue-systems.md)
sections 1-5. Look at how Ink, Yarn Spinner and RPG Maker approach this. **Do not adopt a
library** - the point is to design your own small format.

**Tasks**

- [ ] Write, on paper, the five dialogue scenes you know you need: a greeting, a shopkeeper,
      a scene that changes after a flag is set, a scene with a choice, and a scene that
      gives an item
- [ ] Design the smallest format that expresses all five. A line-based format is far easier
      to write by hand than JSON. Something like:

```
# elder_first_meeting
elder: Ah, a stranger. It has been {years} years since anyone came here.
elder happy: You look tired. Sit, sit.
> Ask about the tower -> ask_tower
> Say nothing -> silence
set met_elder = 1
end

# ask_tower
elder worried: The tower? You should not go there.
give_item Old Key 1
end
```

- [ ] Write the grammar down explicitly in [dialogue-systems.md](../research/dialogue-systems.md)
      or the design doc: what a node is, what a line is, what a command is, what a choice is
- [ ] Decide how conditions work (`if met_elder == 0` blocks, or per-node conditions).
      Choose the version you would rather write 200 of
- [ ] Write two sample script files by hand and see whether authoring feels good. Iterate on
      the format now - changing it after 40 scenes exist is expensive
- [ ] Write down every error your parser must report clearly: unknown node, missing `end`,
      malformed choice, unknown command, unknown flag

**Deliverable:** a written grammar and two hand-authored sample scripts you enjoyed writing.

**Journal:** why did you choose your format over JSON?

---

### Day 57 - The dialogue parser and runner

**Goal:** load scripts and play them back.

**New concepts:** tokenising, an instruction list, a program counter, an interpreter loop,
separating parse-time from run-time errors.

**Tasks**

- [ ] Parse a script file into `struct DialogueNode { std::string id; std::vector<Line> lines; };`
      where `Line` is a variant-like struct: text, command, choice set, or jump
- [ ] Parse **all** files at startup into a `DialogueDatabase` keyed by node ID, and report
      every error with file name and line number
- [ ] Write `class DialogueRunner` with `Start(nodeId)`, `Update(dt)`, `bool Finished()` and
      an instruction pointer. It walks lines, feeding text to the `DialogueBox` and waiting
      for input
- [ ] Make dialogue a pushed `GameState` (`DialogueState`) so the field draws underneath but
      does not update. You built exactly this on Day 38 - notice the reuse
- [ ] Validate all jump targets at load time. A typo'd node name should fail at startup,
      not two hours into playtesting
- [ ] Add a debug command to jump straight to any node by ID, for testing scenes without
      replaying the game

**Deliverable:** talking to an NPC plays a real scripted scene from a file.

**Stuck?** Get one text line from one node working end to end before adding commands or
choices. A tiny working pipeline beats a half-built complete one.

**Journal:** what surprised you about writing a parser?

---

### Day 58 - Branching choices

**Goal:** the player can answer.

**New concepts:** choice UI, cursor navigation in a menu that appears mid-dialogue,
control flow via jumps, default and cancel options.

**Tasks**

- [ ] Draw a choice window anchored to the dialogue box, sized to the longest option
- [ ] Navigate with Up/Down (with repeat), confirm with Confirm; support a cancel option
      when the scene allows it
- [ ] Jump to the chosen node and continue. Make sure the dialogue box state resets cleanly
      between nodes
- [ ] Support 2 to 4 options, and decide what happens with more (scroll, or forbid it in the
      parser - forbidding is usually better design)
- [ ] Implement a merge pattern: both branches jump to a shared node afterwards, so scenes
      do not duplicate their endings
- [ ] Author a real 3-choice conversation with distinct consequences and play it through
      every path
- [ ] Add a cursor sound and a confirm sound. Silence in a menu feels broken

**Deliverable:** a conversation with three meaningfully different outcomes.

**Journal:** how did the choice UI change the feel of the dialogue?

---

### Day 59 - Flags and world state

**Goal:** the world remembers what happened.

**New concepts:** global game state, key-value stores, conditional content, save-ready data
design, naming discipline.

**Tasks**

- [ ] Write `class GameFlags` holding `std::unordered_map<std::string, int>` with
      `Get(name)` (defaulting to 0), `Set(name, value)`, and `Has(name)`
- [ ] Decide the owner: flags belong to a persistent `GameSession`/`GameData` object owned
      by `Game`, **not** to `FieldState`. Verify by walking to another map and back
- [ ] Adopt a naming convention and write it in the design doc:
      `ch1_met_elder`, `ch1_tower_opened`, `item_key_taken_house3`. Chaos here becomes
      unfixable by chapter 3
- [ ] Add conditions to dialogue nodes: an NPC's greeting changes after
      `ch1_met_elder` is set
- [ ] Implement node selection by condition: several nodes share a "topic" and the runner
      picks the first whose condition passes. This is how NPCs stay relevant all game
- [ ] Add a debug flag inspector (F5): list every flag and its value, and let yourself
      toggle them. This single tool will save you hours of replaying scenes
- [ ] Publish a `FlagChanged` event via Day 49's bus and have something react to it

**Deliverable:** an NPC whose dialogue changes based on world state, plus a flag inspector.

**Journal:** your flag naming convention, written down where you will find it.

---

### Day 60 - Dialogue commands

**Goal:** dialogue can *do* things, not just say things.

**New concepts:** command dispatch, extensible command tables, side effects from scripts,
decoupling scripts from engine internals.

**Tasks**

- [ ] Implement a command table: `std::unordered_map<std::string, std::function<void(Args)>>`
      so adding a command is one registration, not a new `if` in a parser
- [ ] Implement: `set flag = value`, `give_item id count`, `take_item id count`,
      `play_sound id`, `play_music id`, `portrait actor mood`, `shake amount duration`,
      `wait seconds`, `face actor direction`, `heal_party`, `start_battle encounterId`
- [ ] Distinguish instant commands from blocking ones. `wait` and `shake` must pause the
      runner until finished; `set` must not
- [ ] Report unknown commands at *load* time by validating against the command table
- [ ] Author a scene that uses six commands together and verify the ordering is exactly what
      you wrote
- [ ] Add `{flag_name}` interpolation in text so dialogue can quote numbers, and
      `{player_name}` for the hero's name

**Deliverable:** a scene where an NPC gives you an item, sets a flag, and plays a jingle,
all authored in the script file.

**Journal:** how does the command table make adding a command cheap?

---

### Day 61 - Text effects

**Goal:** make the text itself expressive. This is a Paper Mario signature and it is cheap.

**New concepts:** inline markup, per-character rendering state, per-character transforms,
parsing tags out of display text.

**Tasks**

- [ ] Support inline tags: `[red]`, `[/red]`, `[shake]`, `[wave]`, `[slow]`, `[fast]`,
      `[pause=0.4]`, `[big]`
- [ ] Critical detail: strip tags before measuring and wrapping, so markup never affects
      layout. Store per-character style in a parallel array instead
- [ ] Render character by character, applying colour, a vertical sine wave offset, a random
      jitter, and scale per character
- [ ] Implement `[pause=x]` inside the typewriter, and speed changes mid-sentence. A pause
      before a punchline is worth more than any animation
- [ ] Auto-pause slightly at commas and full stops. It reads dramatically better for one
      line of code
- [ ] Write a test scene exercising every tag, and keep it as a regression check you can
      jump to with your debug node command

**Deliverable:** dialogue with shaking, waving, coloured, and dramatically paced text.

**Journal:** which effect gave the biggest personality boost?

---

### Day 62 - Dialogue polish and validation

**Goal:** make the system trustworthy so you can write content fast in Phase 7.

**New concepts:** authoring tools, validation passes, content linting, hot reload.

**Tasks**

- [ ] Write a validator run at startup (and on demand): unreachable nodes, jumps to missing
      nodes, unknown flags read but never written, unknown items, unknown sounds, lines that
      exceed the box even after wrapping
- [ ] Add hot reload: press F6 to reparse all scripts without restarting. Iterating on
      dialogue at 2 seconds per edit instead of 20 changes how much you write
- [ ] Add a "log mode" that dumps every scene to a text file, so you can read your whole
      script as prose and catch bad writing
- [ ] Add a skip-all-text mode for fast playtesting (hold Cancel)
- [ ] Write the town's real conversations: 4 NPCs, 3 with conditional variants, one with a
      choice, one shopkeeper stub. Roughly 40 lines of dialogue
- [ ] Play the whole town and fix everything that feels slow, wordy, or unclear
- [ ] Commit: `day 62: dialogue system complete`

**Deliverable:** a validated, hot-reloadable dialogue system and a town that talks.

**Journal:** how long does it now take you to add a new conversation? (If it is over five
minutes, fix the tooling, not the content.)

---

## Block B - Cutscenes, world interaction, encounters (Days 63-71)

### Day 63 - The cutscene system

**Goal:** scripted sequences where the game plays itself.

**New concepts:** command queues, coroutine-like sequencing without coroutines, blocking vs
parallel actions, taking control from the player.

**Research first (25 min):** [state-machines.md](../research/state-machines.md) sections 6-8
(hierarchical states and sequencing).

**Tasks**

- [ ] Write `class Cutscene` holding a queue of `Action` objects, each with
      `Start()`, `Update(dt)`, `bool Finished()`. This is Day 50's `Sequence` grown up
- [ ] Implement actions: `MoveEntityTo(id, tile, speed)`, `Wait(seconds)`,
      `PlayAnimation(id, name)`, `FaceEntity(id, dir)`, `PanCamera(target, duration)`,
      `RunDialogue(nodeId)`, `SetFlag`, `FadeScreen(colour, duration)`,
      `PlaySound`, `SpawnEntity`, `DespawnEntity`
- [ ] Support parallel groups: two NPCs walking simultaneously, then both waiting for each
      other before the next action
- [ ] Push a `CutsceneState` that suppresses player input but still draws and updates the
      field
- [ ] Extend the dialogue script format so a cutscene can be authored in the same file - one
      authoring language for the whole game is a large ergonomic win
- [ ] Add a skip button (hold Cancel for 1 second) that runs every action's end state
      immediately. Implement this *now*; a cutscene you cannot skip is a cutscene you will
      stop testing
- [ ] Author a real 20-second opening cutscene for the town

**Deliverable:** a skippable opening cutscene with movement, camera work, and dialogue.

**Journal:** what was hardest about sequencing?

---

### Day 64 - Triggers

**Goal:** the world reacts to where the player is and what they have done.

**New concepts:** trigger volumes, one-shot events, trigger conditions, re-entrancy.

**Tasks**

- [ ] Add trigger objects in Tiled with properties: `cutsceneId` or `dialogueId`,
      `once` (bool), `requiresFlag`, `setsFlag`
- [ ] Fire on player overlap, and only once per entry (track the entered state so standing
      still does not retrigger every frame)
- [ ] Implement `once` triggers via flags, so they stay consumed after saving and loading
- [ ] Add on-map-enter triggers, so entering a map can start a scene
- [ ] Add the classic edge case: a trigger that starts a cutscene which moves the player
      *through* another trigger. Decide the rule (triggers disabled during cutscenes) and
      enforce it
- [ ] Visualise all triggers and their conditions in the debug overlay
- [ ] Author two: an entry cutscene, and a warning that fires once when approaching the
      dungeon

**Deliverable:** triggers that fire exactly once, survive reloading, and never fire during
cutscenes.

**Journal:** the re-entrancy problem and your rule for it.

---

### Day 65 - Interactive objects

**Goal:** a world you can act on, not just walk through.

**New concepts:** entity behaviours as data, persistent object state, keys and locks,
switches, per-object flags.

**Tasks**

- [ ] Chests: an open animation, an item reward, a per-chest flag so it stays open forever,
      an "already empty" message
- [ ] Locked doors requiring a key item, with distinct messages for having and lacking the
      key
- [ ] Signs and readable objects (reuse dialogue directly - no new system)
- [ ] Switches and pressure plates that set flags, and doors that open when a flag set is
      satisfied
- [ ] A pushable block with tile-aligned movement and collision checks against tiles,
      entities and map bounds
- [ ] Breakable pots that drop coins with a small particle burst
- [ ] Verify all object states persist across a map transition and back

**Deliverable:** a small puzzle room: push a block onto a plate, open a door, find a key,
unlock a chest.

**Journal:** which object needed the most special-casing, and could it have been data
instead?

---

### Day 66 - Quests and world progression

**Goal:** the player knows what they are doing and the game knows how far along they are.

**New concepts:** quest state as derived data, progression gating, journals, soft locks.

**Tasks**

- [ ] Model quests on top of flags rather than as a parallel system:
      `struct QuestDef { std::string id, title; std::vector<QuestStep> steps; };`
      where each step has a description and a completion condition over flags
- [ ] Load quests from a data file
- [ ] Write `QuestState CurrentStep(const QuestDef&, const GameFlags&)` so quest progress is
      always *derived* and can never desynchronise from the world
- [ ] Build a minimal journal UI (a pushed state): active quests, the current step, and
      completed quests
- [ ] Add a main-quest gate: the dungeon entrance is blocked until a flag is set
- [ ] Hunt for soft locks: is there any way to make the game unwinnable? Sell a key item,
      trigger scenes out of order, leave a required area? Write down each risk and how you
      prevent it
- [ ] Author one main quest with three steps and one side quest

**Deliverable:** a journal that always reflects reality, and one gated main quest.

**Journal:** why derive quest state from flags instead of storing it?

---

### Day 67 - Field enemies

**Goal:** enemies that live on the map, in the Paper Mario tradition of visible foes.

**New concepts:** AI state machines, patrol and chase behaviour, aggro radius, line of
sight, steering.

**Research first (30 min):** [enemy-ai.md](../research/enemy-ai.md) sections 3-5.

**Tasks**

- [ ] Give field enemies an AI state machine: `Idle`, `Patrol`, `Chase`, `Return`, `Stunned`
- [ ] Patrol along waypoints authored in Tiled (a polyline or a set of point objects)
- [ ] Chase when the player enters an aggro radius **and** is in front of the enemy, and
      give up after losing sight for 2 seconds - always give the player a way to escape
- [ ] Add a visible telegraph of state: an "!" on aggro, a "?" on losing you. Readable AI is
      better AI
- [ ] Enemies must respect tile collision and not walk through walls or each other
- [ ] Add different movement profiles as data: slow chaser, fast erratic, stationary shooter
- [ ] Debug view: aggro radius, current state, patrol path, target

**Deliverable:** three enemies with visibly different, readable behaviour, that you can
outrun.

**Journal:** what made the AI feel readable rather than random?

---

### Day 68 - Entering battle

**Goal:** the transition from field to battle, with Paper Mario's first-strike rule.

**New concepts:** encounter definitions, contextual state transitions, transition effects,
passing data between states.

**Tasks**

- [ ] Define `struct EncounterDef { std::string id; std::vector<std::string> enemyIds; std::string arenaId; std::string music; };`
      loaded from data, and reference an encounter ID on each field enemy
- [ ] Detect the contact and classify it: player attacked from the front or above
      (**player first strike**), enemy touched the player from behind
      (**enemy first strike**), or a neutral collision
- [ ] Implement a field attack: press an action key to swing, which stuns nearby enemies for
      a second (this is also your Paper Mario field hammer/jump move)
- [ ] Build a battle transition: freeze the field, flash, a swirl or shatter effect, pause on
      black for a moment, then push `BattleState`. Steal the timing from a game you like -
      it is usually around 0.8 seconds total
- [ ] Create a stub `BattleState` that shows "Battle: goblin, goblin (player first strike)"
      and exits on a key press
- [ ] Pass data cleanly: the encounter def, the first-strike flag, and a reference to
      persistent party data. Do **not** copy the party into the battle
- [ ] Store where the player was and which entity started the battle, for the return trip

**Deliverable:** touching a field enemy plays a real transition and enters a stub battle
that knows who ambushed whom.

**Journal:** how did you pass data between states without a global?

---

### Day 69 - Returning from battle

**Goal:** close the loop cleanly, both on victory and on defeat.

**New concepts:** result objects, applying outcomes, respawn policy, game over flow.

**Tasks**

- [ ] Define `struct BattleResult { Outcome outcome; int xpGained; int goldGained; std::vector<std::string> itemsGained; bool fled; };`
- [ ] On victory: pop the battle, remove the defeated field enemy (set a flag so it stays
      gone until the map reloads), show a floating reward summary, resume the field music
- [ ] On defeat: fade to a game over screen with Continue (reload last save - stub until
      Phase 6) and Title options
- [ ] On flee: return the player to the field, briefly invulnerable and pushed away from the
      enemy so the battle does not instantly retrigger. This bug will absolutely happen to
      you
- [ ] Decide and document the respawn policy: do enemies return on map re-entry? (Yes is
      the usual answer, so grinding is possible.)
- [ ] Apply XP and level-ups from the result (a stub is fine; Phase 6 owns progression)
- [ ] Run the full loop 20 times looking for leaks and stale state - especially event bus
      subscriptions from the battle state

**Deliverable:** field to battle to field, on victory, defeat and flee, 20 times with no
leaks.

**Journal:** the instant-retrigger bug and how you solved it.

---

### Day 70 - Field abilities

**Goal:** Paper Mario's other signature - battle moves that are also world tools.

**New concepts:** shared abilities across contexts, ability gating for progression,
context-sensitive input.

**Tasks**

- [ ] Implement two field abilities usable with the same buttons as in battle: a jump
      (hit switches above, cross small gaps, stun flying enemies) and a hammer swing (break
      rocks, hit ground switches, stun ground enemies)
- [ ] Implement a third that gates progression, unlocked in chapter 2: e.g. a spin dash
      through cracked walls
- [ ] Make abilities data-driven and flag-gated so a chapter can unlock one
- [ ] Design one puzzle per ability, and one requiring two in combination
- [ ] Handle animation and input properly: no movement mid-swing, an ability cooldown, no
      queued spam
- [ ] Write down which abilities exist in both contexts and confirm they share their
      implementation where sensible. This shared vocabulary is what makes Paper Mario's
      world feel coherent

**Deliverable:** two working field abilities, three puzzles, one progression gate.

**Journal:** how does sharing abilities between field and battle change your design?

---

### Day 71 - Phase consolidation

**Goal:** a genuinely playable 10-minute slice of overworld.

**Tasks**

- [ ] Assemble: an opening cutscene, a town with four talking NPCs and a shopkeeper stub,
      a house, a puzzle room, a gated dungeon entrance, three field enemies, one main quest
      with three steps, and a working field-battle-field loop
- [ ] Play it start to finish five times, taking notes without fixing anything. Then fix the
      top five items on your list
- [ ] Hand it to a friend or family member. Watch them play in silence. Write down every
      moment of confusion. This is the single highest-value hour of the whole phase
- [ ] Run the sanitizer build and fix everything
- [ ] Update [../design/architecture.md](../design/architecture.md) and fill in the design
      doc sections you now actually know the answers to
- [ ] Commit: `day 71: playable overworld slice`

**Deliverable:** ten minutes of real game someone else can play without instruction.

---

## Phase 04 exit checklist

- [ ] I can author a new conversation in under five minutes
- [ ] Dialogue supports branching, conditions, commands, and inline text effects
- [ ] A validator catches broken scripts at startup, and F6 hot reloads them
- [ ] Flags persist across maps, are inspectable, and follow a naming convention
- [ ] Cutscenes sequence movement, camera, and dialogue, and are skippable
- [ ] Triggers fire exactly once and never during cutscenes
- [ ] Chests, doors, switches, and blocks all persist their state
- [ ] Quest state is derived from flags and shown in a journal
- [ ] Field enemies patrol, chase, and can be escaped and stunned
- [ ] Field to battle to field works on victory, defeat, and flee without leaking
- [ ] Someone else played my game and got through it
