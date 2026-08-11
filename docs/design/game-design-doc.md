# Tales of Legend - Game Design Document

A living document. You fill it in as decisions get made, not before. Empty sections are
honest; invented sections are lies you will have to undo later.

Rule of thumb: **do not write a design section until the day you are about to build it.**
Phase 7 Day 118-121 is where most of this gets filled in properly.

---

## 1. One-line pitch

> _(Example shape: "A turn-based JRPG where every attack and every block is a timed
> button press, about a scribe who rewrites the world's history one page at a time.")_

TBD.

## 2. Pillars

Three things the game is about. Every feature must serve one of them. If a feature serves
none, cut it.

1. TBD
2. TBD
3. TBD

## 3. Player fantasy

What does the player feel like they are doing? One paragraph.

TBD.

---

## 4. Story

### Premise

TBD.

### Cast

| Name | Role | Battle identity | Arc in one line |
| --- | --- | --- | --- |
| | | | |

### Chapter outline

| Ch | Location | Story beat | New mechanic introduced | Boss |
| --- | --- | --- | --- | --- |
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |

Design principle: **every chapter introduces exactly one new mechanic**, and its boss is
an exam on that mechanic.

---

## 5. Battle design

### Turn structure

TBD. (Decide in Phase 5 Day 72. Write the decision here with the reason.)

### Action commands

The core of the combat. Each attack has an input mini-interaction.

| Command | Input | Window | Reward | Where it appears |
| --- | --- | --- | --- | --- |
| Timed strike | press A as the marker crosses the zone | | +50% damage | basic attack |
| Mash | rapid A presses for 2s | | damage scales with count | multi-hit |
| Hold and release | hold A, release in zone | | | charged attack |
| Rotate | full stick rotation | | | special |
| Guard | press A just before impact | | -1 damage | defence |
| Superguard | press B in a tighter window | | 0 damage + counter | defence |

### Resources

| Resource | Purpose | How it is restored |
| --- | --- | --- |
| HP | | |
| FP (flower/skill points) | special moves | |
| Star power | limit-break style moves | |

### Damage formula

Decided in Phase 5. Write the final formula here plus one worked example:

```
TBD
```

---

## 6. Progression

- Level curve: TBD
- What levelling grants (player choice vs automatic): TBD
- Badge / skill system: TBD
- Equipment slots: TBD

## 7. Content budget

Be ruthless here. This is the section that decides whether the game ships.

| Content type | Target count |
| --- | --- |
| Chapters | 3 |
| Maps | |
| Enemy types | |
| Bosses | 3 |
| Party members | |
| Items | |
| Skills per character | |
| Badges | |
| Music tracks | |
| Estimated playtime | |

## 8. Explicitly cut

Write down everything you decided **not** to do, and why. This list protects you from
re-litigating the same idea at 1am in month six.

| Cut feature | Why |
| --- | --- |
| Multiplayer | Doubles every system's complexity for no learning benefit |
| 3D | Not the project |
| Voice acting | Cost, and it locks the script |
| Open world | Pacing nightmare for a first game |
