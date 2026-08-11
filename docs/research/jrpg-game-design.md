# JRPG design: structure, pacing, and content

**Used on:** Day 91 (boss design), Days 121-122 (design doc and writing), Days 126-150 (the chapters),
Day 163 (playtesting).

Why this matters: by Phase 7 your engineering problems are solved and your remaining problems are all
design problems - how long a dungeon should be, when to introduce a mechanic, why a chapter drags. This
note is about the decisions that make the difference between "a working JRPG" and "a JRPG someone
finishes".

---

## 1. The core loop

Every good RPG has a loop the player can feel:

```
explore -> encounter -> grow stronger -> reach somewhere new -> explore
```

Each part must feed the next. Exploration must produce something (items, story, shortcuts); encounters
must produce growth (XP, gold, drops); growth must enable new places (a stronger party, a new ability).
When a loop breaks - exploration that yields nothing, growth that changes nothing - players lose
motivation without being able to say why.

Test your own loop: for each 20 minutes of your game, what did the player gain, and what did that gain
unlock?

---

## 2. Pacing and the shape of a session

Attention has a rhythm. A rough template for a 40-minute chapter:

| Minutes | Content | Purpose |
| --- | --- | --- |
| 0-5 | Story beat, a goal stated | Motivation |
| 5-12 | Town: shops, NPCs, side quest | Breathing room, preparation |
| 12-25 | Dungeon part 1, new mechanic taught | Escalation |
| 25-30 | Mid-point: save, rest, a twist | Relief and a hook |
| 30-38 | Dungeon part 2, harder encounters | Peak challenge |
| 38-45 | Boss, then resolution | Payoff |

Principles behind it:

- **Alternate tension and relief.** Town, dungeon, town. Continuous tension exhausts; continuous relief
  bores.
- **Never more than 10 minutes without a save opportunity.** Real people get interrupted.
- **Never more than 5 minutes without a small reward.** A chest, an XP milestone, a story line.
- **Frontload the hook.** Within the first 5 minutes the player must have control, a goal, and one
  successful action command.

Your best pacing tool is watching someone play (Day 163). Note the exact minute at which their attention
drifts; that is where to cut.

---

## 3. Teaching without tutorials

A tutorial screen is a confession that the design does not teach itself. Use the standard four-step
progression instead:

1. **Introduce safely.** The first timed press happens against an enemy that cannot kill you.
2. **Practise.** Two or three low-stakes repetitions.
3. **Test.** An encounter that requires it.
4. **Combine.** A later encounter needing it alongside something else.

Rules:

- **One new thing at a time.** Two simultaneous new mechanics means neither is learned.
- **Teach by need.** Introduce a mechanic just before the first place it is required, not in a lump at
  the start.
- **Let failure be cheap** the first time. A player who learns by losing a battle learns only that your
  game is unfair.
- **Show, do not tell.** An enemy that flies teaches "I need an aerial attack" faster than a text box
  saying so.
- **Repeat critical information** in the journal, because some players skip all text.

Chapter 1 (Days 126-133) is where nearly all teaching happens. Expect to revise it in Phase 8 after
watching real players.

---

## 4. Chapter structure

Three chapters, each with the same skeleton but escalating stakes:

| Chapter | Role | New mechanic | Feeling |
| --- | --- | --- | --- |
| 1 | Teach the game | Basic attack, guard | Discovery |
| 2 | Deepen it | A field ability that gates progression, plus a party member | Growth |
| 3 | Test everything | No new systems - combinations | Mastery |

The strongest structural idea available to you: **each chapter introduces exactly one mechanic, and its
boss is an exam on that mechanic.** It makes bosses meaningful, gives each chapter an identity, and tells
you exactly what to build.

Chapter 2's mechanic should ideally **open up earlier areas** (a spin dash that breaks cracked walls the
player has already walked past). Retroactively expanding a known space is one of the most satisfying
things a game can do, and it costs almost nothing.

---

## 5. Dungeon design

A dungeon is not a corridor with fights in it. Give it a structure:

- **A loop, not a line.** A path that returns to the entrance means backtracking is not a slog.
- **A hub with branches**, so the player makes a choice about where to go.
- **A mid-point** with a save, a rest, and ideally a story beat.
- **One mechanical idea** - a dungeon about switches, or about avoiding a patrolling enemy - so it is
  memorable as a place.
- **Optional rooms** with real rewards, so exploration is voluntary and rewarded.
- **A shortcut unlocked from the far side**, which makes progress feel physical.
- **3-6 maps.** Longer dungeons need a stronger reason to exist.

Encounter placement: not every room, and never in a corridor the player must re-cross repeatedly.
Because your enemies are visible on the field (Day 67), the player can choose to fight - which changes
placement into a design opportunity rather than a tax.

---

## 6. Boss design

A boss is an examination, a spectacle, and a story beat at once.

- **Phases** (2-3) by HP threshold, each changing the move set. Phase transitions are the drama.
- **A gimmick** requiring the chapter's mechanic - this is what makes the boss the exam.
- **Readable telegraphs**, consistently timed (see [enemy-ai.md](enemy-ai.md) section 6). Difficulty
  from execution, never from hidden information.
- **A desperation phase** below 25% HP, more aggressive - it creates a genuine climax.
- **Dialogue at phase transitions.** Cheap, and it turns a health bar into a character.
- **3-5 minutes.** Longer needs a checkpoint or it becomes an endurance test with a punishing retry.
- **Beatable by an unoptimised player.** Verify it; a boss requiring a specific build is a wall, not a
  challenge.
- **Unique presentation**: an intro, its own music, a health bar with phase markers, a dramatic defeat.

Write the boss out as a table (phase, HP range, moves, weights, telegraphs) before implementing it.

---

## 7. Content budget and the discipline of cutting

The single most common cause of unfinished games is scope. Numbers that fit the remaining days:

| Content | Target |
| --- | --- |
| Chapters | 3 |
| Maps | 20-30 |
| Enemy types | 15-20 |
| Bosses | 3 |
| Party members | 3 |
| Items | 30-40 |
| Skills per character | 6-8 |
| Badges | 20 |
| Music tracks | 10-12 |
| Dialogue | 3000-5000 words |
| Playtime | 2-3 hours |

Two hours of finished game is a real achievement and far more valuable than twenty hours of unfinished
game.

Practise cutting deliberately:

- **Write the cut list** (Day 121) and keep every rejected idea in it. A written cut list is what lets
  you say no in month seven without feeling like you failed.
- **Cut breadth before depth.** Three well-tuned enemies beat eight rough ones.
- **Cut features, not polish.** An unpolished game with many features feels worse than a polished game
  with few.
- **When behind, cut content, never testing.** An untested chapter is not content.

---

## 8. Story that a game can carry

Game stories work differently from prose:

- **Structure over prose.** Players remember what they *did*, not what was said.
- **A clear goal at all times.** The player should always be able to answer "what am I doing and why?"
  Check this every 5 minutes of your game.
- **One page for the whole story.** If it does not fit, it is too complicated for a first game.
- **Characters over plot.** A memorable companion beats an intricate conspiracy.
- **Let mechanics carry meaning.** A party member joining you in battle says more about the
  relationship than a cutscene about it.
- **Write the ending first**, so everything before it can point somewhere.
- **Short scenes.** Two minutes maximum, always skippable.

---

## 9. What players actually remember

Useful when deciding where to spend your remaining days:

1. **The first five minutes** - which is why Chapter 1 gets revised most.
2. **Boss fights** - the peaks.
3. **Moments of mastery** - the first perfect superguard chain.
4. **Characters** - a personality, a joke, a betrayal.
5. **The ending** - it colours the memory of everything.

They forget: exact numbers, most dialogue, most normal encounters, and menu layouts (though bad menus
are *felt* constantly even when not remembered).

Spend disproportionate effort on that first list.

---

## 10. Playtesting, honestly

You cannot evaluate your own game. You know where everything is and what everything does.

- **Give them nothing.** No explanation, no hints. What they need to be told is a design bug.
- **Watch in silence.** The hardest discipline and the most valuable hour in the whole project. Do not
  defend, do not explain, just record every hesitation.
- **Note the minute at which attention drifts.**
- **Ask afterwards**: where did you get stuck, where were you bored, what was confusing, what was your
  favourite moment, and if you stopped, why?
- **Two or more testers hitting the same thing is a real problem**, not a matter of taste.
- **Prioritise by (players affected) / (hours to fix).**
- **Log data too** (Day 163): time per chapter, deaths per encounter, action command success rates. Data
  and observation together are much stronger than either alone.

---

## Exercises

1. Write your core loop and identify what the player gains in each 20-minute segment.
2. Map your Chapter 1 against the pacing table in section 2 and find the gaps.
3. For each mechanic, write down where it is introduced safely, practised, tested, and combined.
4. Design one dungeon on paper as a loop with a hub, a mid-point, and one optional room.
5. Write your boss as a phase table before implementing it.
6. Write your entire story on one page. If it does not fit, cut until it does.
7. Fill in the content budget table with your own numbers, then multiply by your measured authoring
   times and check it fits.

## Done criteria

- [ ] My core loop is written down and every part feeds the next
- [ ] Each chapter introduces exactly one mechanic, examined by its boss
- [ ] Every mechanic has an introduce/practise/test/combine progression
- [ ] There is no tutorial screen
- [ ] No stretch of the game goes 10 minutes without a save or 5 without a reward
- [ ] Dungeons are loops with mid-points, not corridors
- [ ] My content budget is written down and fits my measured authoring times
- [ ] A cut list exists and I have used it
- [ ] Someone else has played it while I watched in silence

## Sources

- Study Paper Mario: The Thousand-Year Door's chapter structure specifically - each chapter is a
  self-contained arc with its own mechanic, and it is the clearest model available for what you are
  building
- [Game Maker's Toolkit](https://www.youtube.com/@GMTK) - search its back catalogue for tutorial design
  and boss design videos
- [The Door Problem](https://lizengland.com/blog/2014/04/the-door-problem/) - what game design actually
  consists of
- Search "JRPG dungeon design principles" and "how to teach mechanics without tutorials"
- Play the first hour of three JRPGs you admire, with a notebook, and record what happens minute by
  minute. It is the single most useful design exercise available to you.
