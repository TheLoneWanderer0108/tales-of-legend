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
