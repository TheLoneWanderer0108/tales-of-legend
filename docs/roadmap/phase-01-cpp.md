# Phase 01 - C++ fundamentals (Days 4-24)

**No graphics in this phase.** Everything is a console program. This is deliberate: a
window, a game loop and a render call would triple the number of things that can go wrong
while you are still learning what a reference is.

By Day 24 you will have a complete, playable, text-only turn-based battle. That means the
hardest part of an RPG - the *logic* - already works before you draw a single pixel.

Work in `sandbox/dayNN/` for the daily exercises. The capstone on Days 21-24 gets its own
folder, `sandbox/console-battle/`, and you will steal ideas (not files) from it later.

Companion reading: [learncpp.com](https://www.learncpp.com/) is the best free structured
course. Each day below names the rough chapters. Read the chapter, then do the day's
tasks without looking at it.

Research notes used in this phase:
[memory-and-ownership.md](../research/memory-and-ownership.md),
[rpg-math-and-balance.md](../research/rpg-math-and-balance.md),
[turn-based-battle-design.md](../research/turn-based-battle-design.md),
[entity-architecture.md](../research/entity-architecture.md).

---

## Block A - Basics (Days 4-10)

### Day 4 - Types, variables, and output

**Goal:** be fluent with the raw materials: numbers, text, and printing.

**New C++ concepts:** `int`, `float`, `double`, `bool`, `char`, `std::string`, `auto`,
`const`, `std::cout`, `std::cin`, integer overflow, integer vs floating-point division.

**Reading:** learncpp chapters 1-4.

**Tasks**

- [ ] Print a formatted character sheet using hardcoded values: name, level, HP/MaxHP,
      ATK, DEF, SPD. Align the columns with `std::setw`
- [ ] Compute and print HP as a percentage. Make it print `73%`, not `0%` - understand
      *why* `73 / 100` is `0` and `73.0 / 100` is not
- [ ] Read the player's name and level from `std::cin` and print the sheet with them
- [ ] Print `sizeof` for each numeric type. Print the largest `int` via
      `std::numeric_limits<int>::max()`, then add 1 to it and observe the wraparound
- [ ] Write down which type you will use for HP, for a damage multiplier, and for a
      "player is poisoned" flag, and why

**Deliverable:** `sandbox/day04/character_sheet.cpp` prints a neat sheet from user input.

**Stuck?** If `std::cin` skips your input, it is because a previous read left a newline in
the buffer. Search for `std::getline` vs `>>` and note the difference - this bites
everyone once.

**Journal:** the integer division discovery. It will bite you again in damage formulas.

---

### Day 5 - Control flow

**Goal:** make the program make decisions and repeat work.

**New C++ concepts:** `if`/`else if`/`else`, `switch`, `for`, `while`, `do while`,
`break`, `continue`, `&&`, `||`, `!`, operator precedence, scope of loop variables.

**Reading:** learncpp chapters 5-8.

**Tasks**

- [ ] Write a damage classifier: given a damage number, print `Miss`, `Graze`, `Hit`,
      `Critical` using `if`/`else if` chains
- [ ] Rewrite it with a `switch` over an `enum`-free integer "tier". Note which version
      reads better and why `switch` needs `break`
- [ ] Simulate 1000 attacks with a random damage roll (`<random>`: `std::mt19937` and
      `std::uniform_int_distribution`) and print how many landed in each tier
- [ ] Write a menu loop: print `1) Attack 2) Defend 3) Run 4) Quit`, read a choice, act,
      loop until quit. Handle invalid input without crashing or infinite-looping
- [ ] Write a nested loop that prints a 10x5 grid of `.` characters with `#` walls around
      the edge. This is your first tilemap, in text

**Deliverable:** a menu loop that cannot be broken by typing `banana`.

**Stuck?** For input validation, look up `std::cin.fail()`, `std::cin.clear()` and
`std::cin.ignore()`. The pattern of those three lines is worth memorising.

**Journal:** how did you finally stop the infinite loop on bad input?

---

### Day 6 - Functions

**Goal:** stop writing everything in `main`.

**New C++ concepts:** declaration vs definition, parameters, return values, `void`, scope
and lifetime, function overloading, default arguments, why global variables hurt.

**Reading:** learncpp chapters 2 and 11.

**Tasks**

- [ ] Take yesterday's menu program and extract at least five functions:
      `PrintMenu`, `ReadChoice`, `RollDamage`, `ApplyDamage`, `PrintStatus`
- [ ] Write `int RollDamage(int attack, int defense, int variancePercent)` and test it
      with edge cases: zero attack, defense higher than attack (should never return
      negative), huge numbers
- [ ] Write `bool IsDead(int hp)` and `int Clamp(int value, int low, int high)` yourself.
      You will use `Clamp` hundreds of times over the next eight months
- [ ] Overload `Heal` for a flat amount and for a percentage. Decide whether overloading
      or two clear names (`HealFlat`, `HealPercent`) is better here, and justify it
- [ ] Move all your declarations to `battle.h` and definitions to `battle.cpp`, build both
      with your CMake target

**Deliverable:** `main` is under 30 lines and reads like a description of the program.

**Stuck?** "Undefined reference" at link time means you declared a function but never
defined it, or you forgot to add the `.cpp` to `CMakeLists.txt`.

**Journal:** how much easier was it to find a bug once the logic was in named functions?

---

### Day 7 - `std::vector`

**Goal:** hold many things, of a number you do not know at compile time.

**New C++ concepts:** `std::vector`, `push_back`, `size`, indexing, range-based `for`,
iterators (lightly), `emplace_back`, `erase`, out-of-bounds is undefined behaviour.

**Reading:** learncpp chapter 16.

**Tasks**

- [ ] Store a party of 3 names in a `std::vector<std::string>` and print them numbered
- [ ] Store HP in a parallel `std::vector<int>`. Feel the awkwardness of keeping two
      vectors in sync - that pain is the argument for `struct`, which arrives on Day 11
- [ ] Write `int TotalHP(const std::vector<int>& hps)`. Note why the parameter is
      `const&`, even though you do not fully understand references until Day 9
- [ ] Write a function that removes all dead party members from the vector. Do it with a
      loop, get the bug where erasing while iterating skips an element, then fix it.
      Afterwards look up `std::erase_if` and see how the standard library solves it
- [ ] Index out of bounds on purpose. Then rebuild with `-fsanitize=address` and see it
      caught precisely. Remember this feeling

**Deliverable:** a party list you can add to, print, and remove dead members from.

**Stuck?** If your erase loop behaves oddly, print the index and the size every iteration.
Printing state is the debugging technique you will use most this year.

**Journal:** what did AddressSanitizer say, and what would you have done without it?

---

### Day 8 - `std::string` and text handling

**Goal:** manipulate text, because an RPG is mostly text.

**New C++ concepts:** `std::string` operations, `substr`, `find`, `size`, concatenation,
`std::string_view` (awareness only), `std::to_string`, `std::stoi`, `std::ostringstream`.

**Reading:** learncpp chapter 5 (strings) and 17.

**Tasks**

- [ ] Write `std::string WrapText(const std::string& text, int columnWidth)` that inserts
      newlines at word boundaries. This is the actual algorithm your dialogue box will need
      in Phase 4, so do it properly
- [ ] Write a typewriter effect in the console: print one character at a time with a small
      sleep (`std::this_thread::sleep_for`). This is also a Phase 4 system, prototyped in
      15 lines
- [ ] Write `std::vector<std::string> Split(const std::string& line, char delimiter)`.
      You will need this for parsing data files on Day 20
- [ ] Build a message formatter: given `"{attacker} hits {target} for {damage}!"`,
      replace the placeholders with real values
- [ ] Parse `"Goblin,12,4,2,3"` into name and four integers, with `std::stoi`. Handle a
      malformed line without crashing

**Deliverable:** `WrapText` and `Split` work correctly, including on empty strings and
strings with double spaces.

**Stuck?** Test `WrapText` on a single word longer than the column width. Decide what
should happen, then make it happen.

**Journal:** which edge case did you not think of until you tested it?

---

### Day 9 - References and pointers

**Goal:** understand the single concept that confuses beginners most, and stop guessing.

**New C++ concepts:** pass by value vs pass by reference, `&`, `const&`, `*`, `->`,
`nullptr`, dangling references, when to use which.

**Reading:** learncpp chapters 12 and 13. Do not skip these.

**Research first (30 min):** [memory-and-ownership.md](../research/memory-and-ownership.md)
sections 1-3.

**Tasks**

- [ ] Write `void HealByValue(int hp)` and `void HealByReference(int& hp)`. Call both.
      Print the result. Explain out loud why they differ
- [ ] Write `void PrintParty(const std::vector<std::string>& party)`. Remove the `const&`
      and note that it still works - then explain what changed invisibly (a copy of the
      whole vector)
- [ ] Take a pointer to a local variable, print the address with `std::cout`, dereference
      it, modify through it
- [ ] Write a function that returns a pointer to the party member with the lowest HP, or
      `nullptr` if the party is empty. Call it, check for null before using it
- [ ] Deliberately create a dangling reference: return a reference to a local variable from
      a function. Build with sanitizers and see what happens. **Write down the rule you
      just learned**
- [ ] Fill in this table in your journal, from memory, at the end of the session:

| I want to... | Use |
| --- | --- |
| read a big object without copying | |
| modify the caller's variable | |
| say "maybe there is no object" | |
| pass a small number like `int` | |

**Deliverable:** you can predict, before running, whether a function will modify its
argument.

**Stuck?** The mental model that helps: a reference is another name for the same box; a
pointer is a note saying where the box is; a value is a photocopy of the box.

**Journal:** the four-row table above. This is the most reread journal entry you will write.

---

### Day 10 - Raw memory once, then never again

**Goal:** see `new`/`delete` so that you understand what smart pointers do for you, then
adopt the habit of not using them.

**New C++ concepts:** stack vs heap, `new`, `delete`, `new[]`, leaks, double free, C
arrays and their decay, why `std::vector` exists.

**Reading:** learncpp chapter 19.

**Research first (20 min):** [memory-and-ownership.md](../research/memory-and-ownership.md)
sections 4-5.

**Tasks**

- [ ] Allocate an `int` with `new`, use it, `delete` it. Then forget the `delete` and find
      the leak with `-fsanitize=address` (or `valgrind ./prog` if you install it)
- [ ] Allocate an array with `new int[10]`, fill it, and free it with `delete[]`. Free it
      with plain `delete` instead and read the sanitizer complaint
- [ ] Write the same program with `std::vector<int>` and count how many lines of
      bookkeeping disappeared
- [ ] Write a C-style array function `int Sum(const int* data, int count)` so you
      understand what "array decays to pointer" means. This is why raylib's C API takes
      pointer + count everywhere
- [ ] **Mini project:** a stat generator. Roll a character's stats from a seed, apply a
      class modifier (Warrior +ATK, Mage +MAG, Rogue +SPD), print the sheet, and let the
      user reroll. Use only what you have learned so far

**Deliverable:** the stat generator, plus a written rule in your journal: "I use `new`
only when ___". (Correct answer by Day 18: essentially never.)

**Stuck?** If sanitizers do not report the leak, make sure you are actually running the
sanitized binary and that the leaked pointer is not still reachable at exit.

**Journal:** why does modern C++ advice say to avoid `new`?

---

## Block B - Objects and design (Days 11-17)

### Day 11 - `struct` and your first game data

**Goal:** group related data so parallel vectors die forever.

**New C++ concepts:** `struct`, member access, aggregate initialisation, default member
initialisers, `struct` containing `struct`, vectors of structs.

**Reading:** learncpp chapter 13.

**Tasks**

- [ ] Define `struct Stats { int maxHp = 1; int hp = 1; int atk = 1; int def = 0; int spd = 1; };`
- [ ] Define `struct Actor { std::string name; Stats stats; bool isEnemy = false; };`
- [ ] Rewrite Day 7's party as `std::vector<Actor>` and delete the parallel vectors
- [ ] Write free functions: `void PrintActor(const Actor&)`,
      `bool IsAlive(const Actor&)`, `void ApplyDamage(Actor&, int amount)`
- [ ] Write `int ComputeDamage(const Actor& attacker, const Actor& target)` using a real
      formula. Start with `atk * atk / (atk + def)` and try several. Compare with the
      formulas in [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)
- [ ] Print a damage table: your formula's output for attacker ATK 5-30 against target DEF
      0-20. Look for places where damage goes to zero or explodes

**Deliverable:** a `Actor`-based party, and a damage table you have actually looked at.

**Research first (25 min):** [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)
sections 1-3.

**Journal:** which damage formula felt best and why? Copy the winner into the design doc.

---

### Day 12 - Classes, constructors, encapsulation

**Goal:** make objects that cannot be put into an invalid state.

**New C++ concepts:** `class` vs `struct`, `public`/`private`, constructors, member
initialiser lists, member functions, `const` member functions, getters that earn their
existence.

**Reading:** learncpp chapters 14-15.

**Tasks**

- [ ] Convert `Actor` into a class with private `hp_` and public
      `int Hp() const`, `void TakeDamage(int)`, `void Heal(int)`
- [ ] Make it impossible for `hp_` to exceed `maxHp_` or drop below 0 - clamp inside the
      class, not at every call site. This is the entire point of encapsulation
- [ ] Write a constructor `Actor(std::string name, Stats stats)` using a member
      initialiser list. Understand why the list is preferred over assigning in the body
- [ ] Mark every member function that does not modify state as `const`. Then try to call a
      non-`const` function on a `const Actor&` and read the error
- [ ] Add `bool IsAlive() const` and remove the free function version
- [ ] Ask yourself for each getter: does the outside world need this, or am I adding it
      reflexively? Delete the ones nothing calls

**Deliverable:** an `Actor` class whose HP is impossible to corrupt from outside.

**Stuck?** "Passing const Actor as this argument discards qualifiers" means you called a
non-`const` method on a `const` object. Mark the method `const`.

**Journal:** name one bug that encapsulation makes impossible.

---

### Day 13 - RAII, destructors, and copies

**Goal:** understand object lifetime, the concept the rest of C++ is built on.

**New C++ concepts:** destructors, RAII, copy constructor, copy assignment, the rule of
zero, move semantics (awareness), `std::move`.

**Reading:** learncpp chapters 14 (destructors) and 22.

**Research first (25 min):** [memory-and-ownership.md](../research/memory-and-ownership.md)
sections 6-7.

**Tasks**

- [ ] Write a `ScopedLog` class that prints `"enter X"` in its constructor and
      `"exit X"` in its destructor. Create them in nested scopes, in a vector, inside an
      `if`. Predict the output order **before** running, every time
- [ ] Explain in your journal how this class is exactly the pattern raylib's
      `InitWindow`/`CloseWindow` pairs want to become
- [ ] Add print statements to the copy constructor of a class and count how many copies
      happen when you `push_back` an object into a vector, then when you `reserve` first
- [ ] Pass an object by value to a function and count the copies. Change to `const&` and
      count again
- [ ] Use `std::move` when pushing a large object into a vector and observe the difference
- [ ] Write the "rule of zero" rule in your journal: if your class owns no raw resource,
      write no destructor, no copy constructor, no assignment operator

**Deliverable:** you can predict destructor order in nested scopes without running.

**Stuck?** Objects are destroyed in reverse order of construction. Vectors destroy their
elements when they die, and may move them when they grow.

**Journal:** where in your game will RAII save you? (Hint: textures, sounds, files.)

---

### Day 14 - Inheritance and polymorphism

**Goal:** one interface, many behaviours - the tool that makes skills and states pluggable.

**New C++ concepts:** base and derived classes, `virtual`, `override`, pure virtual
functions, abstract base classes, virtual destructors, dynamic dispatch, `dynamic_cast`
(and why it is a smell).

**Reading:** learncpp chapters 24-25.

**Research first (30 min):** [entity-architecture.md](../research/entity-architecture.md)
sections 1-3.

**Tasks**

- [ ] Define an abstract `Skill` with
      `virtual void Execute(Actor& user, Actor& target) = 0;`,
      `virtual std::string Name() const = 0;` and a `virtual ~Skill() = default;`
- [ ] Implement three: `Attack`, `Fireball` (costs FP, ignores some DEF),
      `Heal` (targets an ally)
- [ ] Store them as `std::vector<std::unique_ptr<Skill>>` (you will meet `unique_ptr`
      properly on Day 18; for today, use `std::make_unique` and move on)
- [ ] Loop over the vector and call `Execute` through the base pointer. Watch the correct
      derived version run. That is dynamic dispatch
- [ ] Now **remove** the `virtual` from the destructor, delete through a base pointer, and
      run with sanitizers. Read the report. Never forget this
- [ ] Write in your journal what a vtable is, in one sentence

**Deliverable:** a skill list you can iterate and execute polymorphically.

**Stuck?** "Cannot allocate an object of abstract type" means you forgot to implement a
pure virtual function in the derived class - the compiler names it in the error.

**Journal:** what happened without the virtual destructor?

---

### Day 15 - When inheritance is the wrong answer

**Goal:** learn the counterweight to yesterday, before deep hierarchies wreck your codebase.

**New C++ concepts:** composition over inheritance, object slicing, fragile base class,
"is-a" vs "has-a", data-driven alternatives to subclassing.

**Reading:** Game Programming Patterns - Component chapter.

**Research first (30 min):** [entity-architecture.md](../research/entity-architecture.md)
sections 4-6.

**Tasks**

- [ ] Design, on paper, an enemy hierarchy with inheritance:
      `Enemy -> FlyingEnemy -> FlyingFireEnemy -> FlyingFireBossEnemy`. Then add the
      requirement "a grounded fire boss that sometimes flies". Watch it fall apart
- [ ] Redesign it with composition: one `Enemy` class holding
      `bool canFly; Element element; std::vector<std::unique_ptr<Skill>> skills; AiProfile ai;`
- [ ] Prove object slicing to yourself: store derived objects in a `std::vector<Base>`
      (not pointers), call a virtual function, watch the base version run
- [ ] Write your rule in [../design/architecture.md](../design/architecture.md) as decision
      **D002**: where you will allow inheritance (interfaces with behaviour: `Skill`,
      `GameState`, `AiBehaviour`) and where you will not (enemy types, item types)
- [ ] Convert the three skills from Day 14 into data where possible: could `Fireball` be
      a `Skill` **struct** with fields (power, element, cost, target rule) instead of a
      subclass? Try it. Note which approach you would rather add 40 skills to

**Deliverable:** decision D002 written down with reasons, and a composed `Enemy` design.

**Journal:** the moment the inheritance tree became absurd.

---

### Day 16 - Maps, sets, and sorting

**Goal:** look things up by name and order things by rules.

**New C++ concepts:** `std::map`, `std::unordered_map`, `std::set`, `operator[]` inserting
silently, `find`/`contains`, `std::sort` with a comparator, iterator invalidation.

**Reading:** learncpp chapter 16, plus cppreference for each container.

**Tasks**

- [ ] Build `std::unordered_map<std::string, Stats>` as an enemy database keyed by name.
      Look up `"Goblin"`. Look up `"Grblin"` with `operator[]` and notice it silently
      creates an empty entry - then fix it with `find` or `contains`
- [ ] Build an inventory as `std::unordered_map<std::string, int>` (item name to count).
      Write `Add`, `Remove` (removing the key at zero), and `Count`
- [ ] Sort a party by SPD descending with `std::sort` and a lambda comparator. This is
      literally your battle turn order
- [ ] Handle SPD ties deterministically (tie-break by index) and explain why determinism
      matters for a game you want to debug
- [ ] Compare `std::map` and `std::unordered_map`: print iteration order of both. Write
      down when you would pick each

**Deliverable:** a turn order function `std::vector<int> TurnOrder(const std::vector<Actor>&)`.

**Stuck?** A lambda comparator looks like
`[](const Actor& a, const Actor& b) { return a.Spd() > b.Spd(); }`. If `std::sort` crashes,
your comparator is probably not a strict weak ordering (using `>=` instead of `>`).

**Journal:** the `operator[]` insertion trap. It will get you again in a save file loader.

---

### Day 17 - Multi-file projects for real

**Goal:** organise a growing codebase so builds stay fast and headers stay clean.

**New C++ concepts:** header/source split, `#pragma once`, the one-definition rule,
forward declarations, `inline`, circular include problems, include-what-you-use.

**Reading:** learncpp chapter 2 (multi-file) and 7.

**Tasks**

- [ ] Split everything you have written into: `actor.h/.cpp`, `stats.h`, `skill.h/.cpp`,
      `battle.h/.cpp`, `text.h/.cpp`, `main.cpp`
- [ ] Add `#pragma once` to every header. Remove it from one and see the redefinition
      error to understand what it prevents
- [ ] Create a deliberate circular include (`actor.h` includes `skill.h` which includes
      `actor.h`) and fix it with a forward declaration `class Actor;`
- [ ] Move `#include <vector>` etc. out of headers where a forward declaration suffices.
      Note that headers should include only what their declarations need
- [ ] Update `CMakeLists.txt` to list your sources. Learn why `file(GLOB ...)` is
      convenient and why it is discouraged (CMake will not notice new files)
- [ ] Time a full rebuild vs a rebuild after touching one `.cpp`. Then touch a widely
      included header and time it again. This is why header hygiene matters

**Deliverable:** a clean multi-file build, no header including more than it needs.

**Stuck?** "Incomplete type" means a forward declaration is not enough there - that code
needs the full definition, so include the header in the `.cpp` (not the `.h`).

**Journal:** how much slower was the rebuild after touching the shared header?

---

## Block C - Modern C++ (Days 18-20)

### Day 18 - Smart pointers and ownership

**Goal:** never write `delete` again, and always know who owns what.

**New C++ concepts:** `std::unique_ptr`, `std::make_unique`, move-only types,
`std::shared_ptr`, `std::weak_ptr`, raw pointers as non-owning observers.

**Reading:** learncpp chapter 22.

**Research first (30 min):** [memory-and-ownership.md](../research/memory-and-ownership.md)
sections 8-10.

**Tasks**

- [ ] Convert the `Skill` list to `std::vector<std::unique_ptr<Skill>>` properly, using
      `std::make_unique`
- [ ] Try to copy a `unique_ptr`. Read the error. Then move it with `std::move` and watch
      the source become null
- [ ] Write a function taking `Skill*` (non-owning) and call it with `skill.get()`. Write
      the rule: **owners hold `unique_ptr`, users take raw pointers or references**
- [ ] Build a case where `shared_ptr` seems right (two systems both needing an object
      alive), then find a design where one clear owner works instead. Prefer the second
- [ ] Create a `shared_ptr` cycle (A holds B, B holds A) and prove the leak with
      sanitizers. Fix it with `weak_ptr`
- [ ] Draw the ownership graph for your future game in
      [../design/architecture.md](../design/architecture.md): who owns actors, skills,
      textures, the party, the map

**Deliverable:** zero `new`/`delete` in your code, plus a written ownership graph.

**Journal:** who owns the party during a battle? (The answer that avoids pain: not the
battle.)

---

### Day 19 - Enums, `const` correctness, `optional`, `variant`

**Goal:** encode meaning in types so the compiler catches your mistakes.

**New C++ concepts:** `enum class`, scoped enumerators, `switch` exhaustiveness warnings,
`const` correctness, `std::optional`, `std::variant`, `std::pair`, structured bindings.

**Reading:** learncpp chapters 13 (enums) and 12 (const), cppreference for `optional`.

**Tasks**

- [ ] Replace every magic integer and string with `enum class`: `Element`, `TargetType`,
      `BattleResult`, `StatusEffect`, `Command`
- [ ] `switch` over an `enum class` without a `default` and omit one case. See the
      `-Wswitch` warning. That warning is a free bug-finder every time you add an enum value
- [ ] Return `std::optional<int>` from a function that parses a number from text and can
      fail. Consume it with `if (auto n = Parse(s))`
- [ ] Replace a "returns -1 on failure" function with `std::optional`. Note how the caller
      can no longer forget to check
- [ ] Use `std::variant` for something genuinely alternative, e.g. an item effect that is
      either a heal amount, a status cure, or a stat buff. Visit it with
      `std::visit` or `std::get_if`
- [ ] Sweep your codebase for missing `const`: every read-only parameter and every
      non-mutating member function

**Deliverable:** no magic numbers, no sentinel error values, `-Wall -Wextra` clean.

**Journal:** which enum replaced the worst magic number?

---

### Day 20 - Lambdas, algorithms, and file I/O

**Goal:** the last tools you need before the capstone, and the ability to load data files.

**New C++ concepts:** lambdas, captures (`[]`, `[=]`, `[&]`, `[this]`), `std::function`,
`std::find_if`/`any_of`/`count_if`/`accumulate`/`transform`, `std::ifstream`,
`std::getline`, `std::ofstream`.

**Reading:** learncpp chapters 20 and 28.

**Tasks**

- [ ] Rewrite three hand-written loops with algorithms: "is anyone alive"
      (`std::any_of`), "find lowest HP ally" (`std::min_element`), "total party HP"
      (`std::accumulate`)
- [ ] Store a callback in `std::function<void(Actor&)>` and call it. Understand that this
      is how your event and menu systems will work later
- [ ] Demonstrate the capture-by-reference dangling trap: a lambda capturing a local by
      reference, stored and called after the local dies. Sanitizers will catch it
- [ ] Write `assets/data/enemies.csv` by hand with 5 enemies:
      `name,maxHp,atk,def,spd,xp`
- [ ] Write a loader: `std::vector<EnemyDef> LoadEnemies(const std::string& path)` using
      `ifstream` + `getline` + your Day 8 `Split`. Handle: missing file, blank lines,
      comment lines starting with `#`, wrong field count. Report errors with the line
      number - your future self will thank you
- [ ] Write a saver that writes the file back out, and confirm a load-save round trip
      produces an identical file

**Deliverable:** enemies come from a data file, not from code. Changing a stat needs no
recompile.

**Stuck?** If the file will not open, print the path you are using and your working
directory - relative paths resolve from where you *ran* the binary, not where it lives.
Remember this; it causes the same confusion with raylib assets in Phase 2.

**Journal:** how did it feel to tune an enemy without recompiling?

---

## Block D - Capstone: console battle (Days 21-24)

Four days to build a complete, playable, text-only turn-based battle. Everything you have
learned, used at once. **This is the single most important milestone in Phase 1**, because
the systems you get right here carry straight into Phase 5.

Work in `sandbox/console-battle/`. Give it its own CMake target.

Research first, before Day 21:
[turn-based-battle-design.md](../research/turn-based-battle-design.md) in full, and
[status-effects.md](../research/status-effects.md) sections 1-3.

### Day 21 - Design and data

**Goal:** define the shape of the battle before writing the loop.

**Tasks**

- [ ] Write the battle flow on paper as a list of phases:
      `StartBattle -> RoundStart -> DecideTurnOrder -> ActorTurn (repeat) -> RoundEnd -> CheckEnd`
- [ ] Define your types: `Stats`, `Actor`, `SkillDef`, `ItemDef`, `Battle`
- [ ] Decide and write down: is turn order recomputed each round, or fixed at battle
      start? (Recomputing supports SPD buffs, which you will want)
- [ ] Load 3 heroes and 2 enemies from data files (`heroes.csv`, `enemies.csv`,
      `skills.csv`)
- [ ] Print the initial battle state clearly: both sides, HP bars made of `#` and `-`,
      FP, and any statuses
- [ ] Write `BattleResult CheckEnd(const Battle&)` returning `Ongoing`, `Victory`, or
      `Defeat`

**Deliverable:** the battle sets up from data files and prints a readable board. No turns
yet.

### Day 22 - The turn loop

**Goal:** a battle you can actually win and lose.

**Tasks**

- [ ] Implement the round loop with turn order by SPD
- [ ] Player turn: a menu (`Attack`, `Skill`, `Item`, `Defend`), target selection with
      validation (cannot target the dead, cannot heal an enemy)
- [ ] Enemy turn: pick a random living hero and attack (real AI comes tomorrow)
- [ ] Damage with your Day 11 formula plus variance and a 5% critical hit chance
- [ ] `Defend` halves incoming damage until the actor's next turn - which forces you to
      handle "state that expires", a concept you will reuse constantly
- [ ] Print a combat log line for every action: who, what, to whom, for how much
- [ ] Detect victory and defeat, print a summary, and offer a rematch

**Deliverable:** a complete battle, start to finish, that you can win and lose.

**Stuck?** If actors act twice or get skipped, your turn order indices are going stale
when someone dies. Store IDs, not indices, or re-validate before each turn. This exact bug
will greet you again in Phase 5 - solving it now is worth an hour.

### Day 23 - Items, statuses, and AI

**Goal:** the systems that make a battle a *game* rather than a coin flip.

**Tasks**

- [ ] Inventory shared by the party (`unordered_map<std::string,int>`), items loaded from
      data: `Potion` (heal 20), `Ether` (restore 10 FP), `Antidote` (cure poison),
      `Bomb` (12 damage to all enemies)
- [ ] Implement 2 skills with FP cost, one single-target, one all-targets
- [ ] Implement `Poison` (damage at end of turn, 3 turns) and `AttackUp`
      (+50% ATK, 3 turns) via a small
      `struct ActiveStatus { StatusEffect kind; int turnsLeft; int magnitude; };`
      and a `std::vector<ActiveStatus>` on each actor
- [ ] Tick statuses at the correct moment. Decide and document *when*: end of the
      afflicted actor's turn, or end of the round? Both are valid; be consistent
- [ ] Enemy AI with a weighted decision: 60% attack the lowest-HP hero, 25% use a skill if
      FP allows, 15% attack randomly. Bosses heal themselves below 30% HP
- [ ] Add a `--seed N` command line argument so you can replay an exact battle. This is
      the best debugging tool a random-heavy game can have

**Deliverable:** a battle with meaningful choices, where a wrong item use can lose it.

**Research first (20 min):** [enemy-ai.md](../research/enemy-ai.md) sections 1-2.

### Day 24 - Refactor, balance, and retrospective

**Goal:** turn working code into code you would be happy to extend, and reflect properly.

**Tasks**

- [ ] Refactor: no function longer than 40 lines, no file longer than 300, names that read
      as sentences (`ApplyEndOfTurnStatuses`, not `doStuff2`)
- [ ] Extract the battle into a `Battle` class with an explicit
      `enum class Phase` member. You have just written your first state machine - notice
      that, because Phase 3 formalises it
- [ ] Write a headless auto-battle mode (both sides AI) that runs 500 battles and reports
      the hero win rate. Tune stats until it sits between 60% and 80%
- [ ] Add 5 unit-test-style checks in a `Tests()` function using `assert`: damage never
      negative, HP never exceeds max, poison expires after exactly 3 ticks, a dead actor
      never acts, inventory count never goes below zero
- [ ] Run under `-fsanitize=address,undefined` one final time. Fix everything it says
- [ ] Update [../design/architecture.md](../design/architecture.md) with what you built
- [ ] Write a real retrospective in the journal: what took longest, what you would design
      differently, which C++ feature you still do not trust

**Deliverable:** a clean, tested, balanced console RPG battle, committed.

---

## Phase 01 exit checklist

Be honest. Weakness here compounds for the next 146 days. If you fail two or more,
spend 2-3 extra days on those topics before Phase 2 - that is a good investment, not a
delay.

- [ ] I can explain the difference between a value, a reference, and a pointer without
      hedging
- [ ] I know when to use `const&` for a parameter and when to pass by value
- [ ] I can write a class that keeps its own invariants
- [ ] I can explain when to use inheritance and when to use composition, with an example
      of each from my own code
- [ ] I use `unique_ptr` for ownership and raw pointers/references for observation
- [ ] I can split a program across headers and sources and fix a circular include
- [ ] I can read a file, parse it, and report a useful error on a malformed line
- [ ] I have a working console battle with items, statuses, and AI
- [ ] I can debug with gdb and sanitizers instead of only guessing
