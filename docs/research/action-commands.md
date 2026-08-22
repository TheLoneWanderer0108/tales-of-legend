# Action commands

**Used on:** Days 82-89 (the whole action command block), Day 92 (battle speed interaction),
Day 161 (accessibility).

This is the most important research note in the roadmap. Action commands are what make your game
Paper Mario-like rather than a generic JRPG, and they are entirely about **feel** - which means they
are the one system you cannot get right by reasoning alone. You have to build, measure, and tune.

The core design idea: **a turn-based game asks the player to choose. An action command also asks them
to perform.** Choice engages the mind; performance engages the hands. Doing both every single turn is
why Paper Mario battles never feel passive.

---

## 1. Why they work

Three reasons worth understanding, because they tell you what not to break:

1. **They remove downtime.** In a traditional JRPG, once you pick "Attack" you watch. Here you are
   still playing, so attention never drops.
2. **They add a skill axis.** Two players with identical stats perform differently, which means
   progress can come from *getting better*, not just from levelling. That is a much better feeling.
3. **They make damage feel earned.** A critical hit from a dice roll is luck; a `Perfect` grade from a
   precise press is yours.

The corollary: if your commands are trivially easy or purely random, you lose all three benefits and
have added busywork. The skill must matter.

---

## 2. The taxonomy of commands

| Type | Input | Skill tested | Good for |
| --- | --- | --- | --- |
| **Timed press** | Press at the right moment | Precision | Basic attacks - your workhorse |
| **Mash** | Rapid repeated presses | Speed, endurance | Charge-up attacks, escaping a grab |
| **Hold and release** | Hold, release in a zone | Nerve, restraint | Charged attacks |
| **Rotate** | Circle the stick | Coordination | Big spectacle moves |
| **Sequence** | Several prompts in order | Memory, reaction | Multi-hit specials |
| **Alternate** | A, B, A, B rapidly | Coordination | Mash variant, hand-friendlier |
| **Aim** | Position a cursor and confirm | Spatial precision | Ranged attacks |
| **Guard** | Press just before impact | Reaction | **Every defence** |
| **Superguard** | Tighter window, different button | Mastery | High-risk defence |

Two design rules:

- **Variety per turn matters more than variety overall.** Three command types used constantly beats
  eight used rarely, because the player builds real skill at the ones they see often.
- **Match the command to the fantasy.** A hammer swing wants a hold-and-release charge; a rapid
  flurry wants a mash. When the input mirrors the action, learning is nearly instant.

---

## 3. Timing windows and grading

The numbers below are a starting point for a **timed press**. Tune them; do not trust them.

| Grade | Window (± from perfect) | Multiplier |
| --- | --- | --- |
| Perfect | 40ms (about 2.5 frames at 60fps) | 1.8x |
| Great | 90ms (about 5 frames) | 1.4x |
| Ok | 150ms (about 9 frames) | 1.1x |
| Miss | outside | 0.7x |

Two important choices embedded in that table:

- **A miss still does damage** (0.7x, not 0x). Zero-damage misses make a bad turn feel catastrophic
  and punish new players brutally. Paper Mario's basic attack always connects; the command modifies
  it.
- **The spread is meaningful.** From 0.7x to 1.8x is a 2.5x range, so skill visibly matters. If your
  spread is 0.95x to 1.05x, the commands are decorative and players will ignore them.

Grade by absolute time error, not by frames, and compute it on the fixed timestep so the numbers mean
the same thing on every machine.

---

## 4. Fairness: the three non-negotiables

Break any of these and your game feels unfair, which players experience as "broken" and rarely
articulate:

1. **At least 200ms of warning.** Human reaction time to a visual cue is roughly 200-250ms. A prompt
   appearing 100ms before its window is physically impossible to hit, no matter how skilled the
   player. Telegraph earlier than feels necessary.
2. **A pre-window input buffer.** Accept presses up to ~80ms *before* the window and treat them as
   on-time. Without this, players who are trying to be precise get punished for being marginally
   early, which feels dreadful. This one number is most of the difference between "tight" and
   "unfair".
3. **Frame-rate independence.** Verify at 30, 60, and 144 FPS that the same physical timing produces
   the same grade. This is why command logic lives on the fixed timestep.

A fourth, softer rule: **consistency**. If an enemy's wind-up is 0.6 seconds, it must be 0.6 seconds
every single time. Randomised telegraph lengths make timing unlearnable, and learnability is the
entire point.

---

## 5. Early presses, mashing, and cheat resistance

Decide explicitly what an early press does:

- **Punish** (an immediate miss): rewards precision, prevents mashing, and is what Paper Mario does.
- **Ignore**: forgiving, but allows mashing through everything, which destroys the mechanic.

Recommendation: punish, but combine it with the pre-window buffer so that *near*-early presses count
and *wildly* early presses fail. That combination is what feels both tight and fair.

Test by mashing constantly through an entire enemy turn. If mashing performs as well as timing, the
design has failed and no amount of tuning the numbers will save it.

For mash commands specifically: ignore presses closer than ~25ms apart to defeat autofire
controllers, and cap the *benefit* rather than the press count so nobody is rewarded for hurting
their hands.

---

## 6. Feedback: the part that makes them feel good

An action command with weak feedback feels like a chore. Layer four channels:

- **Anticipation.** The prompt pops in with a scale bounce and a wind-up. The player must see it
  coming.
- **Visual.** A clear marker and zone, a flash on hit, a grade label popping with ease-out-back.
- **Audio.** A rising tick as the window approaches (so it can be timed **by ear**, which is more
  precise than by eye for most people), plus a distinct sound per grade. Audio cues are the single
  biggest improvement you can make to timing feel.
- **Physical.** Hitstop on success, screen shake scaled to the grade, controller rumble if supported.

Also: give a distinct response to a **near miss** versus a total miss. "You were close" teaches;
"you failed" only punishes.

---

## 7. Defensive commands

The half that most Paper Mario imitators skip, and the half that makes the enemy turn interesting.

- **Guard**: a forgiving window (~120ms) before impact, for a modest reduction (say -1 flat and 50%).
  Always available, low risk.
- **Superguard**: a tight window (~60ms) on a *different button*, for full negation plus recoil damage
  to melee attackers. High risk (a mistimed press means no protection at all), high reward.

The critical implementation requirement: **the window must be derived from the attack animation's
contact frame, from one source of truth** (see
[animation-and-tweening.md](animation-and-tweening.md) section 8). If the animation and the window
disagree, the game is lying about when to press, and the player will feel cheated without being able
to explain why.

Multi-hit attacks get one window per hit. That is where superguard mastery becomes thrilling.

The test of a good defensive command: your success rate against a given enemy should measurably
improve over 20 attempts. If it does not, the tells are not readable.

---

## 8. Measuring feel

You cannot tune what you cannot measure. Build the harness on Day 82:

- Log for every attempt: window start and end, actual input time, error in milliseconds, and grade.
- Show a running success rate per grade.
- Allow replaying a command in isolation, repeatedly, without a battle around it.

Then measure yourself:

| Measurement | How | Target |
| --- | --- | --- |
| Input latency | Flash the screen on press, film at 60fps+, count frames | ≤ 3 frames |
| Success rate, focused | 20 attempts, concentrating | ~80% Great or better |
| Success rate, distracted | 20 attempts while thinking about strategy | ~40% Great or better |
| Learning curve | Success rate over 20 fights vs one enemy | Should climb |

If your focused rate is 100%, it is too easy. If your distracted rate is 5%, it is too hard - a player
also has to think about tactics.

---

## 9. Difficulty, variety, and fatigue

- **Ramp difficulty through the game** by narrowing windows slightly, adding more inputs per command,
  or making telegraphs subtler - not by removing feedback.
- **Difficulty per skill**, in data: a basic attack is easy, an ultimate is hard. This makes the
  ultimate feel special.
- **Watch for fatigue.** Every turn demands physical input, so keep normal battles to 3-5 rounds
  (see [turn-based-battle-design.md](turn-based-battle-design.md) section 6), and never place more
  than one mash command per turn cycle or three in a row.
- **Optional depth.** Advanced commands (rotate, long sequences) should be on optional powerful moves,
  so a player who dislikes them is not blocked.

---

## 10. Accessibility

Action commands are an accessibility barrier by nature. Decide this on Day 82, not on Day 161,
because retrofitting is painful.

Provide:

- **Window width multiplier** (100% / 150% / 200%). One number, applied everywhere, transforms who can
  play your game.
- **Auto-succeed at `Ok`**, so a player who cannot do timing still plays the strategic game at a
  modest penalty.
- **Convert mash to hold** - repeated rapid pressing is genuinely painful for some people, and this is
  the standard industry solution.
- **Audio *and* visual cues for every command**, so neither channel is required alone.
- **No rotate-only requirement** on any mandatory action, and a keyboard path for every gamepad input.

None of this compromises your design. The players who want a tight challenge can leave it at 100%.

---

## Exercises

1. Play (or watch frame-by-frame footage of) Paper Mario: TTYD and Super Mario RPG with a notebook.
   For three commands, write down: how much warning, how long the window looks, what sounds play, and
   what a miss costs.
2. Build the timed press and tune it until your focused success rate is around 80%. Record the final
   numbers.
3. Measure your input latency with the phone-camera method.
4. Implement the pre-window buffer, then remove it and play for five minutes. Feel the difference.
5. Mash constantly through an enemy turn and confirm it does not work.
6. Fight one enemy 20 times and plot your superguard success rate.

## Done criteria

- [ ] Every command runs on the fixed timestep, verified at 30/60/144 FPS
- [ ] Grades span a meaningful multiplier range (roughly 0.7x to 1.8x)
- [ ] Every command gives at least 200ms of warning and has a pre-window buffer
- [ ] Mashing is not a viable strategy
- [ ] Audio cues exist for every command
- [ ] Guard and superguard windows derive from the animation's contact frame
- [ ] I have measured my own latency and success rates and written them down
- [ ] Accessibility options exist and work in every command

## Sources

- Search "Paper Mario TTYD action commands frame data" - the speedrunning community has measured
  exact windows, which is the best reference material you will find
- [Super Mario RPG timed hits analysis](https://www.youtube.com/results?search_query=super+mario+rpg+timed+hits+design)
- [Juice it or lose it](https://www.youtube.com/watch?v=Fy0aCDmgnxg) - feedback layering
- Search "input buffering fighting games" - fighting game design has the deepest thinking on input
  leniency, and it all transfers
- [Game Accessibility Guidelines](https://gameaccessibilityguidelines.com/) - the timing and motor
  sections
