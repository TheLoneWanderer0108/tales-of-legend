# The game loop and timing

**Used on:** Days 25-26 (loop and delta time), Day 35 (fixed timestep), Day 82 (action command
timing).

Why this matters: the loop is the heartbeat of your game, and timing mistakes produce bugs that
are maddening to diagnose - things that work on your machine and break on someone else's, or that
behave differently when the frame rate changes. Your action commands depend on millisecond
accuracy, so this needs to be right.

---

## 1. What a game loop is

```
initialise
while (not quitting):
    handle input
    update the world
    draw the world
shut down
```

That is it. raylib gives you `WindowShouldClose()` for the condition and
`BeginDrawing()`/`EndDrawing()` to bracket the drawing. Everything else is yours.

The one rule that saves you repeatedly: **update never draws, and draw never mutates state.**
Violating it produces bugs where things happen twice, or happen only when visible, or differ
between frames. Enforce it with `const` where you can.

**Questions to answer**

- Why is drawing separate from updating?
- What does `EndDrawing()` do besides finishing the drawing? (Look up buffer swapping and
  `SetTargetFPS`.)
- What happens to your loop when the window is minimised or dragged?

---

## 2. Frames, buffers, and vsync

The GPU displays one buffer while you draw into another; `EndDrawing` swaps them. Without this
you would see partially drawn frames (tearing).

`SetTargetFPS(60)` makes raylib sleep to approximate 60 FPS. It is a convenience, not a
guarantee - never assume your frame took exactly 16.67ms.

Vsync ties the swap to the monitor's refresh rate, which removes tearing but locks you to the
monitor's rate (60Hz, 144Hz, or something unusual). Which is precisely why frame-rate-independent
code matters.

---

## 3. Delta time

`GetFrameTime()` returns the seconds elapsed since the previous frame. Multiply every rate by it:

```
position += velocity * dt      // velocity is in pixels per second
timer    -= dt                 // durations in seconds
```

If you write `position += 3` instead, your game runs at double speed on a 120Hz monitor. This is
the single most common beginner bug in game programming.

Two subtleties that catch people:

- **Diagonal movement.** Adding full speed on both axes gives 1.41x the intended speed. Normalise
  the direction vector first, and guard against normalising a zero vector (it produces NaN, and
  once NaN is in a position, it never leaves).
- **Multiplicative damping.** `velocity *= 0.9f` each frame is frame-rate dependent, just like
  `position += 3`. The time-based form uses `pow(damping, dt)` or an equivalent exponential.

**Questions to answer**

- What are the units of your speed variables? Write them in a comment.
- Why does `velocity *= 0.9f` behave differently at 30 and 144 FPS?
- What happens to `dt` when you drag the window or the game hitches, and what does that do to
  your physics?

---

## 4. The problem with variable timestep

Variable timestep (updating with whatever `dt` arrived) is simple and fine for movement and
animation. It breaks down for anything that must be consistent or reproducible:

- **Non-determinism.** The same inputs produce different results on different machines, which
  makes bugs unreproducible and replays impossible.
- **Large steps skip things.** A 0.5s hitch can move an entity straight through a wall.
- **Accumulated float error** differs run to run.
- **Timing windows differ.** Your 60ms superguard window would effectively vary by frame rate,
  which is unacceptable for a game built on precise timing.

---

## 5. The fixed timestep

Update the simulation in constant-sized steps, regardless of how long the frame took:

```
accumulator += GetFrameTime();
int steps = 0;
while (accumulator >= FIXED_DT && steps < MAX_STEPS) {
    FixedUpdate(FIXED_DT);
    accumulator -= FIXED_DT;
    ++steps;
}
FrameUpdate(GetFrameTime());   // animation, camera, particles
Draw();
```

`FIXED_DT` of 1/60 is a good default. Now a 60ms window is always exactly 3.6 fixed steps, on
every machine.

**The death spiral:** if a fixed update takes longer than `FIXED_DT` to compute, the accumulator
grows, so you run more steps, which takes longer, and the game freezes permanently. `MAX_STEPS`
(around 5) caps it: the game slows down instead of hanging, which is a recoverable failure. Force
a stall with `WaitTime` and confirm your cap works.

**Questions to answer**

- Why does the accumulator need to be capped?
- What happens if `FIXED_DT` is very small, like 1/1000?
- If a frame takes 100ms, how many 60Hz fixed steps run, and what does that look like on screen?

---

## 6. What runs fixed and what runs per frame

Write your split down, because a system in the wrong bucket causes subtle bugs.

| Fixed step | Per frame |
| --- | --- |
| Movement and collision | Sprite animation |
| Gameplay timers | Camera smoothing |
| Action command windows | Particles |
| Status durations | UI tweens |
| AI decisions | Screen shake |
| Anything affecting fairness | Anything purely cosmetic |

Rule of thumb: **if a player could win or lose because of it, it belongs on the fixed step.**

---

## 7. Interpolation (optional, know it exists)

With a fixed timestep, rendering can land between two simulation steps, which can look slightly
juddery at high refresh rates. The fix is rendering at
`previousPosition + (currentPosition - previousPosition) * alpha`, where alpha is the leftover
accumulator fraction.

For a 2D pixel-art game at 60Hz this is usually unnecessary, and it conflicts with pixel snapping
(Day 51) - snapped positions cannot be smoothly interpolated. Know the technique exists, measure
whether you need it, and probably skip it.

---

## 8. Measuring time correctly

- `GetTime()` returns seconds since init - fine for durations and animation phase.
- `GetFrameTime()` for delta.
- For profiling, prefer `std::chrono::steady_clock` and measure many iterations, not one.
- Accumulating `dt` into a float over hours loses precision. Use `double` for long-running clocks
  like playtime.
- Never measure gameplay timing in frames. "30 frames" is a different duration on every machine;
  "0.5 seconds" is not.

---

## Exercises

1. Move a rectangle with and without `dt`, and change `SetTargetFPS` between 30, 60 and 144.
2. Move diagonally, measure that it is faster, then fix it by normalising.
3. Feed NaN into a position (normalise a zero vector) and watch it never recover. Then add the
   guard.
4. Implement the accumulator, print how many fixed steps run each frame, then force a stall and
   watch the cap engage.
5. Build a timing test: press a key, measure the elapsed time until a target moment, and print the
   error in milliseconds. Confirm the error is identical at 30 and 144 FPS. This test is what
   your action commands will depend on.

## Done criteria

- [ ] Every rate in my game is multiplied by `dt`, in documented units
- [ ] Diagonal movement is normalised and NaN-guarded
- [ ] I have a fixed timestep with a capped accumulator and I can explain the death spiral
- [ ] I have written down which systems are fixed and which are per frame
- [ ] Action command timing is identical at 30, 60, and 144 FPS

## Sources

- [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/) - the canonical
  article, read it twice
- [Game Programming Patterns: Game Loop](https://gameprogrammingpatterns.com/game-loop.html)
- [Game Programming Patterns: Update Method](https://gameprogrammingpatterns.com/update-method.html)
- raylib `core_basic_window` and `core_custom_frame_control` examples
