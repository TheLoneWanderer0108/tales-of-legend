# Memory, references, and ownership

**Used on:** Days 9-10 (references and raw memory), Day 13 (RAII), Day 18 (smart pointers),
Day 40 (asset manager), Day 48 (entities), Day 72 (battle data).

Why this matters: this is the concept that separates people who "know some C++" from people who
can build a large program in it. Almost every mysterious crash in a C++ game is an ownership
mistake. Get this right and your game will be stable; get it wrong and you will spend months
chasing ghosts.

The single question to ask of every object in your game: **who owns this, and who merely looks at
it?**

---

## 1. Stack and heap

- **Stack**: automatic storage. Created when declared, destroyed at the end of its scope, in
  reverse order. Fast, and impossible to leak. Size known at compile time.
- **Heap**: dynamic storage. You (or a container) request it, and it lives until released.
  Needed when the size or lifetime is not known at compile time.

Beginners over-use the heap because tutorials teach `new` early. In modern C++ the correct default
is the **stack plus containers**, with `std::vector` handling heap allocation for you.

**Questions to answer**

- When a function returns, what happens to its local variables?
- Why is returning a pointer to a local variable always a bug?
- What actually lives on the heap when you write `std::vector<int> v;` on the stack?

---

## 2. Value, reference, pointer

The mental model worth memorising:

- A **value** is a photocopy of the box. Changing yours does not change theirs.
- A **reference** (`Actor&`) is another name for the same box. Cannot be null, cannot be
  reseated, must be bound at creation.
- A **pointer** (`Actor*`) is a note saying where the box is. Can be null, can be reassigned,
  must be checked before use.

| I want to | Use |
| --- | --- |
| Read a big object without copying | `const Actor&` |
| Modify the caller's object | `Actor&` |
| Say "maybe there is no object" | `Actor*` (checked) or `std::optional` |
| Pass a small value (`int`, `float`, `Vector2`) | by value |
| Take ownership | `std::unique_ptr<Actor>` |

Rule of thumb: **prefer references; use pointers when null is a legitimate answer.**

---

## 3. Dangling references and the lifetime rule

A reference or pointer is only valid while the thing it refers to is alive. The classic ways to
break this in a game:

- Storing a pointer to an element of a `std::vector`, then `push_back`ing. The vector may
  reallocate and every pointer into it is now garbage. **This will happen to you with entities.**
- Holding a pointer to an entity that gets destroyed later that frame.
- Capturing a local by reference in a lambda that outlives it.
- Returning a reference to a local.

Defences, in order of preference: store IDs or indices instead of pointers; `reserve` the vector
up front so it never reallocates; look up the pointer fresh each time you need it, and never
store it across frames.

**Questions to answer**

- Which `std::vector` operations invalidate pointers to its elements?
- Why is an entity ID safer than an `Entity*` across frames?
- What does the sanitizer message "heap-use-after-free" tell you about your design, not just your
  code?

---

## 4. `new`, `delete`, and why you will not use them

You should write these once, on Day 10, to understand them, and then essentially never again.

The problems: every `new` needs exactly one matching `delete` on every path including exceptions
and early returns; forget it and you leak; do it twice and you corrupt the heap; mismatch
`new[]`/`delete[]` and behaviour is undefined.

Manual memory management is not hard because the concept is difficult. It is hard because it
requires perfection across thousands of code paths.

---

## 5. C arrays and pointer decay

Worth understanding only because raylib's C API is full of pointer-plus-count parameters.

An array name converts ("decays") to a pointer to its first element, losing all size information.
That is why C APIs always take a count alongside the pointer, and why `sizeof` on a parameter
gives you the pointer size rather than the array size.

When calling raylib functions that want `Type*`, pass `vec.data()` and `vec.size()`.

---

## 6. RAII, the central idea

**Resource Acquisition Is Initialisation**: acquire a resource in a constructor, release it in a
destructor. Because destructors run automatically and deterministically at end of scope, the
resource cannot leak - even on an early return.

This is the pattern behind every good C++ resource type: `std::vector` (memory),
`std::ifstream` (file handles), `std::unique_ptr` (any pointer), `std::lock_guard` (locks).

In your game, every raylib pair is begging for RAII:

| Acquire | Release |
| --- | --- |
| `InitWindow` | `CloseWindow` |
| `InitAudioDevice` | `CloseAudioDevice` |
| `LoadTexture` | `UnloadTexture` |
| `LoadFont` | `UnloadFont` |
| `LoadRenderTexture` | `UnloadRenderTexture` |

`BeginDrawing`/`EndDrawing` and `BeginMode2D`/`EndMode2D` can also be wrapped, though many people
leave them explicit for readability. Try both and decide.

**Questions to answer**

- Why is RAII more reliable than remembering to call a cleanup function?
- In what order are destructors run for members of a class, and for locals in a scope?
- What happens if a destructor is called on an object that was moved from?

---

## 7. Copies, moves, and the rule of zero

Copying is silent and can be expensive. A `std::vector<Actor>` passed by value copies every
actor.

- **Copy** duplicates the data.
- **Move** transfers it, leaving the source empty but valid. `std::move` does not move anything
  itself - it just tells the compiler this object may be pillaged.
- The **rule of zero**: if your class owns no raw resource, do not write a destructor, copy
  constructor, or assignment operator. The compiler's versions are correct, and writing one
  incorrectly is worse than writing none.
- The **rule of five**: if you write one of them, you probably need to consider all five.
  Which is a strong argument for owning nothing raw and writing none.

Add print statements to a copy constructor once. Counting the copies in your own program is
genuinely surprising and permanently changes how you write function signatures.

---

## 8. `unique_ptr` - your default owner

`std::unique_ptr<T>` is a pointer that owns its object and deletes it when destroyed. Exactly
one `unique_ptr` can own an object, so ownership is unambiguous by construction. It costs nothing
compared to a raw pointer.

Create with `std::make_unique<Fireball>(args...)`. It cannot be copied, only moved - which is the
compiler enforcing your ownership design for you.

Use it when you need polymorphism (`std::vector<std::unique_ptr<Skill>>`) or when an object's
lifetime must be independent of a scope. Do **not** use it for plain data you could hold by
value - `std::vector<Entity>` is better than `std::vector<std::unique_ptr<Entity>>` unless you
need stable addresses or polymorphism.

To pass to a function that only reads: pass `const T&` or `T*` via `.get()`, never the
`unique_ptr` itself. Taking a `unique_ptr` parameter means "I am taking ownership from you", and
should be rare and deliberate.

---

## 9. `shared_ptr` and `weak_ptr` - rarely, and knowingly

`shared_ptr` is reference-counted shared ownership: the object dies when the last owner dies. It
is a legitimate tool and it is also frequently a sign that you have not decided who owns
something.

Costs: an atomic reference count on every copy, worse cache behaviour, and non-obvious lifetimes.
And cycles leak - two objects holding `shared_ptr`s to each other never reach zero. `weak_ptr`
breaks the cycle by observing without owning.

For this game you almost certainly need zero `shared_ptr`s. Before reaching for one, ask: "could
one clear owner hold this, with everyone else using an ID or a raw pointer?" The answer is
usually yes, and the design is better for it.

---

## 10. Your game's ownership graph

Write this down in [../design/architecture.md](../design/architecture.md) on Day 18 and keep it
current. A defensible shape for this project:

```
Game
├── owns Window, AudioDevice          (RAII)
├── owns AssetManager                 (owns all textures, fonts, sounds)
├── owns InputMap
├── owns GameSession                  (party, inventory, flags, quests - survives everything)
└── owns StateStack
    ├── owns FieldState
    │   ├── owns MapData
    │   ├── owns EntityWorld  (owns entities by value, referenced by ID)
    │   └── uses GameSession, AssetManager   (non-owning references)
    └── owns BattleState
        ├── owns Combatants  (which hold non-owning pointers into GameSession's party)
        └── uses GameSession, AssetManager
```

The load-bearing rule: **a battle must never own the party.** If it does, the party dies with the
battle, and you will discover this at the worst possible moment.

---

## Exercises

1. Write `HealByValue(int)` and `HealByReference(int&)`, call both, and explain the difference
   out loud.
2. Store a pointer to `vec[0]`, `push_back` twenty items, then use the pointer. Run under
   AddressSanitizer.
3. Write a `ScopedLog` RAII class and predict its output in nested scopes before running.
4. Count copies: instrument a copy constructor and push objects into a vector with and without
   `reserve`.
5. Build a `shared_ptr` cycle, prove the leak, then fix it with `weak_ptr`.
6. Draw your game's ownership graph from memory, then compare it with your architecture doc.

## Done criteria

- [ ] I can fill in the value/reference/pointer table without hesitating
- [ ] I know which vector operations invalidate pointers
- [ ] I wrap every raylib acquire/release pair in RAII
- [ ] I have written zero `new`/`delete` since Day 10
- [ ] Every object in my game has exactly one documented owner
- [ ] I can explain why the battle does not own the party

## Sources

- learncpp.com chapters 12, 13, 19, and 22 - the clearest free explanation of all of this
- [Game Programming Patterns: Object Pool](https://gameprogrammingpatterns.com/object-pool.html)
- cppreference on `unique_ptr`, `shared_ptr`, and `weak_ptr`
- Search "CppCon back to basics smart pointers" for a good talk on ownership design
