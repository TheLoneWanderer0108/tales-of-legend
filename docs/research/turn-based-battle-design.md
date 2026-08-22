# Turn-based battle design

**Used on:** Day 21 (console battle design), Day 72 (battle architecture), Day 74 (turn engine).

Why this matters: the battle system is where players spend most of their time and where your game's
identity lives. The *architecture* decisions here (what a turn is, who owns what, when things tick)
are difficult to change later because everything else attaches to them. Two days of thinking now
saves two weeks of restructuring in month five.

---

## 1. Turn order models

Choose one and write down why.

**Round-based, sorted by speed.** Everyone acts once per round, in speed order. Rounds are a clean
unit for "3 turns of poison" and for AI planning.
Pros: simple, predictable, easy to display, easy to balance. Cons: speed is a weak stat (it only
changes *when* you act, not *how often*).

**Individual initiative / ATB (active time battle).** Each combatant fills a gauge at a rate set by
speed; when it fills, they act. Fast characters act more often.
Pros: speed becomes genuinely valuable, and haste/slow effects are exciting. Cons: "3 turns" becomes
ambiguous, turn order display is harder, and balance is trickier.

**Fixed order.** Player party, then enemies, in a set order.
Pros: trivially predictable, which suits a puzzle-like design. Cons: speed does nothing.

**Recommendation for this project: round-based sorted by speed, recomputed each round.** Recomputing
per round (rather than fixing at battle start) is what lets haste and slow effects matter. It also
keeps status durations unambiguous.

**Questions to answer**

- If a haste effect doubles speed, what should it actually do in your model?
- Should the player see the upcoming turn order? (Yes - it turns guessing into planning.)
- What happens when two combatants have identical speed? (Tie-break deterministically, always.)

---

## 2. What a "turn" consists of

Break it into explicit phases, because ambiguity here creates the bugs. A defensible sequence:

```
TurnStart      -> tick "start of turn" effects, check if the actor can act at all
CommandSelect  -> player chooses (or AI decides)
TargetSelect   -> choose targets, validate
Executing      -> animation runs, action command runs, effects apply
TurnEnd        -> tick "end of turn" effects (poison, buff expiry)
```

Then `RoundEnd` handles anything that happens once per round rather than per actor.

**The important decision: when do statuses tick?** At the afflicted actor's turn end, or at round
end? Both are valid and produce different games. Turn-end ticking means a poisoned actor takes damage
before your next action; round-end ticking is easier to reason about. Choose, document it, and be
consistent - inconsistency here produces bugs that look like balance problems.

---

## 3. Actor data: the split that matters most

The single most important architectural decision in the battle system:

- **Persistent character data** (level, XP, learned skills, equipment, badges, current HP/FP) belongs
  to the session and **survives the battle**.
- **Per-battle combat state** (screen position, statuses, guard state, turn flags, animator, battle
  ID) exists only inside the battle.

A `Combatant` therefore *references* character data rather than copying it. Copying leads to the
classic bug where a battle's HP changes are lost, or applied twice, or the party is silently reset.

Enemies are the mirror case: they reference an immutable `EnemyDef` from your data tables, and all
their mutable state is per-battle.

Then build an **accessor layer**: `Atk(combatant)` combines base stats, equipment, badges, and status
modifiers in a documented order. Every formula reads through the accessors, never raw fields.
Without this, you will eventually find a formula that forgot to apply buffs, and it will take an hour
to find.

---

## 4. Identity within a battle

Use battle-scoped **IDs**, not indices into a vector. Turn order lists, target selections, status
sources, and pending actions all outlive a single frame, and any of them can be invalidated by a
death.

This is the same lesson as [entity-architecture.md](entity-architecture.md) section 7, and the bug
manifests in a distinctive way: actors acting twice, or being skipped, or an attack landing on the
wrong target after a death. If you see that, check for stale indices.

---

## 5. The awkward cases (decide these before coding)

Write your answer to each in the design doc. These are where turn engines break:

| Case | Your decision |
| --- | --- |
| An actor dies before their turn arrives | Skip; do not consume a turn slot |
| A status kills an actor at end of turn | Battle-end check must run after status ticks |
| All enemies die mid-round | End immediately; do not finish the round |
| Player and last enemy die simultaneously | Pick one - most games let the player win |
| An actor is stunned when their turn arrives | Show a message and skip; never skip silently |
| A target dies while the target menu is open | Re-validate and move the cursor |
| A summoned enemy joins mid-round | Acts next round, not this one |
| A multi-hit attack kills the target on hit 2 of 5 | Stop, or redirect - decide, and be consistent |

The temptation is to handle these as they arise, which produces a turn engine full of special cases.
Deciding first produces one that is boring - which is the goal.

---

## 6. Action economy and pacing

A turn-based battle is an exchange of *actions*. Balance is largely about action economy:

- An attack that hits all enemies is worth roughly N single attacks; price it accordingly.
- A status that skips an enemy's turn removes an action from their side - which is why sleep and stun
  are always strong and always need short durations or low reliability.
- Healing is only worth it if it exceeds incoming damage per turn; otherwise the player is losing the
  exchange while feeling productive.
- A buff costs an action now to gain value later, so it is only good in long fights. That is a design
  lever, not a flaw.

**Battle length targets:** normal encounters 3-5 rounds (30-60 seconds), bosses 8-15 rounds (3-5
minutes). A normal battle taking 10 rounds will exhaust the player long before the dungeon does.

Because your game has action commands, keep normal battles *short*. Every turn demands physical
input, so a long battle is genuinely more tiring than in a traditional JRPG. This is why Paper Mario
encounters are brief.

---

## 7. Making choices meaningful

A battle system where "attack the weakest enemy" is always optimal is a battle system with no
decisions. Sources of genuine decisions:

- **Resource spending** (FP now vs FP for the boss later) - attrition creates decisions across a
  whole dungeon, not just one fight.
- **Target priority** (kill the healer or the damage dealer first).
- **Type matching** (a flying enemy needs a specific attack).
- **Risk and reward** (superguard's tight window for a big payoff).
- **Turn investment** (buff first, or start damaging now).

The test: for each turn, are there at least two plausible options whose relative value depends on the
situation? If not, add one of the levers above rather than adding more numbers.

---

## 8. Presentation is part of the system

In a turn-based game, the player waits. Respect that:

- Never make the player wait for an animation they have seen 100 times without a skip option.
- Show the results clearly: floating numbers, HP bars that animate, status icons that pop.
- Keep the message banner ("Goblin used Bite!") short and let animation carry the drama.
- Stagger multi-target damage slightly so the numbers read one at a time.
- Add a battle speed setting, but **never** scale action command windows with it (Day 92).

---

## 9. Learning from the genre

Study these specifically, and take notes on the *system*, not the aesthetics:

- **Paper Mario: TTYD** - action commands, badge builds, low HP numbers (which make every point of
  damage legible), audience mechanic. Your primary reference.
- **Super Mario RPG** - the original timed hits; note how forgiving they are.
- **Final Fantasy X** - the CTB turn order display is the clearest ever made; steal it.
- **Undertale** - defensive play as the core interaction, and battles as characterisation.
- **Darkest Dungeon** - position and stress as resources; extremely legible turn economy.

For each, write down: what is the turn unit, what makes turns interesting, and what would you steal?

---

## 10. Testing a battle system

- **Headless simulation** (Day 94) - run thousands of battles with scripted AI and measure win rates.
  This finds balance problems and degenerate strategies far faster than playing.
- **Determinism via seeds** - a `--seed N` argument makes any battle exactly reproducible, which
  turns "it happened once" into a debuggable case.
- **Assertions** - HP within range, damage non-negative, no actor acting twice per round, statuses
  expiring exactly on schedule.
- **Adversarial play** - deliberately do stupid things: use a revive on a living ally, target a dying
  enemy with a 5-hit combo, stack the same buff four times.

---

## Exercises

1. Write your battle phase diagram on paper, with every transition, before coding Day 74.
2. Fill in the table in section 5 with your own decisions, in the design doc.
3. For three JRPGs you know, write down the turn model and one thing you would steal.
4. Take one of your planned encounters and write, for each round, what the interesting decision is.
   If you cannot, the encounter needs redesigning.
5. After Day 81, run 30 rounds of automatic random actions and confirm nothing breaks.

## Done criteria

- [ ] My turn model is a written decision with reasons
- [ ] Persistent character data and per-battle state are separate, and the battle does not own the
      party
- [ ] All formulas read stats through one accessor layer
- [ ] Turn order uses IDs, not indices
- [ ] Every case in section 5 has a written answer
- [ ] Normal battles last 3-5 rounds and bosses 8-15
- [ ] Every turn offers at least two plausible options

## Sources

- [Paper Mario: TTYD battle system breakdowns](https://www.youtube.com/results?search_query=paper+mario+ttyd+battle+system+design) -
  watch a design analysis, not a let's play
- [Game Programming Patterns: Command](https://gameprogrammingpatterns.com/command.html) - directly
  applicable to representing actions
- [Designing RPG combat systems](https://www.gamedeveloper.com/design) - search the site for
  turn-based combat design articles
- Your own Day 21-24 console battle. Reread it before Day 72; it is your best reference.
