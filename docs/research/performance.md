# Performance

**Used on:** Day 45 (tilemap culling), Day 159 (the profiling pass), Day 160 (robustness).

Why this matters: your game is 2D with maybe 100 entities, so it should run at hundreds of frames per
second on any modern machine. If it does not, something specific is wrong, and finding it is a matter of
measurement rather than of general optimisation.

The one law of this note: **measure first.** Your intuition about where the time goes will be wrong. It
is wrong for everyone, including experienced developers, which is precisely why profilers exist.

---

## 1. Know your budget

At 60 FPS you have **16.67 milliseconds** per frame for everything: update, draw, and the GPU's work.

For a 2D pixel-art game, a reasonable target on a modest machine:

| Work | Budget |
| --- | --- |
| Game update | under 2ms |
| Draw calls submitted | under 4ms |
| Everything else | slack |

If you are anywhere near 16ms in a game like this, you have a bug rather than a performance problem -
usually an accidental O(n²) loop, a per-frame allocation, or a per-frame asset load.

**Measure the worst case, not the average.** A game that averages 8ms but spikes to 40ms in the crowded
map feels worse than one steady at 12ms. Stutter is more noticeable than a lower frame rate.

---

## 2. Measurement tools, in order of usefulness

1. **A frame time graph in your debug overlay.** The last 120 frames as a bar graph. This is the single
   most useful performance tool you will build, because spikes become visible rather than merely felt.
   Build it on Day 159 (or earlier).
2. **Manual timers around suspect sections.** `std::chrono::steady_clock` around update, draw, and
   individual systems, displayed on screen.
3. **`perf`** (`perf record ./game`, then `perf report`). Tells you where the CPU actually spent its time,
   with no guessing. Build with `RelWithDebInfo` so you get symbols with optimisation.
4. **`GetFPS()` and `GetFrameTime()`** for a rough sanity check.
5. **`valgrind --tool=callgrind`** if you want call counts rather than samples; very slow but precise.

Also worth knowing: `perf stat` shows cache misses and branch mispredictions, which is interesting once
you have exhausted the obvious problems.

---

## 3. The likely culprits in a 2D game

In rough order of how often they are the answer:

| Cause | Symptom | Fix |
| --- | --- | --- |
| Drawing every tile of a large map | Frame time scales with map size | Cull to the camera (Day 45) |
| Loading an asset per frame | A hitch on first use, or constant slowness | Cache in the asset manager |
| Allocating per frame | Gradually degrading frame time, occasional spikes | Reserve, reuse, pool |
| O(n²) entity loops | Frame time explodes as entities grow | Only check what is nearby |
| Excessive draw calls | Steady high draw time | Batch by texture, cull off-screen |
| String operations in the hot path | Diffuse slowness that profiling reveals | Precompute, use IDs not strings |
| Particles without a cap | Sudden collapse during effects | Cap and pool (Day 93) |
| Debug drawing left enabled | Mysteriously slow release build | Compile it out or gate it |

Note how many of these are *bugs* rather than optimisations. That is normal at this scale.

---

## 4. Draw call cost

Each `DrawTexture` becomes GPU work, and raylib batches automatically **as long as consecutive draws use
the same texture**. Switching texture breaks the batch.

Practical consequences:

- Keep each map to one tileset image where you can.
- Group entity draws by texture where it is free to do so - though correctness (y-sorting) beats batching
  in a game this size.
- Prefer one sprite atlas over many individual textures.
- Culling is more valuable than batching: not drawing something is always cheaper than drawing it
  efficiently.

Caching a static tile layer to a `RenderTexture2D` reduces thousands of tile draws to one texture draw.
Only do it if your measurements say the tile layer costs real time, and note that it complicates animated
tiles.

---

## 5. Allocation and cache

Two categories of cost that are invisible in the code but visible in a profiler:

**Allocations.** `new`, `push_back` growth, and any temporary `std::string` or `std::vector` created per
frame. Individually tiny, collectively a hitch.

- `reserve()` vectors whose size you can estimate.
- Reuse buffers across frames instead of recreating them (a member vector cleared each frame rather than
  a local one).
- Pool particles, damage numbers, and projectiles.
- Avoid building strings in the hot path - `TextFormat` per frame for debug text is fine, but not inside
  a per-entity loop.

**Cache locality.** Contiguous data is dramatically faster to iterate than pointer-chasing.

- `std::vector<Entity>` beats `std::vector<std::unique_ptr<Entity>>` for iteration.
- `std::unordered_map` lookups in a tight loop are slower than they look; hoist them out.
- This is the core insight behind ECS designs (see
  [entity-architecture.md](entity-architecture.md) section 5) - you can benefit from the insight without
  adopting the architecture.

At 100 entities none of this will be your bottleneck. Know it so you can recognise it if a profile points
there.

---

## 6. Load times

Also performance, and also noticed by players:

- **Startup**: preload from a manifest (Day 40) with a simple loading indicator rather than hitching on
  first use of each asset.
- **Map transitions**: under a second. If not, you are probably re-loading textures the asset manager
  should already have cached - check your load log for duplicate loads.
- **Battle entry**: preload battle assets at startup; a hitch here interrupts a transition the player is
  watching.
- Measure each of these explicitly, because they are easy to ignore during development on a fast machine
  with a warm file cache.

---

## 7. The optimisation process

1. **Confirm there is a problem.** A frame time under budget needs no work, however inelegant the code.
2. **Measure** to find the actual hotspot.
3. **Form a hypothesis** about why it is slow.
4. **Change one thing.**
5. **Measure again** and record both numbers.
6. If the improvement is small, **revert it.** Complexity you cannot justify with a number is a cost with
   no benefit.

Two rules worth internalising:

- **Do not optimise what you have not measured.** You will spend a day on something worth 0.1ms.
- **Prefer algorithmic wins over micro-optimisation.** Culling 39,000 tiles beats making one tile draw
  5% faster, every time.

Record before/after numbers in your journal for every optimisation. "I optimised it" without a number is
a story, not an engineering result.

---

## 8. Release builds and measuring honestly

- Always measure in a **release build** (`-O2`). Debug builds can be 5-10x slower, and sanitizer builds
  slower still, so measurements from them are meaningless for performance decisions.
- Ensure debug drawing and debug overlays are disabled or compiled out when measuring.
- Test on the slowest machine you have access to. A game that runs at 300 FPS on your machine may run at
  40 on a laptop with integrated graphics.
- Test with vsync on and off, and at different window sizes (a larger window means more GPU work for the
  final scale-up, though your internal resolution keeps the rest fixed).
- Beware measuring one frame - always look at a distribution or a graph.

---

## Exercises

1. Build the frame time graph in your debug overlay and watch it while playing for five minutes.
2. Draw a 200x200 tilemap naively, record the frame time, add culling, and record it again.
3. Profile with `perf record` and read the report. Compare the top entry with what you predicted.
4. Create an accidental O(n²) loop over entities, spawn 500 of them, and watch the frame time. Then fix
   it.
5. Add a per-frame `std::string` construction in the entity loop, measure, then remove it.
6. Measure your startup, map transition, and battle entry times.
7. Fire every particle effect at once in the busiest map and check the worst-case frame time.

## Done criteria

- [ ] I have a frame time graph and I use it
- [ ] I have profiled with `perf` at least once and read the report
- [ ] Worst-case frame time is under 16ms, not just average
- [ ] Tilemap rendering is culled, with measured before/after numbers
- [ ] No per-frame allocations in hot paths; particles and numbers are pooled
- [ ] Startup, map transitions, and battle entry are all measured and acceptable
- [ ] Every optimisation I kept has a recorded before/after number
- [ ] I measured in a release build on the slowest machine I have

## Sources

- [perf tutorial](https://perf.wiki.kernel.org/index.php/Tutorial) - the basics are enough
- Search "data oriented design cache locality games" - Mike Acton's CppCon talk is the famous one, and
  worth watching even though it is more extreme than you need
- [Game Programming Patterns: Data Locality](https://gameprogrammingpatterns.com/data-locality.html)
- raylib source in `build/_deps/raylib-src/src/rtextures.c` and `rlgl.h` - read how batching actually
  works; it is more approachable than you would expect
