# Phase 00 - Setup and tooling (Days 1-3)

Goal of this phase: by the end of Day 3 you can create a C++ file, build it with one
command, run it, and step through it line by line in a debugger. Everything else in this
roadmap assumes you can do those four things without thinking.

Your machine already has what you need: **g++ 15.2**, **CMake 4.2**, **make** and
**git**. You are on WSL2 with WSLg, so graphical windows will display when Phase 2
arrives.

Research notes for this phase:
[build-and-cmake.md](../research/build-and-cmake.md),
[git-workflow.md](../research/git-workflow.md),
[debugging-and-tooling.md](../research/debugging-and-tooling.md).

---

### Day 1 - Compile something by hand

**Goal:** understand what a compiler actually does before a build system hides it from you.

**New concepts:** preprocessor, compiler, linker, translation unit, object file, warnings.

**Research first (20 min):** [build-and-cmake.md](../research/build-and-cmake.md)
sections 1-2.

**Tasks**

- [x] Create `sandbox/day01/hello.cpp` that prints your game's title
- [x] Compile it by hand: `g++ -std=c++20 -Wall -Wextra hello.cpp -o hello` and run it
- [x] Deliberately break it three ways and read each error carefully: remove a semicolon,
      misspell `std::cout`, call a function you never defined. Write down which stage
      (compile or link) produced each error
- [x] Compile with only `-c` and look at the `.o` file that appears. Try to run it. Note
      what happens
- [x] Split into two files: `main.cpp` calling `int Add(int, int)` defined in `math.cpp`,
      declared in `math.h`. Compile both, link them together
- [x] Read the whole of `g++ --help` once. Not to memorise it, just to know it exists

**Deliverable:** a two-file program you compiled and linked yourself with zero warnings
under `-Wall -Wextra`.

**Stuck?** If linking fails, the classic cause is defining the function in the header
instead of the `.cpp`, or forgetting to pass `math.cpp` on the command line.

**Journal:** what is the difference between a compile error and a link error, in your own
words?

---

### Day 2 - CMake and the project skeleton

**Goal:** one command builds the project, and raylib is fetched automatically.

**New concepts:** build systems, out-of-source builds, targets, `FetchContent`.

**Research first (25 min):** [build-and-cmake.md](../research/build-and-cmake.md)
sections 3-5.

**Tasks**

- [x] Create the directory skeleton: `src/`, `assets/`, `sandbox/`, `build/`
- [x] Write `CMakeLists.txt` **by typing it, not pasting it**, and explain each line to
      yourself out loud:

```cmake
cmake_minimum_required(VERSION 3.22)
project(tales_of_legend CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 6.0
  GIT_SHALLOW 1)
FetchContent_MakeAvailable(raylib)

add_executable(game src/main.cpp)
target_link_libraries(game PRIVATE raylib)
target_compile_options(game PRIVATE -Wall -Wextra)
```

- [x] Write a `src/main.cpp` that only prints text - **no raylib calls yet**
- [x] Configure and build: `cmake -S . -B build && cmake --build build -j`
- [x] The first build downloads and compiles raylib and takes a few minutes. Watch the
      output scroll. Subsequent builds are seconds
- [x] Run `./build/game`
- [x] Add a `Debug` and a `Release` build directory. Compare the binary sizes
- [x] Verify `build/compile_commands.json` exists (your editor uses it for autocomplete
      and error squiggles)

**Deliverable:** `cmake --build build -j && ./build/game` prints your title.

**Stuck?** If `FetchContent` fails, you probably lack network access or a git dependency.
As a fallback, comment out the raylib block and `target_link_libraries` line; you do not
need raylib until Day 25.

**Journal:** what does `-B build` do, and why is building outside the source tree the
normal practice?

---

### Day 3 - Git, the debugger, and sanitizers

**Goal:** you can save your work safely and inspect a running program.

**New concepts:** commits, `.gitignore`, breakpoints, stack frames, watch expressions,
AddressSanitizer.

**Research first (25 min):** [git-workflow.md](../research/git-workflow.md) and
[debugging-and-tooling.md](../research/debugging-and-tooling.md) sections 1-3.

**Tasks**

- [ ] Write a `.gitignore` covering `build/`, `.cache/`, `compile_commands.json`,
      editor directories, and `*.o`
- [ ] Make your first commit: `git add -A && git commit -m "day 1-3: project skeleton"`
- [ ] Practise the loop five times: change something, `git diff`, `git add -p`,
      `git commit`, `git log --oneline`
- [ ] Practise recovery: make a mess in a file, then `git restore <file>`. Then commit a
      mistake and undo it with `git revert`. Knowing you can undo removes the fear that
      makes beginners avoid experimenting
- [ ] Write a program with an obvious bug (an off-by-one loop that reads past the end of
      an array). Build with `-g -O0`
- [ ] Run it under gdb: set a breakpoint (`break main`), step (`next`, `step`), print
      variables (`print i`), inspect the stack (`bt`), continue (`continue`)
- [ ] Rebuild with `-fsanitize=address,undefined -g` and run it again. Read the sanitizer
      report and find the exact line it names
- [ ] Add a `Sanitize` build option to your `CMakeLists.txt` so you can turn this on
      whenever something behaves strangely
- [ ] Set up your editor: C/C++ extension, format-on-save (clang-format), and confirm
      go-to-definition works via `compile_commands.json`

**Deliverable:** you found a memory bug twice - once with gdb by stepping, once with
AddressSanitizer instantly. You now know which tool to reach for.

**Stuck?** If gdb will not stop at a breakpoint, you almost certainly built without `-g`
or with optimisation on.

**Journal:** which found the bug faster, and what does that tell you about your workflow
for the next 167 days?

---

## Phase 00 exit checklist

Do not move on until all of these are true.

- [ ] `cmake --build build -j` succeeds from scratch with no warnings
- [ ] I can create a new `.cpp`, add it to `CMakeLists.txt`, and build it
- [ ] I understand the difference between compile errors and link errors
- [ ] I have committed at least 3 times and know how to undo a change
- [ ] I can set a breakpoint and print a variable's value
- [ ] I can turn on sanitizers when something is misbehaving
