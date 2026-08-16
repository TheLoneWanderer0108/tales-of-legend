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

### Day 1 - 8/11/2026
Broke: linking libraries was difficult, ended up getting a bunch of errors beacuse i returned nothing
Learned: always return something in c++, ensure to specify flags with -, linking is esentially unifing two files together, in my own words, compile errors are related to the code itself not being able to be build, linking can be build with warnings but does not gurantee the program will work.

### Day 2 - 8/12/2026
Broke: Broke my cmake file because of syntax issues
Learned: The importance of cmake, debug and release, how cmake compiles files and uses timestamp to avoid compiling unchanged files. to answer the question -B tells cmake where he generates a bucnh of files, most importantly the binary of the game.

### Day 3 - 8/13/2026
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