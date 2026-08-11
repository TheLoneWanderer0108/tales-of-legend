# Animation, tweening, and easing

**Used on:** Day 30 (sprite animation), Day 50 (tweens and easing), Day 55 (box animation),
Day 78 (attack sequences), Day 157 (juice).

Why this matters: animation is what makes a game feel alive, and tweening is the cheapest quality
you can buy. A menu cursor that eases into place, a dialogue box that scales open, a damage number
that pops and drifts - each is a handful of lines and each makes the game feel several times more
professional. This note covers both sprite animation (playing frames) and procedural animation
(moving values over time).

---

## 1. Sprite animation basics

A sprite sheet is one image containing many frames in a grid. Animation is showing successive
frames over time.

The data you need:

```cpp
struct Animation {
    int   firstFrame;      // index into the sheet
    int   frameCount;
    float frameDuration;   // seconds per frame
    bool  loops;
};

struct Animator {
    const Animation* current = nullptr;
    int   frame  = 0;
    float timer  = 0.0f;
    bool  finished = false;
};
```

Update by accumulating `dt` and advancing when the timer exceeds `frameDuration`. Use a `while`
rather than an `if`, so a long frame does not merely delay the animation but advances it correctly.

Frame rate: 8-12 frames per second reads as classic pixel-art animation. That is 0.08-0.125 seconds
per frame - note it is independent of your 60 FPS rendering.

**Questions to answer**

- Why store frame *duration* rather than frames per second?
- What should happen when a non-looping animation finishes? Who is responsible for noticing?
- How do you handle a sheet where animations have different frame counts?

---

## 2. Driving animation from state

Animation should be a *consequence* of game state, not something you set imperatively from twenty
places. Derive it:

```
if attacking      -> "attack_" + direction
else if moving    -> "walk_"   + direction
else              -> "idle_"   + direction
```

**The bug you will write:** re-selecting the currently playing animation every frame resets its
timer, so the sprite freezes on frame 0. Guard it: only change if the new animation differs from the
current one. Everyone writes this bug once; you can write it zero times.

Directions: for a four-direction top-down game you need idle and walk in four directions. A common
saving is to draw only left and mirror it for right by negating the source rectangle width.

For non-looping animations (attack, hurt, open), the game logic usually needs to know when they
finish. Expose `Finished()` and let the state machine react - this is how your attack sequences stay
synchronised with their damage frames (see section 8).

---

## 3. Practical sprite sheet advice

- Keep every frame the same size and use a consistent origin, or your sprite will jitter between
  frames.
- Anchor characters at their **feet centre**, not their top-left. It makes positioning, y-sorting,
  and collider alignment all simpler.
- Leave a transparent margin if frames touch, or you will get bleeding from neighbouring frames when
  filtering is not point-sampled.
- Lay out one animation per row when you can - it makes indices readable.
- Write down your sheet convention in the design doc so all your art matches.
- Use `TEXTURE_FILTER_POINT` for pixel art, always.

---

## 4. Tweening: animating any value

A tween moves a value from A to B over time using an easing curve. The core is one function:

```
value = start + (end - start) * ease(t)      where t goes 0 -> 1
```

`Lerp(a, b, t) = a + (b - a) * t` is linear interpolation and the basis of everything here. Write it
yourself once for floats and once for `Vector2` (raylib 6.0 also added C++ math operators for
`Vector2`, which makes this pleasant).

What you will tween, constantly: menu cursor position, panel scale on open, HP bar fill, damage
number position and alpha, camera pans, sprite lunges, screen fades, and text reveal.

Build this once (Day 50) and you will use it every day afterwards.

---

## 5. Easing functions

Linear motion looks robotic because nothing in the physical world moves at a constant speed with
instant starts and stops. Easing fixes it.

The vocabulary:

| Easing | Feel | Use for |
| --- | --- | --- |
| Linear | Mechanical | Progress bars, timers |
| Ease-in (quad) | Slow start, accelerating | Something leaving, falling |
| Ease-out (quad) | Fast start, settling | UI arriving, cursor moving - **the default choice** |
| Ease-in-out | Smooth both ends | Camera pans, long moves |
| Ease-out-back | Overshoots then settles | Pop-ins, damage numbers, dialogue boxes |
| Ease-out-elastic | Springy oscillation | Playful emphasis - use sparingly |
| Ease-out-bounce | Bounces to rest | Objects landing |

Rules of thumb: **ease-out for things arriving, ease-in for things leaving, ease-out-back for
anything that should feel snappy and alive.** If you only learn two, learn ease-out-quad and
ease-out-back.

Implement them yourself - they are one-liners over a normalised `t`. Then build the visualiser
(Day 50): graph each curve and animate a square along it. Fifteen minutes with that visualiser
teaches you more than any amount of reading, because easing is something you recognise by eye.

---

## 6. Duration is as important as curve

Beginners choose durations that are far too long. Reference points worth internalising:

| Thing | Duration |
| --- | --- |
| Menu cursor move | 0.06-0.12s |
| Panel open/close | 0.15-0.25s |
| Damage number lifetime | 0.6-1.0s |
| Screen fade | 0.25-0.4s |
| Attack lunge | 0.15-0.25s |
| Hitstop freeze | 0.05-0.10s |
| Camera pan (cutscene) | 0.8-2.0s |

When something feels sluggish, cut the duration by 20-30% before touching anything else. That is
usually the whole fix. Snappy UI is a large part of why polished games feel polished.

---

## 7. Timers, and sequencing tweens

You need three related utilities, and they compose:

- **Timer**: a duration, a `Progress()` from 0 to 1, and a `Finished()`. Also useful looping (for
  cooldowns and periodic effects).
- **Tween**: a timer plus a start value, an end value, an easing function, and an optional
  `onComplete` callback.
- **Sequence**: an ordered list of steps, each either a tween, a wait, or a callback, advancing when
  the current step finishes. Also worth supporting parallel groups (several tweens at once, waiting
  for all).

That third one is the important one: **your battle attack animations, cutscenes, and UI transitions
are all sequences.** Building it properly on Day 50 means Days 63 and 78 mostly consist of
authoring, not engineering.

Callbacks need care: a callback firing after its owner is destroyed is a use-after-free. Prefer
capturing IDs over pointers, and check validity when the callback runs.

---

## 8. Synchronising animation with logic

This is the subtle one, and it matters most in battle (Day 78).

An attack has an *animation* (wind-up, lunge, recoil, return) and a *logical event* (damage is
applied). They must agree on when contact happens. If damage is applied when the command is
selected, the numbers appear before the sprite touches the target and everything feels
disconnected.

Approaches:

- **Event frames**: mark a frame in the animation as "the hit frame" and fire the damage when the
  animator reaches it. Data-driven and precise.
- **Sequence steps**: the sequence itself contains "move forward", then "apply damage", then
  "recoil". Simpler, and usually enough.

Either way, **one source of truth for the contact moment.** This becomes critical for defensive
action commands (Day 86): the guard window is centred on the incoming attack's contact moment, so if
the animation and the logic disagree, the game is effectively lying to the player about when to
press, which is unforgivable in a timing-based game.

Related: **hitstop.** Freezing everything for 0.05-0.1s at the moment of impact is the single most
effective piece of game feel available. Implement it as a global "freeze frames" counter that skips
gameplay updates but still draws (and let effects and UI keep animating).

---

## 9. Anticipation, follow-through, and juice

Three animation principles from traditional animation that transfer directly and cheaply:

- **Anticipation**: a small movement opposite to the action before it happens (a wind-up before a
  punch, a slight squash before a jump). It makes actions readable and telegraphs them - which your
  action commands depend on.
- **Follow-through**: the motion settles rather than stopping dead. Ease-out plus a slight overshoot.
- **Squash and stretch**: scale non-uniformly during fast motion. Even 10% is noticeable and adds a
  lot of life.

Applied to your game: enemies wind up before attacking (which *is* the telegraph), the dialogue box
scales open with a slight overshoot, damage numbers pop up fast then drift and fade, the player
squashes slightly on landing.

None of these are expensive. All of them are why some games feel good and others feel stiff.

---

## Exercises

1. Build the animator and animate a 4-frame walk cycle. Then deliberately write the reset bug and
   watch the sprite freeze on frame 0.
2. Switch animations from state, including four directions, and mirror left to make right.
3. Implement six easing functions and build the visualiser. Watch them side by side.
4. Tween a menu cursor with ease-out-quad, then try linear, then ease-out-back. Decide which you
   prefer and why.
5. Build a sequence: move right 100px, wait 0.2s, move back, then print a message.
6. Add hitstop to a hit and toggle it on and off. Write down which you prefer.

## Done criteria

- [ ] My animator handles looping and one-shot animations, and exposes `Finished()`
- [ ] Animation is derived from state and does not reset when reselected
- [ ] I have written and *seen* at least six easing curves
- [ ] I have a tween and a sequence utility used by UI, battle, and cutscenes
- [ ] My animation durations are in the range from section 6
- [ ] Damage lands on the animation's contact moment, from one source of truth
- [ ] I have hitstop, and I understand why it matters

## Sources

- [Easings.net](https://easings.net/) - every easing function, graphed, with formulas
- [Juice it or lose it](https://www.youtube.com/watch?v=Fy0aCDmgnxg) - a 10-minute talk that will
  change how you think about feel; watch it before Day 157
- [Game Programming Patterns: Update Method](https://gameprogrammingpatterns.com/update-method.html)
- raylib `textures_sprite_anim` and `shapes_easings_*` examples (raylib ships easing examples)
- Search "hitstop game feel" and "anticipation animation principle games"
