# Phase 05 - Battle system and action commands (Days 72-95)

This is the heart of the game and the reason you chose this genre. Twenty-four days: ten to
build a solid turn-based battle, eight to build the Paper Mario action-command layer that
makes it *interactive*, and six to make it feel good.

You already wrote a working turn-based battle on Days 21-24 in the console. That was not a
throwaway exercise - it was the design document for this phase. Reread your Day 24 code
before starting.

The one idea to keep in mind: **a turn-based battle asks the player to choose; an action
command asks the player to perform.** Paper Mario's magic is that every single turn asks
for both. No turn is ever passive.

Research notes for this phase:
[turn-based-battle-design.md](../research/turn-based-battle-design.md),
[action-commands.md](../research/action-commands.md),
[rpg-math-and-balance.md](../research/rpg-math-and-balance.md),
[status-effects.md](../research/status-effects.md),
[enemy-ai.md](../research/enemy-ai.md),
[animation-and-tweening.md](../research/animation-and-tweening.md),
[ui-systems.md](../research/ui-systems.md).

---

## Block A - Battle core (Days 72-81)

### Day 72 - Battle architecture and data

**Goal:** design the battle before building it, using what you learned in the console
version.

**New concepts:** separating persistent character data from per-battle combat state,
battle-scoped IDs, arena definitions.

**Research first (40 min):**
[turn-based-battle-design.md](../research/turn-based-battle-design.md) in full.

**Tasks**

- [ ] Reread your Day 21-24 console battle and write down three things you would do
      differently. Those three things are the whole point of this day
- [ ] Make the key separation explicit. Persistent data (`CharacterData`: name, level, XP,
      learned skills, equipment, current HP/FP) lives in the session and survives battles.
      Per-battle state (`Combatant`: battle ID, screen position, statuses, guard state,
      turn flags, animator) exists only inside the battle:

```cpp
struct Combatant {
    int              id = 0;          // battle-scoped, stable
    bool             isEnemy = false;
    CharacterData*   character = nullptr;  // non-owning; null for enemies
    const EnemyDef*  enemyDef = nullptr;   // non-owning; null for heroes
    Vector2          homePosition{};       // where they stand in the arena
    Animator         animator{};
    std::vector<ActiveStatus> statuses;
    bool             defending = false;
    bool             hasActedThisRound = false;
};
```

- [ ] Write the accessor layer: `int MaxHp(const Combatant&)`, `int Atk(const Combatant&)`,
      etc., which read from whichever source applies and then apply status modifiers. Every
      formula goes through these, never through raw fields
- [ ] Define `struct ArenaDef { std::string background, ground; std::vector<Vector2> heroSlots, enemySlots; }`
      loaded from data
- [ ] Define the phase enum: `Intro`, `RoundStart`, `TurnStart`, `CommandSelect`,
      `TargetSelect`, `Executing`, `TurnEnd`, `RoundEnd`, `Victory`, `Defeat`, `Fleeing`
- [ ] Draw the phase diagram on paper, including every transition and who triggers it. Keep
      the drawing next to your keyboard for the next three weeks
- [ ] Write it into [../design/architecture.md](../design/architecture.md) as decision D004

**Deliverable:** a written phase diagram and the data structures, with the persistent/
per-battle split explicit.

**Journal:** what would you change about your console battle, and why?

---

### Day 73 - The battle scene

**Goal:** a battle that looks like a battle, before any turns happen.

**New concepts:** arena layout, actor placement, battle camera framing, HUD composition.

**Tasks**

- [ ] Lay out the arena: heroes on the left, enemies on the right, both slightly diagonal so
      back-row actors are visible. Load slot positions from the arena def
- [ ] Draw a background, a ground plane, and every combatant as a placeholder rectangle with
      an idle bob animation (a sine offset - two lines, and the scene stops looking dead)
- [ ] Build the HUD: per-hero name, HP/MaxHP, FP, and status icons in a bottom-left panel.
      Enemy HP shown as a bar over their head, revealed only after you first damage them
      (a small design choice worth thinking about)
- [ ] Animate HP bars: when HP changes, tween the bar over 0.3s with a white "chip" trail.
      Instant bars feel cheap; this is 15 lines
- [ ] Set up the battle camera: a slight zoom-in on the acting combatant, framing both
      attacker and target during an attack
- [ ] Add a message banner at the top for "Goblin used Bite!" style text
- [ ] Spawn the real encounter from Day 68's `EncounterDef` so the scene reflects actual data

**Deliverable:** a static but attractive battle scene built from real encounter data.

**Journal:** which visual detail bought the most life for the least code?

---

### Day 74 - The turn engine

**Goal:** turns that flow correctly, forever, without special cases.

**New concepts:** phase state machines, turn order recomputation, actor death mid-round,
round bookkeeping.

**Tasks**

- [ ] Implement the phase machine from Day 72 with a single `switch` in `Update`, where each
      phase either waits for input, waits for an animation, or transitions
- [ ] Compute turn order at `RoundStart` by SPD, with deterministic tie-breaking
- [ ] Store turn order as **combatant IDs**, never indices. Then kill a combatant mid-round
      and confirm nothing shifts or double-acts. (You met this exact bug on Day 22. Notice
      that you now recognise it before it happens.)
- [ ] Skip dead and fled combatants when their turn arrives
- [ ] Handle the ugly cases explicitly and write down what you chose: an actor dies before
      acting; every enemy dies mid-round; a status kills someone at end of turn; the last
      hero falls at the same moment as the last enemy (simultaneous KO - who wins?)
- [ ] Add a round counter and expose it, since many mechanics key off "on round 3"
- [ ] Build a `Fast forward` debug key that auto-resolves turns randomly, so you can reach
      round 10 in seconds while testing

**Deliverable:** a battle that runs 30 rounds of automatic random actions without a single
glitch or crash.

**Stuck?** If the phase machine grows tangled, the usual cause is one phase doing two jobs.
Split it. Phases should be boring.

**Journal:** your answer to the simultaneous-KO question.

---

### Day 75 - The command menu

**Goal:** the interface the player touches every single turn. It must be flawless.

**New concepts:** nested menus, a menu stack, cancellation, disabled entries with reasons,
descriptions.

**Research first (25 min):** [ui-systems.md](../research/ui-systems.md) sections 5-8.

**Tasks**

- [ ] Build a reusable `class Menu`: items with a label, an enabled flag, a description, and
      a callback; cursor navigation with repeat; confirm and cancel
- [ ] Build the root battle menu: `Attack`, `Skills`, `Items`, `Guard`, `Flee`
- [ ] Nest submenus with a stack, so Cancel always reliably returns one level. Reuse your
      Day 38 stack thinking rather than writing bespoke back-navigation
- [ ] Show a description line for the highlighted item, and for skills show FP cost and
      target type
- [ ] Grey out unavailable options with a *reason* ("Not enough FP", "No items"). Never
      silently ignore a press - a menu that does nothing feels broken
- [ ] Add cursor animation (ease into position), a sound on move, a confirm sound, and a
      distinct cancel sound
- [ ] Ensure the menu is fully keyboard **and** gamepad driven through the Day 41 input layer
- [ ] Test with an empty item list, a 20-item list (scrolling), and long names

**Deliverable:** a battle menu that is pleasant to use 500 times and never confusing.

**Journal:** what did you change after using it 50 times?

---

### Day 76 - Target selection

**Goal:** choose who to hit, unambiguously.

**New concepts:** target rules as data, target sets, cycling, live previews.

**Tasks**

- [ ] Define `enum class TargetRule { OneEnemy, AllEnemies, OneAlly, AllAllies, Self, OneAny, RandomEnemy }`
      as a property of each skill and item
- [ ] Build target selection: a cursor over the current target, cycle with left/right (and
      up/down for rows), confirm, cancel back to the menu
- [ ] Highlight the whole affected set for group targets, and dim non-targets
- [ ] Exclude invalid targets automatically: dead enemies, dead allies (unless the item is a
      revive), fled combatants. Prefer excluding over rejecting after the fact
- [ ] Default the cursor sensibly: the last target you chose, or the lowest-HP ally for a
      heal. Small conveniences like this are why some games feel smooth
- [ ] Show a damage or healing preview above the target ("~14"). Decide whether you want
      this (it reduces mystery but increases tactical clarity) and record the decision
- [ ] Handle the case where every valid target dies while the menu is open

**Deliverable:** target selection covering all seven target rules with no way to pick an
invalid target.

**Journal:** did the damage preview improve the game? Argue both sides in two lines.

---

### Day 77 - The damage pipeline

**Goal:** one path that every hit flows through, so balance is tunable and bugs have one
home.

**New concepts:** pipeline design, elemental multipliers, defence, criticals, variance,
immunity and absorption, event hooks.

**Research first (30 min):** [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)
sections 4-7.

**Tasks**

- [ ] Define the pipeline explicitly as ordered stages, and implement each as its own small
      function:

```
1. base power (skill power, attacker ATK)
2. action command grade multiplier   <- the Paper Mario hook, added Day 83
3. attacker buffs and badges
4. element vs target resistance (weak 1.5x, resist 0.5x, immune 0x, absorb heals)
5. target DEF and guard/superguard reduction
6. critical roll and multiplier
7. random variance (plus or minus 10%)
8. clamp to a minimum of 1 (or 0 if fully blocked)
9. apply, then emit a DamageDealt event
```

- [ ] Return a `struct DamageResult { int amount; bool wasCritical; Effectiveness effect; bool blocked; }`
      so the presentation layer can react without recomputing anything
- [ ] Implement floating damage numbers styled by result: normal, critical (bigger, shake),
      weak ("Weak!" label), resisted (small and grey), blocked ("Blocked!"), healing (green
      with a plus)
- [ ] Add hit flash, a brief hitstop freeze (about 0.08s), and knockback on heavy hits.
      Hitstop is the single highest-impact 10 lines in the whole phase - implement it and
      toggle it on and off to feel the difference
- [ ] Build a damage calculator debug screen (F7): pick attacker, target, skill, and grade,
      and print every stage's intermediate value. You will use this constantly when balancing
- [ ] Write assertions: damage is never negative, never exceeds a sane cap, HP stays within
      range

**Deliverable:** every hit flows through one pipeline you can inspect stage by stage.

**Journal:** how much better did hitstop feel?

---

### Day 78 - Attack sequences

**Goal:** attacks that are performances, not instant number changes.

**New concepts:** sequencing battle actions, coordinating animation with logic, timing the
damage frame.

**Research first (20 min):**
[animation-and-tweening.md](../research/animation-and-tweening.md) sections 8-9.

**Tasks**

- [ ] Reuse your Day 50 `Sequence` for a basic attack: step forward, wind up, lunge, **apply
      damage on the contact frame**, hitstop, recoil, return home, end turn
- [ ] The critical rule: damage is applied at the animation's contact moment, not when the
      command was chosen. Data and presentation must agree on when the hit lands
- [ ] Build three distinct sequences: a melee lunge, a projectile (spawn, travel with an arc,
      impact), and a targeted spell (cast pose, effect at the target, delay, damage)
- [ ] Handle multi-hit: three hits with 0.12s spacing, each its own damage event, with a
      combo counter
- [ ] Handle group attacks: stagger the damage across targets by 0.06s so the numbers read
      one at a time rather than as a wall
- [ ] Make every sequence interruptible/skippable with a hold key, for testing and for
      impatient players
- [ ] Add camera work: a small zoom on the attacker, a shake on impact scaled to damage

**Deliverable:** three attack types that read clearly and land their damage on the right
frame.

**Journal:** what happens if damage is applied at the wrong point in the sequence?

---

### Day 79 - Enemy turns and telegraphing

**Goal:** enemies that act intelligently and *readably*.

**New concepts:** AI decision scoring, telegraphs, difficulty through information.

**Research first (25 min):** [enemy-ai.md](../research/enemy-ai.md) sections 6-8.

**Tasks**

- [ ] Port your Day 23 weighted AI and upgrade it to a scoring model: enumerate legal
      actions, score each (finish a low-HP hero, heal an ally below 30%, buff on round 1,
      avoid wasting an AoE on one target), then pick the best with a little randomness so it
      is not perfectly predictable
- [ ] Define AI profiles in data: `aggressive`, `defensive`, `support`, `erratic`, `boss`.
      Assign profiles to enemy defs so new enemies need no code
- [ ] Add telegraphs: the enemy strikes a windup pose and an icon shows what is coming next
      round. **This is what makes defensive action commands fair** - the player must be able
      to prepare
- [ ] Add a short pause before the enemy acts, so the player can read the telegraph. Tune it:
      too fast is unfair, too slow is boring
- [ ] Give each enemy a distinct, mechanically meaningful signature move
- [ ] Add an AI debug view showing every scored option, so you can tell "bad AI" from "bad
      scoring"

**Deliverable:** enemies whose next action you can predict from what you see on screen.

**Journal:** how did telegraphing change the difficulty?

---

### Day 80 - Statuses and buffs

**Goal:** the layer that turns combat into a system rather than a damage race.

**New concepts:** status stacking rules, duration semantics, application timing, visual
communication.

**Research first (30 min):** [status-effects.md](../research/status-effects.md) sections 4-8.

**Tasks**

- [ ] Implement `struct ActiveStatus { StatusId id; int turnsLeft; int magnitude; int sourceId; };`
      and a status registry loaded from data (name, icon, tick behaviour, stacking rule,
      whether it blocks actions)
- [ ] Implement at least eight: `Poison`, `Burn`, `Sleep`, `Stun`, `Slow`, `AttackUp`,
      `DefenseUp`, `Regen`
- [ ] Decide and document each one's rules: does reapplying refresh, extend, or stack? Does
      it tick at the actor's turn end or the round end? Does damage wake sleep? Writing this
      table down is 80% of the work; the code is easy afterwards
- [ ] Route stat modifiers through the Day 72 accessors so no formula bypasses them
- [ ] Handle action-blocking statuses in the turn engine (a stunned actor's turn is skipped
      with a visible message, not silently)
- [ ] Present them clearly: icons with turn counters under each combatant, a pop-in animation
      on application, a flash on expiry, a distinct sound per category
- [ ] Test the nasty interactions: poison killing an actor whose turn it is, sleep plus stun
      together, a buff expiring mid-sequence, a status source dying first

**Deliverable:** eight statuses with documented rules that survive adversarial testing.

**Journal:** the rules table. Keep it in the design doc, not just in code.

---

### Day 81 - Ending the battle

**Goal:** close the loop with rewards that feel earned.

**New concepts:** outcome handling, reward calculation, level-up flow, escape rules.

**Tasks**

- [ ] Victory: a victory pose and jingle, then a results panel listing XP, gold, and item
      drops appearing one line at a time
- [ ] Apply XP to persistent `CharacterData`, handle multiple level-ups in one go, and show
      stat gains with the old and new values
- [ ] Item drops with per-enemy drop chances from data
- [ ] Defeat: a defeat animation, fade, and the game over flow you stubbed on Day 69
- [ ] Flee: a success chance based on SPD difference, a failed-flee message that costs the
      turn, and a guaranteed escape after several failures so a player can never be trapped
- [ ] Return to the field via Day 69's `BattleResult`, with everything applied
- [ ] Run the full field-battle-field loop 20 times, watching for leaks, stale event
      subscriptions, and music that fails to resume

**Deliverable:** a complete battle, winnable and losable, with rewards flowing into
persistent data.

**Journal:** does winning feel good yet? If not, what is missing - sound, timing, or reward?

---

## Block B - Action commands (Days 82-89)

Eight days on the mechanic that defines your game. Everything before this was a competent
JRPG; this is what makes it *yours*.

Before Day 82, spend real time playing (or watching) Paper Mario: The Thousand-Year Door and
Super Mario RPG with a notebook. Count frames. Note exactly when prompts appear, how long
windows last, what sound plays, what happens on a miss. You are reverse-engineering feel,
and feel is measurable.

### Day 82 - The action command framework

**Goal:** one abstraction that every command type plugs into, designed before any concrete
command exists.

**New concepts:** interface design under uncertainty, grading, lifecycle, timing precision.

**Research first (45 min):** [action-commands.md](../research/action-commands.md) in full.
This is the most important research note in the roadmap.

**Tasks**

- [ ] Design the interface. Aim for something close to this, but derive it yourself from the
      list of commands you intend to build:

```cpp
enum class Grade { Miss, Ok, Good, Great, Perfect };

struct CommandResult {
    Grade grade = Grade::Miss;
    float multiplier = 1.0f;   // fed into damage pipeline stage 2
    int   hits = 1;            // for mash/multi commands
    bool  bonusTriggered = false;
};

class ActionCommand {
public:
    virtual ~ActionCommand() = default;
    virtual void Start(const CommandContext&) = 0;
    virtual void Update(float dt, const InputMap&) = 0;
    virtual void Draw() const = 0;              // prompts, gauges, markers
    virtual bool Finished() const = 0;
    virtual CommandResult Result() const = 0;
};
```

- [ ] Decide the timing basis and write down why. Action commands should run on the **fixed
      timestep** so a window is the same length on every machine, while their *drawing*
      interpolates per frame
- [ ] Define the grade-to-multiplier table in **data**, not code, because you will retune it
      dozens of times
- [ ] Build a test harness state (F8) that runs any command in isolation with no battle
      around it, printing timing data: window start, window end, actual press time, error in
      milliseconds. Without this harness, tuning is guesswork
- [ ] Decide how a command integrates with a Day 78 sequence: the sequence pauses at the
      windup, the command runs, then the sequence continues with the resulting multiplier
- [ ] Write down the accessibility plan now: an option to widen all windows, an option to
      auto-succeed at `Ok`. Deciding this now costs nothing; retrofitting it later is painful

**Deliverable:** the framework plus an isolated test harness reporting millisecond accuracy.

**Journal:** why fixed timestep for the logic and per-frame for the drawing?

---

### Day 83 - The timed press

**Goal:** the bread-and-butter command: press at the right moment.

**New concepts:** timing windows, leniency, input buffering, early-press punishment,
readable prompts.

**Tasks**

- [ ] Implement `TimedPressCommand`: a marker sweeps across a bar; a target zone sits
      somewhere along it; the player presses Confirm inside the zone
- [ ] Grade by distance from the zone centre: `Perfect` within 40ms, `Great` within 90ms,
      `Ok` within 150ms, otherwise `Miss`. These are starting numbers - your job is to tune
      them by feel
- [ ] Decide what an early press does. Punishing early presses (an immediate miss) rewards
      precision; ignoring them rewards mashing. Paper Mario punishes. Pick one, document it
- [ ] Add a pre-window input buffer of about 80ms, so a press landing a hair early still
      counts. This one number is the difference between "tight" and "unfair"
- [ ] Present it well: the marker, the zone, a flash on hit, a grade label popping with
      `EaseOutBack`, distinct sounds per grade, and a subtle rising tick as the marker
      approaches so the player can time it by ear as well as by eye
- [ ] Wire it into the basic attack and feel the whole battle change
- [ ] Tune with your harness until you can hit `Great` about 80% of the time when
      concentrating, and about 40% when distracted. Record the final numbers in the design doc

**Deliverable:** a timed press that feels tight and fair, tuned with real measurements.

**Stuck?** If it feels unfair, the cause is almost always missing input buffering or a
prompt that appears too late for a human to react (under about 200ms of warning).

**Journal:** your final window numbers and buffer size.

---

### Day 84 - The mash command

**Goal:** press rapidly, with a fair ceiling.

**New concepts:** press counting, rate limiting, physical fairness, gauge feedback.

**Tasks**

- [ ] Implement `MashCommand`: count Confirm presses within a fixed duration (about 2s) and
      map the count to a multiplier
- [ ] Set a realistic ceiling. Humans manage roughly 6-9 presses per second, so a 2s window
      caps around 14-18. The maximum reward must be reachable without hurting anyone's hand -
      cap the *benefit*, not the presses
- [ ] Cheat-proof it: ignore presses faster than about 25ms apart (autofire controllers), and
      require alternating input for the harder variant
- [ ] Feedback matters more here than anywhere: a filling gauge, a growing character
      animation, escalating sound pitch, screen shake building. Mashing with weak feedback
      feels like a chore; with strong feedback it is thrilling
- [ ] Add threshold markers on the gauge so the player can see the next reward tier
- [ ] Implement the alternating variant (Confirm and Cancel alternately) and note how
      different it feels
- [ ] Consider your players' hands: no more than one mash command per turn cycle, and never
      three in a row. Write the rule in the design doc

**Deliverable:** a mash command with a fair ceiling, cheat protection, and great feedback.

**Journal:** what was your measured maximum press rate, and where did you set the cap?

---

### Day 85 - Hold, release, and rotate

**Goal:** two more input verbs, so commands stay varied.

**New concepts:** hold duration measurement, overcharge, analogue stick angle tracking,
keyboard fallbacks for analogue input.

**Tasks**

- [ ] Implement `HoldReleaseCommand`: hold Confirm to charge, release inside a target zone.
      Overcharging past the zone must fail, so it is a real risk
- [ ] Show the charge as a filling meter with a marked release zone, plus a rising audio pitch
      that peaks at the zone - many players will do this entirely by ear
- [ ] Implement `RotateCommand`: track the left stick angle, accumulate total rotation,
      require N full turns. Handle the angle wraparound at 180 degrees correctly, and ignore
      reversals so scrubbing back and forth does not count
- [ ] Provide a keyboard fallback for rotation (alternating left/right arrows) and make sure
      both paths are equally achievable. Never ship a command that only works on a gamepad
- [ ] Implement `SequenceCommand`: a sequence of 3-5 directional or button prompts to hit in
      order under time pressure, with per-input grading
- [ ] Test every command on keyboard and gamepad. Anything that feels bad on one of them is
      not finished
- [ ] Add each to your harness and record the difficulty you actually achieve

**Deliverable:** four command types working equally well on keyboard and gamepad.

**Journal:** which command type was hardest to make feel good, and why?

---

### Day 86 - Defensive commands

**Goal:** the player is never passive, even on the enemy's turn. This is the other half of
Paper Mario's design and the half most imitators skip.

**New concepts:** reactive timing, telegraph-to-window coupling, risk/reward tiers,
counterattacks.

**Tasks**

- [ ] Implement `Guard`: press Confirm within about 120ms before the hit lands, reducing
      damage (say -1 flat and 50%). Forgiving, always available
- [ ] Implement `Superguard`: press Cancel within a tighter window of about 60ms, blocking
      all damage and dealing recoil to a melee attacker. High risk, high reward
- [ ] Couple the window to the animation: the hit's contact frame from Day 78 defines the
      window centre. Both must come from one source of truth, or the game will feel like it
      lies to the player
- [ ] Make the telegraph honest: a wind-up animation of consistent length per enemy, plus an
      optional audio cue. If the player cannot learn the timing, the mechanic is noise
- [ ] Punish mistimed presses mildly (a brief inability to retry) so mashing is not a viable
      strategy. Test by mashing throughout an enemy turn - if that works, the design failed
- [ ] Add feedback: a guard flash and clang, a superguard sparkle and a satisfyingly distinct
      sound, plus a small time-slow on a successful superguard
- [ ] Handle multi-hit attacks: one window per hit, each independently guardable
- [ ] Playtest: fight an enemy 20 times and check that your success rate improves as you learn
      the tells. Improvement over repetition is exactly the sign of a good mechanic

**Deliverable:** guard and superguard tied honestly to enemy animations, learnable through
practice.

**Journal:** did your success rate improve over 20 fights? By how much?

---

### Day 87 - Feel and tuning

**Goal:** turn functional commands into commands that feel *good*. Budget the whole day for
this; it is not padding.

**New concepts:** game feel, juice, perceptual latency, coyote time, difficulty curves.

**Research first (25 min):** [action-commands.md](../research/action-commands.md)
sections 8-10 (feel and fairness).

**Tasks**

- [ ] Measure your input latency: on a press, flash the whole screen white for one frame and
      film it with a phone at 60fps or higher. Count the frames between your finger and the
      flash. Anything above about 3 frames needs investigating
- [ ] Audit every command for the three fairness rules: at least 200ms of warning before a
      window, a pre-window buffer, and consistent timing regardless of frame rate
- [ ] Add anticipation to every prompt: a prompt that pops in with a scale bounce reads far
      faster than one that simply appears
- [ ] Add a "just barely missed" response distinct from a total miss, so near-misses teach
      instead of only punishing
- [ ] Layer the audio: a warning tick, a hit sound per grade, a whiff sound. Audio timing
      cues are more precise than visual ones for most players, so lean on them
- [ ] Implement the accessibility options you promised on Day 82 (wider windows,
      auto-succeed) and confirm they work in every command
- [ ] Get someone else to play. Watch which command they fail repeatedly and fix that one
- [ ] Record final tuning numbers for every command in the design doc

**Deliverable:** commands that a first-time player can succeed at, and an expert can master.

**Journal:** what was your measured input latency?

---

### Day 88 - The economy: FP, badges, and star power

**Goal:** the resource layer that makes action commands into decisions, not just reflex tests.

**New concepts:** resource economies, build customisation, cost curves, meaningful choice.

**Tasks**

- [ ] Implement FP (skill points): skills cost FP, FP restores partially after battle and
      fully at rest points. Tune so a normal battle uses roughly 30-50% of your FP, making
      dungeon-level attrition a real consideration
- [ ] Implement badges: equippable modifiers with a shared capacity cost, so the player
      builds a loadout rather than equipping everything. Examples: `+2 ATK`,
      `Perfect grades restore 1 FP`, `Superguard window +30%`,
      `First strike deals double damage`, `Poison on hit`, `Half FP costs, half damage`
- [ ] Make at least three badges interact directly with action commands, so mastering the
      timing unlocks build options
- [ ] Implement star power: a meter that fills on `Great` and `Perfect` grades and on
      successful superguards, spent on powerful moves with their own elaborate commands.
      Skill directly funds spectacle - that is the loop you want
- [ ] Build the badge equip UI with capacity display and conflict warnings
- [ ] Balance-test three different builds against the same encounter and confirm all three
      are viable but distinct. If one build dominates, the numbers are wrong
- [ ] Record every badge and its numbers in the design doc

**Deliverable:** an economy where the player's build and their execution both matter.

**Journal:** which badge combination broke the game, and how did you fix it?

---

### Day 89 - Full integration

**Goal:** every action in the game has an interaction, and it all holds together.

**Tasks**

- [ ] Attach a command to every offensive skill and every item that should have one, all
      configured in data (`"command": "timed_press", "difficulty": 2`)
- [ ] Attach defensive commands to every enemy attack
- [ ] Balance-pass the multipliers: succeeding should feel strong without making failing
      hopeless. A reasonable target is `Miss` 0.7x to `Perfect` 1.8x
- [ ] Run 20 real battles. Note every command that felt annoying, unclear, or repetitive, and
      fix the top three
- [ ] Verify the accessibility options across everything
- [ ] Confirm command timing is unaffected at 30, 60 and 144 FPS
- [ ] Commit: `day 89: action commands complete`

**Deliverable:** a battle system where no turn is passive, for either side.

**Journal:** describe how a full turn now plays out, start to finish. If it takes more than
six lines, the turn may be too slow.

---

## Block C - Battle depth and polish (Days 90-95)

### Day 90 - Enemy mechanics and counterplay

**Goal:** enemies that demand different tactics, not just bigger numbers.

**New concepts:** attack typing, mechanical counterplay, encounter composition.

**Tasks**

- [ ] Implement attack *types* as data: `Ground`, `Aerial`, `Ranged`, `Magic`, and give
      enemies properties that interact with them - flying enemies immune to ground attacks,
      spiked enemies that damage jump attackers, armoured enemies needing defence-piercing
      or armour-break, shielded enemies whose shield must be removed first
- [ ] Add a scan/tattle action that reveals HP, weaknesses, and a description. Make it fun to
      read; in Paper Mario the tattle is a beloved feature, not a chore
- [ ] Build eight enemies whose mechanics each demand a specific answer, and write the answer
      in the design doc so you can verify the player has access to it when they meet it
- [ ] Compose encounters that force switching: a flyer plus a spiked ground enemy cannot be
      handled with one move type
- [ ] Add enemy behaviour that reacts to the player: enemies that flee at low HP, call
      reinforcements, or enrage when their ally dies
- [ ] Test each enemy alone to confirm its mechanic is legible without explanation

**Deliverable:** eight mechanically distinct enemies, each with a documented counter.

**Journal:** which enemy taught the player something? That is the one to imitate.

---

### Day 91 - The boss fight

**Goal:** a multi-phase fight that examines everything the player has learned.

**New concepts:** phase transitions, scripted beats, arena hazards, difficulty pacing.

**Research first (20 min):** [jrpg-game-design.md](../research/jrpg-game-design.md)
sections 5-6 (boss design).

**Tasks**

- [ ] Build a boss with three HP-threshold phases, each changing its move set and telegraphs
- [ ] Add a signature gimmick that requires a specific mechanic: a shield only breakable by
      superguard counters, a minion it keeps summoning, an arena hazard that shifts, a charge
      attack that must be interrupted
- [ ] Add scripted dialogue at phase transitions using your Day 60 dialogue commands - the
      systems compose, and that is the payoff for building them properly
- [ ] Give it a unique, harder action command for its ultimate attack
- [ ] Make it survivable: a telegraphed lethal move the player can prepare for, and no
      unavoidable damage spikes that can kill from full HP
- [ ] Tune the length to 3-5 minutes. Longer bosses need a mid-fight save or checkpoint
- [ ] Add the presentation: a boss intro, unique music, a health bar with phase pips, and a
      dramatic defeat sequence
- [ ] Beat it five times at different skill levels (play badly on purpose once)

**Deliverable:** a boss fight you are proud of, beatable in 3-5 minutes.

**Journal:** what made the phase transitions feel dramatic rather than arbitrary?

---

### Day 92 - Battle UI polish

**Goal:** the player always knows what is happening and what is coming.

**Tasks**

- [ ] Add a turn order indicator showing the next 4-6 turns, updating live when SPD changes
- [ ] Add status icon tooltips (hold a key to inspect) and an enemy inspect view
- [ ] Add a combat log (toggle with Tab) listing the last 10 actions with amounts, invaluable
      for both players and your own debugging
- [ ] Add a battle speed setting (1x, 1.5x, 2x) that scales animations but **not** action
      command windows. Verify that carefully - it is a subtle bug waiting to happen
- [ ] Add a "skip animations" option for repeat encounters
- [ ] Ensure everything is readable at your internal resolution: no text under 8px, adequate
      contrast, no critical information conveyed by colour alone
- [ ] Test the whole battle UI with only a gamepad, then with only a keyboard

**Deliverable:** a battle UI that answers every question the player might have.

**Journal:** what did the combat log reveal about your own systems?

---

### Day 93 - Audio and visual effects

**Goal:** impact. The difference between a prototype and a game.

**Research first (20 min):**
[particles-and-vfx.md](../research/particles-and-vfx.md) sections 1-4.

**Tasks**

- [ ] Build a small particle system: emitters with count, lifetime, velocity spread, gravity,
      colour ramp, and size curve. Keep it under 150 lines and pool the particles
- [ ] Add effects for: hit sparks, critical bursts, healing sparkles, level-up rays, status
      application puffs, dust on landing
- [ ] Add screen effects: a white flash on criticals, a red vignette at low HP, a radial blur
      or zoom punch on big hits
- [ ] Build the sound layer properly: distinct sounds per grade, per element, per status, per
      menu action, plus enemy-specific hit sounds. Randomise pitch on everything repetitive
- [ ] Add battle music with an intro flourish, plus separate boss music, and a low-HP tension
      layer if you are feeling ambitious
- [ ] Mix the audio: set relative volumes so nothing drowns out the action command cues. The
      command cues are gameplay-critical and must sit on top
- [ ] Compare a recording of today's battle with one from Day 81. The gap is your entire
      motivation for polish work

**Deliverable:** a battle that sounds and feels like a real game.

**Journal:** watch the Day 81 and Day 93 recordings back to back. Write what changed.

---

### Day 94 - Balance harness

**Goal:** balance with data instead of vibes.

**Research first (20 min):** [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)
sections 8-10.

**Tasks**

- [ ] Build a headless simulation mode: run a battle with no rendering, with scripted AI on
      both sides and a configurable action command success profile (`bad` 20% success,
      `average` 60%, `expert` 95%)
- [ ] Run 1000 battles per encounter per profile and report win rate, average rounds, average
      HP remaining, and average FP spent
- [ ] Set targets and tune to them: a normal encounter should be about 95% win rate for an
      average player in 3-5 rounds; a boss about 60-75% on the first attempt
- [ ] Check that skill matters: an expert profile should clear a fight in noticeably fewer
      rounds than a bad one. If skill does not move the numbers, your multipliers are too
      timid and the action commands are decorative
- [ ] Find the degenerate strategies: is one skill always optimal? Is an item spam loop
      unbeatable? The simulator finds these far faster than playtesting
- [ ] Export results to CSV and look at them in a spreadsheet, plotting damage against level
      to check your curves
- [ ] Write the target numbers into the design doc so future encounters have a standard

**Deliverable:** a simulator, a CSV of results, and encounters tuned to written targets.

**Journal:** which encounter was most out of balance, and by how much?

---

### Day 95 - Phase consolidation

**Goal:** the full game loop, end to end, solid.

**Tasks**

- [ ] Play from the title through the overworld, three normal battles, and the boss, then
      back to the field. Fix everything that breaks
- [ ] Run the sanitizer build through a full session
- [ ] Check for leaks across 30 battles: memory, event subscriptions, sounds, textures
- [ ] Confirm the persistent/per-battle data split is still honest - no battle code writing
      to `CharacterData` outside the rewards step
- [ ] Update [../design/architecture.md](../design/architecture.md) with the battle system
      diagram, and fill in the design doc's battle section completely
- [ ] Have someone else play a battle without instructions. Watch what they do not understand
- [ ] Record a two-minute video of a battle. This is your first real proof of progress -
      compare it with your Day 36 rectangle demo
- [ ] Commit: `day 95: battle system complete`

**Deliverable:** a complete, polished, balanced battle system integrated with the overworld.

---

## Phase 05 exit checklist

- [ ] Persistent character data and per-battle state are cleanly separated
- [ ] The phase machine runs 30 rounds without a glitch, using IDs not indices
- [ ] The command menu is nested, cancellable, and pleasant after 500 uses
- [ ] All seven target rules work and invalid targets cannot be chosen
- [ ] Every hit goes through one inspectable damage pipeline
- [ ] Damage lands on the animation's contact frame, with hitstop
- [ ] Enemies telegraph their next action readably
- [ ] Eight statuses exist with documented stacking and timing rules
- [ ] Four offensive command types plus guard and superguard all feel good
- [ ] Command timing is identical at 30, 60, and 144 FPS
- [ ] Accessibility options for window width and auto-succeed work everywhere
- [ ] FP, badges, and star power make builds meaningfully different
- [ ] A boss with three phases and a gimmick is beatable in 3-5 minutes
- [ ] The simulator confirms that skill changes outcomes measurably

