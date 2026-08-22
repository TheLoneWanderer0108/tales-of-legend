# Status effects and buffs

**Used on:** Day 23 (console battle statuses), Day 80 (full status system), Day 98 (equipment
resistances).

Why this matters: statuses are what turn a damage race into a system with tactics. They are also the
single most bug-prone part of a battle system, because they involve state that persists across turns,
modifies other systems' calculations, and has to expire correctly. Almost every status bug comes from
not having decided the rules precisely.

The work here is 80% writing a rules table and 20% code. Do them in that order.

---

## 1. What a status is

A temporary modification to a combatant, with a duration. Three broad categories:

- **Damage over time**: poison, burn, bleed. Ticks damage periodically.
- **Stat modifiers**: attack up, defence down, haste, slow. Change the numbers.
- **Action restrictions**: sleep, stun, confusion, silence. Change what the actor may do.

The data structure is small:

```cpp
struct ActiveStatus {
    StatusId id;
    int turnsLeft;
    int magnitude;    // damage per tick, or stat delta
    int sourceId;     // who applied it (for "poison from X" and revenge effects)
};
```

Combatants hold `std::vector<ActiveStatus>`. A registry loaded from data defines each status's name,
icon, tick behaviour, stacking rule, and whether it blocks actions.

**Questions to answer**

- Why store the source of a status?
- Should the magnitude scale with the applier's stats, or be fixed by the skill?
- What happens to statuses at the end of a battle? (Usually cleared; decide, and be consistent.)

---

## 2. Duration semantics: decide precisely

"Poison for 3 turns" is ambiguous, and the ambiguity causes bugs. Pin down:

- **Whose turns?** The afflicted actor's turns, or rounds? Rounds are simpler; actor turns are fairer
  in a speed-based system.
- **When does it tick?** At the start or end of the afflicted actor's turn, or at round end?
- **Does the applying turn count?** If you poison an enemy on round 1, does it tick on round 1?
- **When is it decremented** relative to when it ticks? Tick then decrement, or decrement then tick?
  This determines whether "3 turns" means 3 ticks or 2.

Recommendation for this project: **statuses tick and decrement at the end of the afflicted actor's
turn, and the turn on which they are applied does not tick.** So a 3-turn poison deals damage exactly
3 times. Write your version in the design doc and add an assertion that verifies the tick count.

---

## 3. Application: reliability

Three models:

- **Always applies.** Predictable, and makes statuses reliably strong - so they must be balanced by
  cost or duration.
- **Chance-based** (say 60%). Traditional, but frustrating: spending a turn and a resource to get
  nothing feels awful.
- **Resistance-based.** Enemy resistances (immune, resistant, vulnerable) as data. Predictable *and*
  interesting, because it rewards knowledge.

Recommendation: **always applies, modified by target resistance.** It fits a game where the player
just performed a precise timed input - failing on a dice roll after succeeding at the mechanic feels
like a betrayal (the same reasoning as omitting accuracy/evasion in
[rpg-math-and-balance.md](rpg-math-and-balance.md) section 3).

Bosses should resist or shorten the strongest statuses rather than being flatly immune, so the player's
tools still do something.

---

## 4. Stacking rules

For each status, decide which one applies. This is the table that prevents most status bugs:

| Rule | Behaviour | Suits |
| --- | --- | --- |
| **Refresh** | Reapplying resets the duration | Poison, burn |
| **Extend** | Durations add up | Rarely; can be abused |
| **Stack magnitude** | Two applications = double effect, with a cap | Attack up (cap at 2-3 stacks) |
| **Independent** | Multiple instances tick separately | Almost never - a bookkeeping nightmare |
| **Highest wins** | The strongest instance applies | Defence buffs from different sources |
| **Exclusive** | A new one replaces the old | Sleep vs stun |

Also decide **opposition**: does attack-up cancel attack-down, or do they coexist and sum? Cancelling
is more intuitive; summing is simpler to implement. Either is fine if documented.

Without stacking rules, a player will stack the same buff four times and trivialise your boss. Test
exactly that.

---

## 5. Stat modifiers and the accessor layer

Every stat-modifying status must flow through the accessor layer from
[turn-based-battle-design.md](turn-based-battle-design.md) section 3:

```
Atk(combatant) = base + equipment + badges + statusModifiers
```

If any formula reads a raw stat field, buffs will silently fail to apply in that one place, and the bug
will take an hour to find because everything *looks* right.

Two details:

- **Order and form.** Are buffs additive (+5 ATK) or multiplicative (+50%)? Multiplicative scales with
  the game; additive is more predictable with small numbers. With small numbers, additive is usually
  better - +2 ATK on a base of 5 is dramatic and legible.
- **Max HP changes.** A status raising max HP must decide what happens to current HP on expiry. Clamp
  it, and never let current exceed max.

---

## 6. Action restrictions

These are the strongest statuses in any turn-based game, because removing a turn removes an entire
action from the enemy's economy.

- **Sleep**: skips turns; conventionally cancelled by taking damage. That interaction makes it
  tactical rather than simply strong.
- **Stun / paralysis**: skips turns, not cancelled by damage. Keep durations short (1-2 turns) or
  bosses become trivial.
- **Confusion**: acts randomly, possibly hitting allies. Fun, and needs careful AI handling.
- **Silence**: skills blocked, basic attacks allowed. A softer restriction.

Implementation notes: the turn engine must check restrictions at `TurnStart` and **show a message**
when skipping. A turn that silently does nothing looks like a bug to the player.

Balance rule: stun-lock must be impossible. Either add diminishing returns (each reapplication is
shorter) or a brief immunity after recovery. Test by trying to stun-lock your own boss - if you can,
your players will.

---

## 7. Communicating statuses

A status the player cannot see is a bug from their perspective.

- **Icons with turn counters** under each combatant. Number *and* icon.
- **Distinct shapes**, not just colours, so colour-blind players can read them (Day 161).
- **Pop-in animation on application** and a **flash on expiry** - expiry especially, since a buff
  running out unnoticed feels like a betrayal.
- **A sound per category**: harmful, beneficial, expiring.
- **Tooltips** (hold a key) explaining what each does and how long it lasts.
- **Log lines**: "Goblin is poisoned! (3 turns)".

---

## 8. Testing statuses

The nasty interactions, all of which you should deliberately try (Day 80):

- Poison damage kills an actor exactly when their turn begins.
- Sleep and stun applied simultaneously.
- A buff expiring in the middle of an attack sequence (are the numbers computed at start or on hit?).
- The status source dies before the status expires.
- The same buff applied four times.
- A status applied to an already-dead actor.
- Statuses at battle end, and whether they carry over.
- An actor with 6 statuses at once - does the UI still fit?
- A status that modifies max HP while HP is at max.
- Reapplying poison on the turn it would have expired.

Add assertions: poison expires after exactly N ticks, no stat is negative, no duration is negative, no
status vector exceeds a sane size.

---

## 9. A starting status set

Eight is a good number for a game this size. More becomes bookkeeping without depth.

| Status | Type | Effect | Duration | Stacking |
| --- | --- | --- | --- | --- |
| Poison | DoT | 1-2 damage at turn end | 3 | Refresh |
| Burn | DoT | 2 damage, also -1 ATK | 2 | Refresh |
| Sleep | Restriction | Skips turn, wakes on damage | 2-3 | Exclusive |
| Stun | Restriction | Skips turn | 1 | Exclusive, with immunity after |
| Slow | Modifier | -50% SPD | 3 | Highest wins |
| AttackUp | Modifier | +2 ATK | 3 | Stack to 2 |
| DefenceUp | Modifier | +2 DEF | 3 | Stack to 2 |
| Regen | HoT | +2 HP at turn end | 3 | Refresh |

Note the symmetry: for every buff there is a debuff, which makes the system feel complete without
extra content.

---

## 10. Where statuses fit the wider design

- **Enemies using statuses** creates a reason for the player to carry cures, which makes item
  management meaningful.
- **The player using statuses** must be worth a turn. A status spending a turn to deal *less* damage
  than attacking is only worthwhile in long fights - which is exactly why they shine against bosses.
  That is a good, intentional dynamic.
- **Equipment resistances** (Day 98) give gear a purpose beyond numbers and create real choices.
- **Badges** that add on-hit statuses turn a basic attack into a build option (Day 103).
- **Action commands** can interact: a Perfect grade could apply a status. That ties your core mechanic
  to the tactical layer, which is exactly what you want.

---

## Exercises

1. Write the full rules table (section 9's columns) for your own statuses **before** coding.
2. Implement poison and assert that it ticks exactly 3 times.
3. Try to stun-lock your own boss. Then make it impossible.
4. Apply the same buff four times and check the cap holds.
5. Kill an actor with poison at the exact moment their turn begins and confirm the battle-end check
   still runs.
6. Fit six status icons under one combatant and check the UI still reads.

## Done criteria

- [ ] Duration semantics are written down and asserted
- [ ] Every status has a documented stacking rule
- [ ] All stat modifiers flow through the accessor layer
- [ ] Action restrictions show a message when skipping a turn
- [ ] Stun-lock is impossible
- [ ] Statuses are readable by icon shape, not colour alone
- [ ] Every interaction in section 8 has been tested deliberately

## Sources

- Search "status effect design turn based RPG" for genre analyses
- [Paper Mario status effects list](https://www.mariowiki.com/) - see how few there are and how
  legible each one is
- Search "diminishing returns crowd control design" - the MMO and MOBA discussion of stun-lock applies
  directly
- Your own Day 23 console implementation; it will already have taught you half of this
