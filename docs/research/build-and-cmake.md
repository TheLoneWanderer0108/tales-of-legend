# Build systems, compilation, and CMake

**Used on:** Days 1-2 (setup), Day 17 (multi-file), Day 44 (adding a dependency),
Day 167 (release packaging).

Why this matters: for the next eight months, every single thing you do begins with a build. An
hour spent understanding the build now saves you a hundred confused minutes later, because
build errors that look like mysteries are almost always one of five specific things.

---

## 1. What actually happens when you compile

The pipeline, in order. You must be able to name the stage that produced any given error.

1. **Preprocessor** - handles `#include` (literally pasting file contents in), `#define`,
   `#ifdef`. Output is one enormous "translation unit" of pure C++.
2. **Compiler** - parses that translation unit, type-checks it, and emits machine code into an
   object file (`.o`). It works on **one translation unit at a time** and knows nothing about
   the others.
3. **Linker** - takes all the object files plus libraries, resolves every symbol (function and
   variable name) to an address, and produces the executable.

Try this: `g++ -E main.cpp | wc -l`. A file with a single `#include <vector>` expands to tens
of thousands of lines. That is why headers affect build times.

**Questions to answer**

- Why is a missing semicolon a compile error but a missing function body a link error?
- If two `.cpp` files both include the same header, how many times is that header's text
  processed?
- Why can the compiler check a call to a function it has only seen declared, not defined?

---

## 2. Reading errors and the five common causes

Beginners read the last line of an error. Read the **first** line - errors cascade, and the
first one is usually the real problem.

Nearly every build failure you will hit is one of these:

| Symptom | Almost always means |
| --- | --- |
| `undefined reference to X` (link stage) | Declared but never defined, or the `.cpp` was not added to the build |
| `X was not declared in this scope` | Missing `#include`, or a typo, or wrong namespace |
| `redefinition of X` | Header included twice without `#pragma once`, or a definition in a header |
| `incomplete type` | A forward declaration where the full definition is required |
| Template errors, hundreds of lines | Look for the *first* mention of your own file |

**Key terms to search:** translation unit, one-definition rule (ODR), symbol, external
linkage, name mangling.

---

## 3. Why a build system exists

Compiling by hand works for two files and collapses at twenty. A build system:

- knows which files depend on which, so it rebuilds only what changed;
- stores your flags so every build is identical;
- finds and links libraries;
- generates project files for different compilers and platforms.

CMake is not a build system. It is a build system **generator** - it produces Makefiles (or
Ninja files) that then do the work. This two-step nature explains why you run two commands:
`cmake -S . -B build` configures, `cmake --build build` builds.

**Questions to answer**

- What is in the `build/` directory, and why is it git-ignored?
- Why is configuring separate from building?
- What is the difference between `make` and `ninja`, and how do you tell CMake which to use?

---

## 4. The CMake concepts you actually need

You need maybe eight commands for this entire project. Ignore everything else you find online.

- `cmake_minimum_required(VERSION x.y)` - always first.
- `project(name CXX)` - names the project and enables the C++ toolchain.
- `add_executable(target src/a.cpp src/b.cpp)` - what to build.
- `target_link_libraries(target PRIVATE lib)` - what to link.
- `target_include_directories(target PRIVATE dir)` - where to find headers.
- `target_compile_options(target PRIVATE -Wall -Wextra)` - warnings.
- `set(CMAKE_CXX_STANDARD 20)` - language version.
- `include(FetchContent)` plus `FetchContent_Declare`/`MakeAvailable` - dependencies.

**Think in targets, not in global settings.** `target_*` commands attach properties to one
target; the older global commands (`include_directories`, `add_definitions`) leak everywhere
and are the main reason old CMake advice is bad advice.

`PRIVATE` vs `PUBLIC` vs `INTERFACE` matters once you have libraries: `PRIVATE` means "I need
this to build", `PUBLIC` means "I need it and so does anyone who links me", `INTERFACE` means
"only my users need it".

**Questions to answer**

- Why is `target_link_libraries(game PRIVATE raylib)` enough to get raylib's headers too?
- What does `CMAKE_EXPORT_COMPILE_COMMANDS` produce and which tool consumes it?
- Why is `file(GLOB ...)` for source lists discouraged?

---

## 5. Dependencies with FetchContent

`FetchContent` downloads a dependency's source at configure time and builds it as part of your
project. For a solo project this is the least painful option: no system packages, no version
mismatches, and it works identically on any machine.

Pin an exact tag, never a branch. `GIT_TAG 6.0` is reproducible; `GIT_TAG master` means your
build can break overnight because someone else committed.

Set a dependency's options **before** `FetchContent_MakeAvailable`, because that call is when
its `CMakeLists.txt` runs:

```cmake
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
```

The first configure is slow because it clones and builds raylib. Later configures reuse it.
If you ever need to force a fresh fetch, delete `build/_deps`.

**Questions to answer**

- Why pin a tag instead of tracking a branch?
- Where does the fetched source live, and how do you read raylib's own source from there?
      (Reading raylib's source when you are confused is a legitimate and underused technique.)
- What is the difference between `find_package` and `FetchContent`, and why does the raylib
  template try `find_package` first?

---

## 6. Build types and flags

| Type | Flags | Use for |
| --- | --- | --- |
| `Debug` | `-g -O0` | Debugging, stepping in gdb |
| `Release` | `-O2`/`-O3 -DNDEBUG` | Shipping, performance measurement |
| `RelWithDebInfo` | `-O2 -g` | Profiling |
| Sanitizer build | `-g -O1 -fsanitize=address,undefined` | Hunting memory bugs |

Configure with `-DCMAKE_BUILD_TYPE=Debug`. Keep separate build directories
(`build-debug`, `build-release`) so switching does not force a full rebuild.

Warnings worth having from day one: `-Wall -Wextra`. Consider adding `-Wshadow` and
`-Wconversion` once you are comfortable - they are noisy but they catch real bugs. Treat
warnings as errors in your own code (`-Werror`) only if you have the discipline for it; it is a
good habit but frustrating at first.

**Note:** `assert` is disabled by `NDEBUG` in release builds, so never put logic with side
effects inside an assert.

---

## Exercises

1. Compile a two-file program by hand: preprocess only (`-E`), compile only (`-c`), then link.
   Look at each intermediate output.
2. Produce each of the five common errors from section 2 deliberately, and note the exact
   wording so you recognise it instantly later.
3. Add a second executable target to your `CMakeLists.txt` (your sandbox programs) and build
   both.
4. Add a dependency with `FetchContent` (`nlohmann/json`) and use one function from it.
5. Time a full rebuild, then a rebuild after touching one `.cpp`, then after touching a widely
   included header. Write the three numbers down.

## Done criteria

- [ ] I can name which stage produced any given error
- [ ] I can add a source file, a target, and a dependency to CMake without looking it up
- [ ] I know what `PRIVATE` means and why I use it
- [ ] I have separate debug, release, and sanitizer build directories
- [ ] I understand why header changes cost more than source changes

## Sources

- [CMake tutorial (official)](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) -
  skim only the first three steps
- [An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/) - the best
  free guide, and it explicitly teaches target-based CMake
- [raylib CMake project template](https://github.com/raysan5/raylib/blob/master/projects/CMake/CMakeLists.txt)
- `man g++`, specifically the "Options for Debugging" and "Warning Options" sections
