# Debugging and tooling

**Used on:** Day 3 (setup), Day 35 (debug overlay), Day 120 (developer console),
Day 160 (robustness).

Why this matters: you will spend more hours debugging than writing new code. Beginners debug by
staring at code and guessing; that scales terribly. Every technique here replaces guessing with
observing, and the difference in speed is enormous - often the difference between five minutes
and five hours.

---

## 1. The debugging method

Guessing is not a method. This is:

1. **Reproduce reliably.** A bug you cannot trigger on demand cannot be fixed, only accidentally
   disturbed. Find the exact steps first. (This is why the `--seed` argument on Day 23 matters.)
2. **Form a hypothesis** that is specific and falsifiable: "the turn order vector still contains
   the dead actor's index", not "something is wrong with turns".
3. **Test the hypothesis** with one observation - a print, a breakpoint, a watch.
4. **Narrow the space.** Bisect: does it happen with one enemy? With no statuses? Halve the
   problem each time.
5. **Fix the cause, not the symptom.** Clamping a value that should never have been wrong hides
   the bug and it will resurface somewhere worse.
6. **Confirm** by reproducing the original steps, then write down what it was.

**Questions to answer**

- Why is an intermittent bug a reproduction problem before it is a code problem?
- What is the difference between fixing a cause and suppressing a symptom? Give an example from
  your own code.

---

## 2. Printf debugging is legitimate

Do not let anyone tell you otherwise. For game code it is often the *best* tool, because game
bugs happen over time rather than at a single moment, and a stream of values shows you a
trajectory a breakpoint cannot.

Do it well:

- Print the variable **name** with its value, not a bare number.
- Print state each frame only when something changes, or you drown in output.
- Prefix by system (`[battle]`, `[input]`) so you can filter with grep.
- In raylib, prefer drawing values on screen with `DrawText` and `TextFormat` over console
  output - a value you can see while playing is far more useful.
- Remove them, or route them through a proper logging function with levels, before committing.

---

## 3. gdb, the minimum useful set

You need about eight commands. Build with `-g -O0` first, or the debugger will lie to you about
line numbers and optimise variables away.

```
gdb ./build/game
break main.cpp:42      # or  break Battle::Update
run
next / n               # next line, stepping over calls
step / s               # step into a call
finish                 # run until the current function returns
print variable         # or  p actor.hp
backtrace / bt         # how did I get here
continue / c
watch variable         # break when it changes - superb for "who is corrupting this?"
```

The two highest-value uses:

- **A crash.** Run under gdb, let it crash, type `bt`. You now know exactly where and how you
  got there. This turns a mystery segfault into a two-minute fix.
- **A value going wrong.** `watch` it and let gdb tell you which line changed it.

Debugging a game loop is awkward because breaking every frame is unusable. Use a conditional
breakpoint instead: `break battle.cpp:88 if round == 3`.

**Questions to answer**

- Why do you need `-g -O0`?
- What is the difference between `next` and `step`?
- How would you find which code writes to a variable that keeps becoming -1?

---

## 4. Sanitizers, the highest-value tool you own

AddressSanitizer and UndefinedBehaviorSanitizer catch entire classes of bug instantly and point
at the exact line. Build with:

```
-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer
```

They catch: out-of-bounds reads and writes, use-after-free, double-free, memory leaks, null
dereferences, signed integer overflow, and misaligned access.

The cost is roughly 2x slower, which is irrelevant for a 2D game. **Keep a sanitizer build
directory and run it regularly, not only when something is wrong** - it finds bugs that have not
yet produced visible symptoms, which are the dangerous ones.

When it reports something, read the stack trace top-down: the allocation site, the free site, and
the access site are usually all shown. That is normally the whole answer.

valgrind (`valgrind ./build/game`) catches similar problems without recompiling but runs perhaps
20x slower. Use it as a second opinion on a full playthrough (Day 160).

---

## 5. Building tools for yourself

The professional habit that most distinguishes fast developers: when debugging becomes
repetitive, build a tool.

Your roadmap builds these deliberately:

| Tool | Day | What it saves |
| --- | --- | --- |
| F1 debug overlay | 35 | Constantly wondering what a value is |
| Slow motion and frame step | 35 | Debugging anything time-based |
| State stack printout | 39 | Understanding what is on top of what |
| Flag inspector | 59 | Replaying an hour to test a scene |
| Dialogue node jump | 57 | Replaying to reach a conversation |
| Damage calculator screen | 77 | Recomputing formulas by hand |
| Action command harness | 82 | Tuning timing blind |
| Hot reload | 62, 120 | Restarting for every content tweak |
| Developer console | 120 | Everything above, at once |
| Headless simulator | 94 | Guessing at balance |

Rule of thumb: **if you do something manually three times, automate it.** Each of these tools
costs an hour or two and saves ten.

Also worth building: an on-screen log of recent events, a toggle for collider rendering, and a
"screenshot with debug info" key for bug reports.

---

## 6. Editor setup

Whatever editor you use, get these three working on Day 3:

- **Go to definition** and **find all references**, driven by `compile_commands.json`.
- **Inline errors** as you type, so you find mistakes before building.
- **Format on save** with clang-format, so you never think about formatting again. Add a
  `.clang-format` file and forget it exists.

If you are on VS Code, the C/C++ or clangd extension plus a `launch.json` for gdb covers
everything. Being able to hit one key to build and debug removes a small amount of friction 50
times a day.

---

## 7. Assertions and defensive programming

`assert(condition)` documents an assumption and crashes loudly when it is violated. Loud and
early beats silent and later, every time.

Assert on things that must be true: an index is in range, HP is within bounds, a pointer is not
null before use, an enum has a handled value. Do **not** assert on things that can legitimately
happen (a missing file, bad user input) - handle those.

Remember `assert` disappears in release builds (`NDEBUG`), so never put required side effects
inside one.

For a game, consider a soft assert: log loudly, draw a big red warning on screen, and continue.
Crashing on a content error while you are authoring 200 items is more disruptive than useful.

---

## Exercises

1. Write a program with an out-of-bounds write. Find it with gdb by stepping, then with
   AddressSanitizer. Time both.
2. Use `watch` to find which line changes a variable to a wrong value.
3. Cause a segfault deliberately, get the backtrace, and read it out loud.
4. Add a conditional breakpoint that triggers only on frame 500.
5. Build the smallest possible version of an on-screen debug overlay and use it for a day.

## Done criteria

- [ ] I reproduce before I hypothesise
- [ ] I can get a backtrace from a crash without looking up commands
- [ ] I have a sanitizer build directory and I use it regularly
- [ ] I build a tool when a manual check happens for the third time
- [ ] My editor does go-to-definition and format-on-save

## Sources

- [GDB cheat sheet](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf)
- [AddressSanitizer documentation](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- Debugging chapter of any decent systems book; the technique matters more than the tool
- [Rubber duck debugging](https://rubberduckdebugging.com/) - sounds like a joke, works
  reliably, costs nothing
