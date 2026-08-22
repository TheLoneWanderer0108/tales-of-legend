# Enemy AI

**Used on:** Day 23 (console battle AI), Day 67 (field enemies), Day 79 (battle AI and telegraphing),
Day 91 (boss AI).

Why this matters: enemy AI in a JRPG has a job that is *not* "play optimally". Its job is to be
**readable, varied, and fair** - to give the player a puzzle they can learn to solve. An AI that plays
perfectly is usually less fun than one that plays characterfully.

The best AI in your game will be simple code plus good telegraphing.

---

## 1. What game AI is actually for

Three goals, in priority order:

1. **Readability.** The player must be able to predict what happens next, most of the time. Prediction
   is what makes their decisions meaningful.
2. **Variety.** Different enemies should behave differently, so encounters feel distinct.
3. **Challenge.** A distant third, and mostly achieved through mechanics rather than cleverness.

A perfectly optimal AI would focus-fire the weakest party member every turn, which is both brutal and
boring. Deliberate imperfection is a design tool, not a compromise.

**Questions to answer**

- If an enemy always did the optimal thing, would your battles be more or less fun?
- How does the player learn what an enemy will do? (If the answer is "by dying", improve the
  telegraphing.)

---

## 2. Weighted random: start here

The simplest useful battle AI: a list of actions with weights.

```
60% attack the lowest-HP hero
25% use a skill if FP allows
15% attack a random hero
```

This gets you a long way and is what you built on Day 23. It is readable ("this enemy mostly attacks"),
varied enough, and trivially data-driven.

Its limits: it cannot react to the situation. It will heal at full HP, waste an AoE on one target, and
buff on the last round of the fight.

---

## 3. Utility / scoring: the upgrade

Enumerate every legal action, score each, then pick the best - with a little randomness so the enemy is
not perfectly predictable.

```
for each possible action:
    score = base priority
    score += bonus if it would defeat a target this turn
    score += bonus if healing and an ally is below 30%
    score += bonus per extra target hit by an AoE
    score -= penalty if the target already has this status
    score -= penalty if FP is needed for something better
pick from the top few, weighted
```

This is much smarter with very little more code, and it stays debuggable because you can display the
scores. It also makes AI *authorable*: adjusting weights in data changes behaviour without touching
code.

The essential trick: **pick from the top 2-3 options randomly, not always the best**. That single
change turns a predictable optimiser into something that feels like a decision-maker while remaining
readable.

---

## 4. AI profiles as data

Do not write per-enemy AI code. Define profiles in data and assign them:

| Profile | Behaviour |
| --- | --- |
| `aggressive` | Prioritises damage, targets the weakest, ignores support |
| `defensive` | Guards when hurt, buffs defence, plays for attrition |
| `support` | Heals and buffs allies, attacks only when nothing else is useful |
| `erratic` | Mostly random - genuinely useful for making an enemy feel wild |
| `tactical` | Full scoring, exploits weaknesses, targets by threat |
| `boss` | Scripted phases plus scoring within each phase |

Then a new enemy needs only a profile name and a skill list. This is the same "data over code" lesson
as [entity-architecture.md](entity-architecture.md) section 6, and it is what makes Day 124's
20-minute enemy authoring possible.

---

## 5. Field AI: state machines

Field enemies (Day 67) are a different problem - movement rather than decisions. Use a state machine:

```
Idle -> Patrol -> Chase -> Return -> Idle
                    |
                 Stunned
```

- **Patrol** follows waypoints authored in Tiled, so behaviour is level design rather than code.
- **Chase** activates when the player is within an aggro radius **and** in front of the enemy. Giving
  enemies a facing and a limited view cone makes stealth and avoidance possible, which is far more
  interesting than omniscient aggro.
- **Return** after losing sight for ~2 seconds. The player must always be able to escape - being
  chased forever is exhausting.
- **Stunned** after a field attack, which is what makes your field abilities (Day 70) useful.

**Readable AI needs visible state.** An "!" on aggro and a "?" on losing you communicates the entire
state machine without a word of text. Do this; it costs nothing and transforms how fair the game feels.

Movement profiles as data: slow chaser, fast erratic, stationary shooter, one that flees. Same
principle as battle profiles.

---

## 6. Telegraphing: the most important part

This section matters more than everything above it, because your defensive action commands (Day 86)
are impossible without it.

The player must know an attack is coming, and roughly when it lands:

- **A wind-up animation** of consistent length. Consistency is non-negotiable - if the wind-up varies
  in length, the timing cannot be learned, and the mechanic becomes a coin flip.
- **An intent icon** showing what the enemy will do next round: a sword for attack, a shield for
  guard, a spiral for a special. This turns the enemy turn into a puzzle the player can prepare for.
- **An audio cue** at a fixed offset before impact. Many players will guard by ear rather than by eye,
  and audio timing is more precise for most people.
- **A pause before acting**, so the telegraph can be read. Tune it: too fast is unfair, too slow is
  boring. Around 0.3-0.6 seconds is usually right.

Design principle: **difficulty should come from execution, not from missing information.** An enemy
that is hard because you must react precisely is fun. An enemy that is hard because you cannot tell
what it is doing is just noise.

---

## 7. Boss AI

Bosses differ from normal enemies in structure, not intelligence:

- **Phases** by HP threshold, each with a different move set and telegraphs. Phase changes are the
  drama of the fight.
- **Scripted beats**: a signature attack on round 1, a desperation move below 25%, a summon at 50%.
  Scripting is *better* than intelligence here, because it is learnable and therefore masterable.
- **A gimmick** requiring a specific mechanic: a shield only broken by superguard counters, a charge
  that must be interrupted, minions that must be cleared. The gimmick is what makes the boss an exam
  on the chapter's lesson.
- **Telegraphed lethality.** A move that can kill from full HP must be visible far in advance, and
  there must be an answer to it.
- **No unwinnable states.** Verify a player who has been ignoring optimisation can still win.

Write out the boss's script as a table before coding it: for each phase, its HP range, its moves,
their weights, and its telegraphs.

---

## 8. Difficulty without unfairness

Levers that increase difficulty *fairly*:

- More enemies, or better enemy compositions.
- Enemies that punish specific mistakes (a counterattacker that hurts you for attacking into its
  guard).
- Tighter action command windows for tougher foes.
- Resource pressure (attrition across a dungeon rather than a single hard fight).
- Mechanics requiring specific answers.

Levers that feel unfair and should be avoided:

- Hidden information, or randomised telegraph timings.
- Enemies that act twice with no indication.
- Unavoidable damage.
- Status effects with no counter available.
- Stat inflation alone (a damage sponge is tedious, not difficult).

---

## 9. Debugging AI

- **Display the scored options** for the current enemy in a debug view. This distinguishes "my AI is
  bad" from "my scoring weights are wrong", which are entirely different bugs.
- **Log every AI decision** with its reasoning ("healing: ally at 22% HP").
- **Force a decision** via the developer console, so you can test a specific behaviour without waiting
  for it to occur.
- **Watch a headless simulation** (Day 94) and look for absurdities: an enemy healing at full HP, or
  buffing on the final round.
- **Play against it 20 times.** If you cannot learn its patterns after 20 fights, it is not readable
  enough.

---

## Exercises

1. Implement the weighted random AI, then upgrade it to scoring, and compare how each feels over ten
   fights.
2. Add "pick randomly from the top 3" and note the difference in how predictable it feels.
3. Build the field state machine with a view cone, and add the "!" and "?" indicators.
4. Give an enemy a wind-up animation of consistent length and try to guard it 20 times. Plot your
   success rate.
5. Write your boss's phase script as a table before implementing it.
6. Build the AI debug view showing scores, then deliberately mis-weight one option and watch the AI go
   wrong in an explainable way.

## Done criteria

- [ ] AI behaviour is defined by data profiles, not per-enemy code
- [ ] Enemies pick from the top few options rather than always the best
- [ ] Field enemies have visible states and can be escaped
- [ ] Every enemy attack has a consistent-length wind-up and an intent telegraph
- [ ] I can see the AI's scored options in a debug view
- [ ] My boss's script is written as a table
- [ ] Difficulty comes from execution, not from hidden information

## Sources

- [Game Programming Patterns: State](https://gameprogrammingpatterns.com/state.html) for field AI
- Search "utility AI game decision making" - the scoring approach, explained well in several talks
- Search "telegraphing enemy attacks game design" - the most valuable reading in this note
- [Behaviour trees explained](https://www.gamedeveloper.com/programming) - useful to know, but overkill
  for this project; read it after you ship
