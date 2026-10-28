# Dev journal

Two lines a day. No more, no less. Line 1: what broke or confused you. Line 2: what you
learned or how you fixed it. Newest entries at the bottom so it reads as a story.

This file is not documentation. Nobody else will read it. Be blunt.

---

## Template

```
### Day N - YYYY-MM-DD
Broke: I could not figure out why the player jittered when moving diagonally.
Learned: normalise the direction vector before multiplying by speed, otherwise diagonal
movement is 1.41x faster.
```

---

## Day 1 - 8/11/2026
Broke: linking libraries was difficult, ended up getting a bunch of errors beacuse i returned nothing
Learned: always return something in c++, ensure to specify flags with -, linking is esentially unifing two files together, in my own words, compile errors are related to the code itself not being able to be build, linking can be build with warnings but does not gurantee the program will work.

## Day 2 - 8/12/2026
Broke: Broke my cmake file because of syntax issues
Learned: The importance of cmake, debug and release, how cmake compiles files and uses timestamp to avoid compiling unchanged files. to answer the question -B tells cmake where he generates a bucnh of files, most importantly the binary of the game.

## Day 3 - 8/13/2026
Broke: I made a buggy app to utilize debugging tools and see the errors
Learned: How to use debug tools, how to find mistakes related to memory problems, how to add it to cmake and when to use each one. To answer the question, using the sanitizefound it fasterbuts more costly comapred to gdb which is more manual, in terms of my workflow i will have 2 builds, one for debugging and another the actual release ready game.

## Day 4 - 8/14/2026
Broke: nothing broke, it was just learning types, sizes and calculating with ints and floats.
Learned: I learned that if you do a division it should be with a double since with an int it gives whole numbers and truncates them, ex, 75/100 would be 0 beacuse 100 fits 0 times in 75, with decimals this problem is solved.

## Day 5 - 8/15/2026
Broke: I forgot to initialize counters outside of a loop, which resulted in them being reset, tried printing some
columns but they did not appear.
Learned: Always declare counters outside the loops, and col and rows dnt print anything unless you specify
which is why cols where not showing anything, how to get random numbers generated, and the uses of
if/else and switch statements, ifs are used for conditionals that are in ranges and switch when its much more shorter and used for eact values. i fixed the banana thing for do while loops by flagging the error, clearing the flag and then clearing the buffer.

## Day 6 - 8/16/2026
Broke: Some health variables overwrote themselves, some logic didnot apply as i was doing incorectly, some logical issues
Learned: The importance of clamps, declare variables twice for health, one for current health and one for max so you can have
a max range when passing to functions and have the other to be dynamic beacuse it changes, same with enemy health,
learned the use of void, use it when you dont want to return anything, learned more about fucntions and fucntion overload.
how much easier was it to find a bug once the logic was in named functions? easier since the job was segmentated, just had
to look at that piece of code.

## Day 7 - 8/17/2026
Broke: Learned that indexes cannot be negative when being itirated, which makes sense since index starts t 0, anything that broke was intentional
Learned: Learned about const and vectors, i can know there sizes with .size(), learned how to delete them and the meaning behind &,
which is to not make a copy of he vector instead use the original. what did AddressSanitizer say, and what would you have done without it?
sanitizer told me about trying to look at an index that did not exist and aborted the loop, killing the program.

## Day 8 - 8/19/2026
Broke: When trying to do the typewriter it paaused and then gave me the string, some variables left over wich i forgot to add.
Learned: cout keeps a buffer that only appears on screen when a new line happens or the program ends, flush forces it to show its contents
as they come, learned to use vectors better, adn the keyword auto which allows the compiler to figure the from whateevr is on the right side =.
the edge case was if the data presented mas malformed, so i used try and catch for the first time.

## Day 9 - 8/22/2026
Broke: Some stuff was suppose to break, it was part of learning
learned: the use of pointers
| I want to... | Use |
| --- | --- |
| read a big object without copying | const& |
| modify the caller's variable | & |
| say "maybe there is no object" | * |
| pass a small number like `int` | its so small is not really needed &|

## Day 10 - 8/22/2026
Broke: Forgot to clean the memory after using it
Learned: About the usage of pointer and memory allocation, "I use `new`
only when i want to allocate and initialze memory for a variable, after this day never again".
why does modern C++ advice say to avoid `new`? Because allocating memory yourself can be dangerous and have
huge risks in your code, also forces you to use delete.

## Day 11 - 8/24/2026
Broke: Nothing really broke, just have tomake sure to use the necessary {} when nesting a vector.
Learned: Game design for damage and defense, how to organize my structs, and think about game balance
which damage formula felt best and why? Copy the winner into the design doc. my game will work with a
bit larger numbers the formula is atk²/(atk+def), this allows for defense to be meaningful
but not overpowered same thing with attack, also no huge spikes with 0, 0 damage is allowed as a way to show how powerful
you become, also i want to add variance and crits and this gives me room for both.

## Day 12 - 8/25/2026
Broke: Forgot that class privates the variables, so i have to set a constructor for it
Learned: The importance of constructors and why i dont need setters everywhere,
name one bug that encapsulation makes impossible. a function accidentally modifying a value like hp

## Day 13 - 8/26/2026
Broke: No bugs really, today was more about best practices and such.
Learned: The importance of deconstructors, why use templates and why copying large vectors is not such a good idea;
Write the "rule of zero" rule in your journal: you should not manually define special member functions such as destructors, copy/move constructors, or copy/move assignment operators. Instead, rely on the compiler-generated defaults and let member objects (like std::string, std::vector, std::unique_ptr) handle resource management for you. In raylib, InitWindow is responsible for initializing system resources needed for rendering (i.e., creating a window), while CloseWindow cleans up those resources when they are no longer needed. In both cases, the constructor signifies the start of a resource management procedure (opening a window, entering a log scope), while the destructor ensures that resources are properly released or cleaned up (closing the window, exiting a log scope).

## Day 14 - 8/27/2026
Broke: i need to initialzed the unique pointers when using them
Learned: The importance of virtual when dealing with inheritance,  If you delete or destroy through a base pointer — including unique_ptr<Base> — the base destructor must be virtual. A vtable is a per-class table of function pointers that lets C++ call the correct overridden function — and the correct destructor — at runtime when using a base pointer.

## Day 15 - 8/28/2026
Broke: I kept passing the wrong thing into execute — Enemy instead of Actor — and forgot the third argument (kind) in my skill loop.
Learned: Deep inheritance falls apart when enemy traits combine (flying + fire + boss + sometimes grounded). One Enemy struct with flags, 
element, and a skill list scales better; skills are data rows with a kind field, not a class per spell.

## Day 16 - 8/29/2026
Broke: Turn order / sorting indices confused me; operator[] silently added "Grblin" to the enemy DB.
Learned:  unordered_map for fast lookup, map when you need sorted keys; sort slot numbers, not the party.

## Day 17 - 8/31/2026
Broke: CMake wanted Ninja but build/ had old Makefiles; also wrote execute instead of Skill::execute and the IDE was right.
Learned: Touching main.cpp only recompiled 1 file. Touching a shared header would recompile all of them. That's why #pragma once and lean 
headers matter.

## Day 18 - 9/2/2026
Broke: unique_ptr would not copy into the vector without std::move; tried skill.Name() on a pointer instead of skill->Name(); shared_ptr cycle leaked 64 bytes until I used weak_ptr on the back-link.
Learned: Owners hold unique_ptr, users borrow with .get() or references. GameSession will own the party — BattleState only uses it, never owns it, or the party dies when the battle ends. shared_ptr cycles leak silently; one clear owner is almost always the better design.

## Day 19 - 9/6/2026
Broke: Command vs int in switch; -1 inside optional; get_if<enumerator> instead of get_if<Type>; naming the get_if result effect shadowed the variant.
Learned: Command replaced the worst magic numbers (menu 1–4). optional + if (auto n = …) beats sentinel -1. get_if gives a pointer — * / -> for the data. const marks “I won’t modify this.”

## day 20- 9/9/2026
Broke: I kept treating every algorithm like std::sort, mixed up linenumber with the actual line text, and thought a stored lambda's locals died when the lambda "finished" instead of when the `{ }` around x ended.
Learned: any_of / min_element / accumulate each have their own lambda contract; capture [&] on a stored callback is the same dangling as a pointer to a local. Tuning a goblin without recompiling felt like the stats finally lived outside the code — change the csv, run again, done. New monster with the same fields is a new row, not a new if.

## Day 21 - 9/11/2026
Broke: CMake flipped between Ninja and Makefiles on the same build/; looping a map with skills[i] inserted empty keys and printed a wall of 0s; CheckEnd returned victory/defeat on the first dead actor and I called it inside the board loop so "fight continues" printed five times.
Learned: CSV is defs only — CurrentHp/Fp, statuses, defending are set in code; skills live in unordered_map keyed by id (fill the SkillDef then skills[e.ID] = e); actors go in Battle.actors as a vector with BattleId as identity; hero skills need Split on ',' then ';' on parts[7]; enum class needs SideToString to print; CheckEnd scans everyone for AlliesAlive/FoesAlive then decides once after the board.d

## Day 22 - 9/13/2026
Broke: Turn order felt impossible until I mixed up the sticky-note list with the board — shadowed `order`, treated seats like actors, put `cin` inside the print loop, and for Defend I checked the attacker and cleared the flag on hit instead of the target lasting until their next turn.
Learned: `TurnOrder` returns seats; person is `battle.actors[seat]`. Target pick is two phases (build living-foe list, then validate). Damage is atk²/(atk+def) then ±10% then 5% crit; Defend halves on the *target* and expires at *their* turn start. Outer `while (CheckEnd == ongoing)` with a fresh `TurnOrder` each round is the fight spine — empty target lists must be guarded or the menu spins forever.

## Day 23 - 10/4/2026
Broke: Guarded the rolled skill *index* as if it meant "no skills"; treated antidote target as `person.statuses` instead of living allies; bomb at 30 wiped 15–20 HP foes and looked like antidote ended the fight.
Learned: Empty list ≠ id 0. Antidote picks an ally seat then erases poison. One shared `battle.rng` + `--seed` makes crits/AI replayable — `argv[i] == "--seed"` compares pointers, use `std::string(argv[i])`.

## Day 24 - 10/14/2026
Broke: Extracted `RunFoeTurn` by value so damage vanished; left duplicate functions in `main` after moving to `.cpp`; forgot `.cpp` entries in CMake; GCC ASan link failed without `libasan` (clang worked).
Learned: Refs for mutable battle state. Headers via `#include`, only `.cpp` in CMake. `Phase` on `Battle` + `RunBattle` is the first state machine. `--auto` 500 fights landed ~71% hero wins after tuning enemy CSV. Took longest: index-vs-object and pass-by-value bugs. Would design splits earlier. Still distrust: unsigned/`size_t` compares and when `operator[]` inserts into maps.

### Day 24 retrospective (fill in your own words if you want)
- What took longest: The parsing and combat logic, especially the turn order and target selection. It was tricky to get all the pieces to work together correctly.
- What you would design differently: Nothing as of now, but I would like to have a better understanding of how to structure the code and separate concerns more effectively.
- Which C++ feature you still do not trust: Unsigned integers and `size_t` comparisons, as they can lead to unexpected behavior if not handled carefully. Also, the behavior of `operator[]` in maps can be surprising since it inserts default values for missing keys.

## Day 26 - 10/17/2026
Broke: Moving with `position += 3` per frame made the speed depend on FPS (90 px/s at 30, 180 at 60, 432 at 144). Moving diagonally was about 1.41x faster, because x and y each moved `speed * dt` and the real path was the diagonal of that triangle. Normalising with no keys held divides by a length of 0. My first velocity code used `=` where it needed `+=`, so velocity never built up, and I wrote the movement into `dirX` instead of `rectX`. `velX *= 0.9f` was frame-rate dependent too. I called `clamp(rectX, ...)` and threw away the returned value, and my `clamp` returned a hard-coded 0 instead of `min`.
Learned: Movement is `position += direction * speed * dt`, with speed in pixels per second, so it's the same at any frame rate. The `if` blocks only build a direction (`dirX`, `dirY`), then I normalise it (divide by its length) and move once. Guard `length > 0` before dividing: `0.0f / 0.0f` gives NaN, NaN spreads through every calculation, and the rectangle vanishes with no error and never comes back. Acceleration changes the velocity (`velX += dirX * accel * dt`) and velocity changes the position (`rectX += velX * dt`), so velocity has to live outside the loop. A per-frame friction multiplier is frame-rate dependent, so friction has to be time-based: `velX *= std::pow(friction, dt)`, where `friction` is the fraction kept after 1 second. A function that returns a value needs its result caught (`rectX = clamp(...)`), like `float lenght = std::sqrt(...)`. `clamp` fixes the position, not the velocity, so the rectangle hung against the wall because velocity kept building. A frame that stalls for 0.5 s gives `dt = 0.5`, and the rectangle jumps 100 px in one frame. `speed * dt` keeps the total correct, but it looks like a teleport.

## Day 27 - 10/21/2026
Broke: I forgot `Window window;` so no window opened, and my first key display watched `KEY_BACKSPACE` in one row and `KEY_SPACE` in another, with every label copy-pasted so I couldn't tell the rows apart. I passed a `std::string` to `%s` in `TextFormat` instead of `.c_str()`. My menu `if` compared the text to `"KEY_UP"` instead of comparing `i` to `selected`, and drew only the selected row because I had no `else`. I wrote `options == 0` when I meant `selected == 0`, mixing up the list with the index. I copied Down's body into Up without flipping the condition, the wrap target, and the `+=`. The Up repeat only worked after I understood that the mover has to sit below every detector, because the repeat set `moveDown` after the move code had already read it.
Learned: `IsKeyDown` is true every frame while held and `IsKeyPressed` is true for exactly one frame, so a quick tap on a menu with `IsKeyDown` jumped about 6 rows (6 frames at 60 FPS) and `selected` went past 3 so the highlight vanished. `selected` is a number and `options[selected]` is the text. Check `selected <= 0` before subtracting, because on a `size_t` `0 - 1` wraps to a huge number. Key repeat is a timer that adds `dt` while the key is held, resets to 0 when released, and after it passes 0.4 fires and sets itself to 0.3, so the next one comes 0.1 s later. Each key gets its own timer, or a roll from Down to Up inherits leftover time. Detectors only set a `moveDown` flag, and one mover reads it once, so the move code exists in one place. The flag is made fresh every frame inside the loop, unlike the timer, which has to live outside. Mouse uses the same Down/Pressed split (`IsMouseButtonPressed`), and separate `if`s for left and right so both can fire in the same frame.
Action commands: timing hit = `IsKeyPressed` plus a clock of summed `dt` compared to a window around the target time. Mash = `IsKeyPressed` plus an `int` count and a clock, and `IsKeyDown` would count about 60 a second. Hold-and-release = `IsKeyPressed` to start, `IsKeyDown` to add `dt` to a timer, `IsKeyReleased` to read the timer, and only then reset it.

## Day 28 - Shapes, colour, and a fake battle scene

How far can you get with rectangles alone? Pretty far. Boxes, bars, a line, a triangle and a panel were enough to make a battle screen that reads clearly.

What I learned:
- y grows downward, and DrawRectangle's x and y are the top-left corner. To stand on a line, PosY = line y - box height. Going up means subtracting.
- Draw order decides what covers what. My names vanished until I drew the box first and the text after, and the overlay only dimmed the things drawn before it.
- HP bar fill = hp / maxhp, as doubles (3 / 4 as ints is 0), times the full bar width.
- DrawTriangle only draws counter-clockwise points. {x, y} has x first.
- Named colours in one place (a palette) make it easy to change a colour later.
- Two keys that share one timer reset each other. Every key needs its own.

What went wrong: I reused one timer for two keys. I also spent a long time on the triangle coordinates because I mixed up PosX (the left edge) with the middle of the box.

The half-scale test: I had to change almost every number in the file, including ones that don't look related. Hardcoded coordinates don't scale, and that is why Day 42 exists.
