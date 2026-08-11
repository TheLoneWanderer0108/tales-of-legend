# RPG math and balance

**Used on:** Day 11 (first damage formula), Day 77 (damage pipeline), Day 94 (balance harness),
Day 99 (economy), Day 101 (XP and growth curves), Day 124 (enemy tiers).

Why this matters: RPG balance is the most quietly mathematical part of game development, and it is
where "it feels fine to me" fails hardest, because you are the worst possible playtester - you know
every system and every enemy. The fix is to work with curves and simulations rather than intuition.

The most useful tool in this note is not a formula. It is a spreadsheet.

---

## 1. Damage formula families

**Subtractive:** `damage = atk - def`

Simple and intuitive. The problem is a hard cliff: when `def >= atk`, damage is zero and the game is
unwinnable; a few points of DEF can trivialise an enemy. Requires careful clamping and tight stat
ranges. Classic early Final Fantasy.

**Multiplicative / ratio:** `damage = atk * atk / (atk + def)`

Smooth, never reaches zero, and diminishing returns on DEF fall out naturally. Harder to predict
mentally, and needs tuning to keep numbers in a nice range. Recommended default.

**Percentage reduction:** `damage = atk * (100 / (100 + def))`

Very controllable: DEF of 100 halves damage, and you can reason about it directly. Extremely common
in modern games, and easy to explain to players.

**Fixed damage with modifiers:** `damage = skillPower * gradeMultiplier - def`

Attack power comes mostly from the *skill*, with stats as modifiers. This is what Paper Mario does,
and it is why its numbers are tiny (a hammer does 2 damage, an enemy has 6 HP).

**Recommendation for this project: small numbers, skill-power based, with subtractive DEF and a
minimum of 1.** Small numbers are a genuine design advantage: every point matters, players can do the
arithmetic in their heads, a +1 badge is exciting, and balancing is far easier because the space is
small. Big numbers (9999 damage) hide their own imbalance.

**Questions to answer**

- With your formula, what happens when DEF is very high relative to ATK?
- Can the player ever deal zero damage? Should they be able to?
- Can you predict, mentally, the damage of your basic attack against your first enemy? (If not,
  consider smaller numbers.)

---

## 2. The case for small numbers

Compare two designs for the same fight:

| | Small numbers | Big numbers |
| --- | --- | --- |
| Player HP | 10 | 450 |
| Attack damage | 2-4 | 85-140 |
| A +1 ATK badge | +25-50% impact, exciting | invisible |
| Player can compute the turns to win | Yes, instantly | No |
| Balancing effort | Small search space | Large search space |
| A tuning error of 1 point | Noticeable and fixable | Undetectable |

Paper Mario's HP values are famously tiny and it is one of the reasons its combat is so legible.
Strongly consider: player HP starting at 10 and reaching 50 by the end, damage from 1 to 10.

---

## 3. Variance and criticals

Some randomness keeps combat from feeling scripted; too much makes skill irrelevant.

- **Variance** of ±10% is a good default. With small numbers, variance may round to nothing - in which
  case drop it entirely and let action command grades provide the variation. That is *better* design,
  because the variation becomes skill-based rather than random.
- **Criticals**: a 5-10% chance for 1.5-2x. With action commands, consider making criticals a
  *reward for a Perfect grade* rather than a dice roll. Skill-based criticals feel far better than
  lucky ones, and they reinforce your core mechanic.
- **Accuracy and evasion**: strongly consider omitting them. A missed attack in a game where you just
  performed a perfect timed press is infuriating - you did everything right and the dice said no. If
  you want evasive enemies, express it as a narrower timing window instead. That is the Paper Mario
  philosophy and it is a better one.

---

## 4. The damage pipeline

Order matters enormously, so define it once and implement it as ordered stages (Day 77):

```
1. base           = skill power (+ attacker ATK contribution)
2. commandGrade   = multiply by action command multiplier   [0.7x - 1.8x]
3. attackerMods   = buffs, badges, equipment
4. element        = weak 1.5x, resist 0.5x, immune 0x, absorb -> heal
5. defence        = subtract DEF, then apply guard/superguard reduction
6. critical       = apply crit multiplier if triggered
7. variance       = random ±10% (optional; see section 3)
8. clamp          = minimum 1, or 0 if fully blocked
9. apply + emit event
```

Consequences of the ordering to think through: applying DEF before the grade multiplier makes DEF far
stronger; applying elemental multipliers after DEF makes weaknesses more dramatic. There is no single
right answer - but there is a right *process*, which is to pick an order, write it down, and make the
debug calculator (Day 77) show every stage.

---

## 5. Elemental design

Keep it small. Four to six elements is plenty; twelve is a bookkeeping burden with no added depth.

Suggested: Physical, Fire, Ice, Electric, plus perhaps Light/Dark for late game.

Multipliers: weak 1.5x (or 2x for drama), resist 0.5x, immune 0x, absorb heals. Communicate them
clearly - the "Weak!" label doing 1.5x damage teaches the system in one hit, whereas a silent
multiplier teaches nothing.

Avoid the rock-paper-scissors trap where the player must have every element to progress. Elements
should reward preparation, not gate it.

---

## 6. Defence and mitigation stacking

Watch for **multiplicative stacking**, the classic balance killer: 50% from armour, 50% from a buff,
and 50% from guard combine to 87.5% reduction, and your enemies become harmless.

Options:

- Additive stacking with a cap (say 75% total reduction).
- Diminishing returns: each source is less effective than the last.
- Only one source of each type applies (the highest).

Whatever you choose, **test the maximum stack**. Build the most defensive loadout possible, fight your
hardest enemy, and see whether you can be hurt at all. Do the same in reverse for damage output.

---

## 7. Health, damage, and time-to-kill

Time-to-kill (TTK) is the number that actually determines how a fight feels.

```
rounds to kill = enemyHP / (partyDamagePerRound)
```

Targets for this project:

| Enemy | Rounds to kill | Reason |
| --- | --- | --- |
| Weak trash | 1-2 | Should not slow exploration |
| Normal | 2-3 | Long enough for one decision |
| Strong / mini-boss | 4-6 | Requires a plan |
| Boss | 8-15 | A performance, in phases |

And in reverse: how many rounds until the *player* dies with no healing? If it is 2, the game is
brutal; if it is 20, there is no tension. Aim for 4-6 for a normal encounter.

Work backwards from these numbers. Choose the TTK you want, then derive the enemy's HP from your
party's expected damage output. This is much easier than inventing HP values and hoping.

---

## 8. Balancing by simulation

The technique that makes all of this tractable (Day 94):

1. Run battles headlessly, thousands of times, with no rendering.
2. Model the player at three skill levels via action command success profiles: bad (20% success),
   average (60%), expert (95%).
3. Report win rate, average rounds, average HP remaining, and FP spent.
4. Compare against targets, adjust one number, re-run.

Targets worth writing in the design doc:

| Encounter type | Average-player win rate | Rounds |
| --- | --- | --- |
| Normal | 95%+ | 3-5 |
| Tough | 80-90% | 5-8 |
| Boss (first attempt) | 60-75% | 8-15 |

Also check that **skill matters**: an expert profile should finish in noticeably fewer rounds than a
bad one. If the two are similar, your grade multipliers are too timid and the action commands are
decorative.

The simulator is also the fastest way to find degenerate strategies: if one skill is always optimal,
or if item spam is unbeatable, the numbers will show it long before a playtester does.

---

## 9. Spreadsheets

Use a spreadsheet, genuinely. Three sheets will do:

1. **Stat curves**: level in column A, then HP, ATK, DEF, SPD per character. Plot it. Look for kinks.
2. **Enemy tiers**: for each chapter and tier, the HP, ATK, DEF, SPD, XP, and gold. Authoring a new
   enemy becomes "copy the tier row, then add a gimmick" (Day 124).
3. **Damage matrix**: your formula's output for a range of ATK against a range of DEF. Scan for zeros
   and explosions.

Export your simulator results to CSV and plot them against these. Seeing your curves is completely
different from imagining them - kinks and cliffs are obvious in a graph and invisible in a table.

---

## 10. Common balance failures

| Failure | Symptom | Fix |
| --- | --- | --- |
| Damage sponge enemies | Fights are long and boring, not hard | Lower HP, raise damage |
| The one true strategy | Player repeats one action forever | Add costs, or buff alternatives |
| Healing outpaces damage | Fights become unlosable but slow | Cap healing per turn, or raise enemy damage |
| Status effects too strong | Stun-lock trivialises bosses | Short durations, boss resistances, diminishing returns |
| Grinding is optimal | Player levels for an hour to win | Scale XP down with level difference |
| Level-scaled enemies | Levelling feels pointless | Do not scale enemies to player level |
| Economy inflation | Gold becomes meaningless | Fewer, more expensive sinks |
| Difficulty cliff | Players quit at one specific fight | Simulate every encounter; smooth the curve |

---

## 11. XP and level curves

Two curves, and they interact:

**XP required per level.** Linear (`100 * level`) makes late levels fast; quadratic
(`base * level^1.5`) is the classic feel; exponential punishes late levelling severely. For a 20-level
game, mildly superlinear works well.

**Stat growth per level.** For a short game, an **explicit table per character** (20 rows) gives total
control and takes 20 minutes to author. Formulas are for games with 99 levels.

Then decide what levelling *grants*. Automatic stat gains are fine, but Paper Mario's model - the
player chooses HP, FP, or badge capacity - is better, because every level-up is a decision that
expresses a playstyle. Strongly consider it.

**Do not scale enemies to player level.** It makes levelling feel pointless and is one of the most
disliked mechanics in the genre. Instead, write an expected level per chapter in the design doc and
balance encounters against it.

Do scale **XP rewards** down when the player greatly out-levels an enemy, so grinding stops paying.

---

## 12. Economy

The loop is: fight, earn, spend, get stronger. Balance it with a few anchor numbers:

- A normal battle should pay for roughly one consumable.
- A gear upgrade should cost several dungeons' worth of income, so it is a decision.
- Sell value at 50% of purchase price (never 100%, or buy-sell loops appear).
- Total income across a chapter should slightly exceed the cost of that chapter's recommended
  purchases - so a careful player can afford everything and a careless one cannot.

Watch for infinite money: a respawning enemy plus a sellable drop is a money printer. Either accept it
(grinding is a legitimate choice) or cap it.

---

## 13. The balance process

1. **Set targets numerically** before tuning anything (TTK, win rates, income).
2. **Change one number at a time.** Changing three and re-testing teaches you nothing.
3. **Simulate**, then playtest to confirm the simulation matches reality.
4. **Watch other people play.** They will use strategies you never considered, which is how you find
   the degenerate ones.
5. **Prefer fixing outliers over global tuning.** One overtuned enemy is a bug; a whole chapter being
   hard is a curve problem.
6. **Write your final numbers in the design doc**, so the next enemy you author has a reference.

---

## Exercises

1. Build the damage matrix in a spreadsheet for three formula families and compare their shapes.
2. Design your game with player HP of 10 at the start. Work out the damage numbers that gives you.
3. Compute the rounds-to-kill for each of your planned enemy tiers, then derive their HP from your
   party's damage.
4. Plot your XP curve cumulatively to level 20.
5. Build the most defensive loadout possible and see whether anything can hurt you.
6. Run the simulator on one encounter at three skill profiles and confirm skill changes the result.

## Done criteria

- [ ] My damage formula is a written decision, with a worked example in the design doc
- [ ] My damage pipeline stages are ordered, documented, and inspectable
- [ ] Numbers are small enough that a player can do the arithmetic
- [ ] Mitigation stacking is capped and I have tested the maximum stack
- [ ] Time-to-kill targets are written down and enemy HP is derived from them
- [ ] The simulator confirms my win rate targets and that skill matters
- [ ] Curves live in a spreadsheet and I have looked at the graphs
- [ ] Enemies are not scaled to player level, and XP scales down with level difference

## Sources

- [Understanding damage formulas](https://www.gamedeveloper.com/design) - search the site for RPG
  damage formula analyses
- [Paper Mario stat analysis](https://www.youtube.com/results?search_query=paper+mario+damage+formula+analysis) -
  study specifically how small its numbers are
- Search "RPG balance spreadsheet time to kill" for practical examples
- [Level up curves in RPGs](https://en.wikipedia.org/wiki/Experience_point) - start here, then look at
  real games' published curves
