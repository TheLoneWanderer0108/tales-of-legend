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