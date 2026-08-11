# Particles and visual effects

**Used on:** Day 93 (battle effects), Day 157 (the juice pass), Day 65 (breakable objects).

Why this matters: particles and screen effects are where "it works" becomes "it feels good". A hit
without a spark is a number changing; a hit with a spark, a flash, hitstop, and a shake is an *impact*.
This is also a place where a small, simple system serves you far better than an elaborate one.

---

## 1. A minimal particle system

You need one, and it should be small - under about 150 lines. The data:

```cpp
struct Particle {
    Vector2 position, velocity;
    float   life, maxLife;
    float   size;
    Color   colorStart, colorEnd;
    float   rotation, angularVelocity;
    float   gravity;
    bool    alive;
};
```

An emitter is a description of how to spawn them: a count (or a rate), a lifetime range, a speed range,
an angle range, a size curve, a colour ramp, and gravity.

Update is trivial: integrate velocity, apply gravity, decrement life, kill at zero, and interpolate size
and colour by `1 - life / maxLife`.

**Pool them.** Pre-allocate a fixed array (2000 is generous) and reuse dead slots. Allocating per
particle causes hitches and fragmentation for no reason. This is a good first use of the object pool
pattern.

**Questions to answer**

- Should particles collide with the world? (Almost never - it costs a lot and adds little.)
- Should particles be affected by hitstop? (Usually not - keeping effects animating during a freeze looks
  better.)
- World space or screen space? (World for hit sparks, screen for UI sparkles.)

---

## 2. Effects worth having

Small set, big impact:

| Effect | Where | Look |
| --- | --- | --- |
| Hit spark | Every hit | 8-12 fast radial particles, brief life, white to yellow |
| Critical burst | Perfect grades, crits | Larger, more particles, plus a screen flash |
| Heal sparkle | Healing | Slow upward drift, green to white, gentle |
| Level-up rays | Level up | Vertical beams plus rising particles, a longer effect |
| Status puff | Status applied | A small cloud in the status's colour |
| Dust | Landing, footsteps, running | A few low, short-lived particles |
| Coin/pickup pop | Chests, drops | Arc upward with gravity, then fade |
| Death poof | Enemy defeated | Outward burst plus the sprite fading or scaling |

Emit from **data**, not code: an effect definition per name, referenced by skills, items, and enemies. It
is the same data-driven pattern as everything else (see
[data-driven-design.md](data-driven-design.md) section 5), and it lets you tune effects with hot reload.

---

## 3. Screen effects

Often more impactful than particles, and cheaper:

- **Flash**: draw a full-screen white or coloured rectangle with decaying alpha. One line, and it sells a
  critical hit better than any particle.
- **Vignette**: a dark border, intensified at low HP. Communicates danger without a UI element.
- **Screen shake**: see [camera-and-coordinates.md](camera-and-coordinates.md) section 7. Scale to the
  event; always provide an option to disable it.
- **Hitstop**: freeze gameplay updates for 0.05-0.1s on impact. The single highest-impact effect in the
  whole game. Not visual, but it belongs in this list because it *reads* as visual weight.
- **Zoom punch**: a brief camera zoom in and out on a big hit.
- **Fade to colour**: transitions, damage flashes, story beats.
- **Time slow**: scale the timestep briefly on a successful superguard. It makes mastery feel great.

Combine them for important moments. A boss's death is a flash, a shake, hitstop, particles, a zoom punch,
and a sound - all at once, and none of them individually expensive.

---

## 4. Sprite-based effects

Not everything should be particles. Hand-drawn effect animations (a slash arc, an explosion, a magic
circle) read better for anything with a distinct shape.

- Use a one-shot animation with your existing animator, drawn above the actors, destroyed on completion.
- Additive blending makes magic and light effects glow. raylib supports blend modes via
  `BeginBlendMode(BLEND_ADDITIVE)`.
- Layer them: a sprite flash *plus* particles *plus* a screen flash reads as more powerful than any one.
- Timing matters more than art quality: an effect appearing exactly on the contact frame with a 0.2s
  lifetime looks good even if the art is crude.

---

## 5. Damage numbers as an effect

Your most frequently seen effect, so it is worth real attention:

- Pop up with `EaseOutBack`, drift upward, fade out over 0.6-1.0s.
- Style by result: normal, critical (larger, shaking, different colour), weak ("Weak!" label), resisted
  (small, grey), blocked ("Blocked!"), healing (green with a plus).
- **Stagger multi-target and multi-hit numbers** by 0.06s so they read one at a time instead of stacking
  into an unreadable pile.
- Offset overlapping numbers so two hits on the same target remain legible.
- Pool them, like particles.

---

## 6. Performance and restraint

Particles are the easiest way to accidentally tank your frame rate, and the easiest place to overdo the
visuals.

- **Cap the total.** A hard limit (2000) with oldest-first replacement, so a bug cannot spawn a million.
- **Cull off-screen particles** from drawing (not from updating, unless they will never return).
- **Prefer many small draws of one texture** over many textures - it batches better.
- **Measure.** Fire every effect at once (Day 159's worst case) and check the frame time.
- **Restraint is design.** If everything sparkles, nothing reads as important. Reserve big effects for
  big moments, and keep the routine ones subtle. A game where a basic attack has a screen-filling
  explosion has no way to make a critical feel special.

---

## 7. Feedback layering: the mental model for Day 157

For each player action, ask which channels are covered:

| Channel | Example |
| --- | --- |
| Visual | Particles, flash, sprite change |
| Audio | A distinct sound |
| Motion | Animation, knockback, shake |
| Timing | Hitstop, pause, slow motion |

**Fewer than two channels means the action feels weak.** Important actions should have three or four.
Go through every action in your game (attack, guard, heal, level up, pickup, menu confirm, dialogue
advance, taking damage) and fill in the grid. The gaps are your Day 157 to-do list.

---

## Exercises

1. Build the pooled particle system and emit a hit spark. Tune it for two minutes until it feels right.
2. Add a screen flash on criticals and toggle it on and off. Note which you prefer.
3. Implement hitstop and A/B test it. Write down the difference in your journal.
4. Build damage numbers with pop, drift, fade, and staggering.
5. Fire every effect in the game simultaneously and record the frame time.
6. Fill in the feedback grid from section 7 for eight player actions and find the gaps.

## Done criteria

- [ ] Particles are pooled with a hard cap
- [ ] Effects are defined in data and hot-reloadable
- [ ] I have hit sparks, criticals, heal sparkles, dust, and death effects
- [ ] Hitstop exists and I understand why it matters
- [ ] Damage numbers are styled by result and staggered
- [ ] Screen shake can be disabled
- [ ] Every player action has at least two feedback channels
- [ ] The worst-case effect load stays under 16ms

## Sources

- [Juice it or lose it](https://www.youtube.com/watch?v=Fy0aCDmgnxg) - watch this before Day 157; it is
  the single most useful 10 minutes on this topic
- [Game Programming Patterns: Object Pool](https://gameprogrammingpatterns.com/object-pool.html)
- Search "game feel hitstop screen shake" for practical breakdowns
- raylib `textures_particles_blending` and blend mode examples
- Study Paper Mario's hit effects frame by frame: they are simple, well-timed, and consistently layered
