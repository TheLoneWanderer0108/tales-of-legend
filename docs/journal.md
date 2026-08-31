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