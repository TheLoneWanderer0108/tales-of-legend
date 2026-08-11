# Entity architecture

**Used on:** Days 14-15 (inheritance vs composition), Day 48 (the entity model decision),
Day 90 (enemy mechanics).

Why this matters: how you represent "things in the world" shapes every system that touches them.
It is also the topic where beginners most often follow bad advice, either building a deep
inheritance tree that collapses under its own weight, or building a full ECS for a game with 40
entities on screen.

The goal of this note is to let you make the decision on Day 48 deliberately, and write down why.

---

## 1. What an entity needs to be

In your game, an entity is: the player, an NPC, a field enemy, a chest, a door, a switch, a
pushable block, a trigger volume, a projectile, a particle emitter.

Look at that list and notice how little they share: a position, maybe a collider, maybe a sprite,
maybe an update behaviour. A trigger has no sprite. A chest never moves. That observation is the
whole argument against a deep hierarchy.

---

## 2. Approach 1: an inheritance hierarchy

```
Entity -> Character -> Player
                    -> Npc
                    -> Enemy -> FlyingEnemy
Entity -> Interactable -> Chest
                       -> Door
```

**Why it appeals:** it matches how you talk about the game, tutorials teach it, and virtual
functions make `entity->Update()` work polymorphically.

**Why it fails, specifically:** requirements do not form a tree. The moment you need "a flying
enemy that is also a chest" or "an NPC that sometimes fights", you must either duplicate code,
push behaviour up into a base class that does not need it, or build a diamond. Deep hierarchies
also make behaviour hard to locate - understanding `FlyingFireEnemy` means reading four files.

The classic failure is a base class that grows to hold every field any subclass might need, so
every entity carries dozens of unused members and the hierarchy provides no real benefit.

**Questions to answer**

- Design `Enemy -> FlyingEnemy -> FireFlyingEnemy`, then add "a ground fire enemy that sometimes
  flies". What are your options and what does each cost?
- What is object slicing, and what happens if you store derived objects in a `std::vector<Base>`?
- Why does a virtual destructor matter when deleting through a base pointer?

---

## 3. Where inheritance *is* right

Inheritance is excellent for **interfaces with behaviour that varies**, where the set of
implementations is open and each is genuinely a different algorithm:

- `GameState` - each screen behaves entirely differently.
- `ActionCommand` - each command type reads input differently.
- `Skill` (arguably) - though data often serves better; see section 6.
- `Cutscene::Action` - each action does its own thing.

The pattern: **one interface, several implementations, no data shared between them.** That is a
healthy hierarchy, and it is one level deep.

---

## 4. Approach 2: composition

Instead of "an entity is a kind of X", say "an entity has an X":

```cpp
struct Entity {
    EntityKind kind;
    Vector2    position;
    Collider   collider;      // optional
    Animator   animator;      // optional
    AiProfile  ai;            // optional
    // ...
};
```

Variation comes from which parts are present and what values they hold, not from which class the
object is. "A flying fire enemy that sometimes lands" becomes data: `canFly = true`,
`element = Fire`, `ai = "hybrid"`.

**Why this wins for a JRPG:** almost all your variety is *data* variety, not behaviour variety.
Forty enemies differ in stats, sprites, skills, and AI profile - not in code. Composition lets you
add the thirty-ninth enemy by editing a JSON file, which is exactly what Phase 7 depends on.

**Questions to answer**

- Which of your entity's parts are genuinely optional?
- What is the cost of every entity carrying an unused `Animator`? Is it acceptable at your scale?
  (For 100 entities, yes, comfortably.)
- How would you add "a chest that walks away when you approach" under each approach?

---

## 5. Approach 3: a full ECS

Entity Component System: entities are just IDs, components are plain data in tightly packed
arrays, and systems iterate over all entities that have a given set of components.

**Genuine advantages:** excellent cache locality, trivially data-driven, systems fully decoupled,
scales to tens of thousands of entities.

**Genuine costs for your project:** significant up-front machinery, indirection when debugging,
awkward for "do this specific scripted thing to this one NPC", and it solves a performance problem
you do not have. A JRPG has perhaps 50 entities on screen.

**The honest recommendation: do not build an ECS for this game.** Not because ECS is bad - it is
excellent - but because building one is a *different project* from building your JRPG, and you
would learn less about game architecture and more about container plumbing.

Do read about it. Understanding data-oriented design will change how you think about layout even
in a non-ECS codebase. Just do not make it your first architecture.

---

## 6. Data over code

The deepest version of this lesson: much of what looks like it needs polymorphism is actually a
table.

Consider a `Skill`. As a class hierarchy, forty skills means forty subclasses. As data:

```json
{ "id": "fireball", "power": 12, "element": "fire", "fpCost": 3,
  "target": "one_enemy", "command": "timed_press", "effects": [ ... ] }
```

Forty skills is forty rows, addable without a rebuild. Only genuinely unique behaviour (a skill
whose effect is "swap the HP of two combatants") needs code, and that can be a named effect type
in a table of effect handlers.

The general test: **if two subclasses differ only in numbers or in which existing operations they
compose, they should be data, not subclasses.**

This is the same reasoning behind your effect system (Day 96), your AI profiles (Day 79), and
your command table (Day 60). Notice how often the answer is "a table plus a handler map".

---

## 7. Identity: IDs, not pointers or indices

Whatever model you choose, entity **identity** needs deciding.

- **Raw pointers** break when the container reallocates or the entity is destroyed. Never store
  them across frames.
- **Indices** break when entities are removed and the container shifts, silently pointing at a
  different entity - the worst failure mode, because nothing crashes.
- **IDs** (a monotonically increasing integer, never reused) are safe: a lookup either finds the
  entity or tells you it is gone.

Store IDs anywhere a reference must outlive the current function: turn order, target selection,
quest state, save files, and event payloads.

For lookup, an `unordered_map<int, size_t>` from ID to index is plenty at your scale. If you want
to learn something more advanced, read about **generational handles** (an index plus a generation
counter, giving O(1) lookup with safety) - a genuinely useful technique and much simpler than an
ECS.

---

## 8. Lifetime: spawn and destroy queues

Never add or remove entities during iteration. It invalidates iterators, shifts indices, and
produces the same class of use-after-free you met in the state stack.

Instead: mark for destruction during update, and process a destroy queue afterwards. Queue spawns
too, so an entity spawning another does not reallocate the container mid-loop.

This also gives you a clean place to publish `EntityDestroyed` events and to handle death
animations - a "dying" entity can remain for its animation, excluded from gameplay queries.

---

## 9. Update order and determinism

Entities updated in different orders produce different results, and inconsistent ordering causes
bugs that appear random.

- Update in a defined order (ID order, or spawn order) and keep it stable.
- Beware entities reacting to each other's changes mid-frame: two entities pushing each other can
  produce different results depending on who moved first. If it matters, compute all intentions
  first, then apply them.
- Draw order is separate from update order. For a top-down game, sort by y position for drawing
  (Day 48) while updating in ID order.

---

## Recommended decision for this project

Write this in your architecture doc on Day 48, in your own words and with your own reasoning:

> **One `Entity` struct with a `Kind` enum and optional parts, stored in a vector, referenced by
> stable IDs, with spawn and destroy queues. Inheritance reserved for interfaces with genuinely
> varying behaviour: `GameState`, `ActionCommand`, and cutscene actions. All content variety
> expressed as data in JSON.**

This is boring, which is the point. It will comfortably carry a three-chapter JRPG, it stays
debuggable, and it makes Phase 7's content production fast.

---

## Exercises

1. Take the flying/fire/boss enemy problem and solve it three ways (inheritance, composition,
   data). Write down the cost of adding the twentieth enemy under each.
2. Demonstrate object slicing and explain the output.
3. Delete a derived object through a base pointer with a non-virtual destructor, under sanitizers.
4. Prove the index-invalidation bug: hold an index, destroy an earlier entity, use the index.
5. Convert one small class hierarchy in your code into data plus a handler table, and compare the
   line counts.

## Done criteria

- [ ] I can argue for and against inheritance with an example from my own code
- [ ] My entity model is a written decision with reasons, not an accident
- [ ] I store IDs, never indices or pointers, across frames
- [ ] Spawning and destroying happen through queues
- [ ] My content variety lives in data files, not in subclasses

## Sources

- [Game Programming Patterns: Component](https://gameprogrammingpatterns.com/component.html) -
  the most important chapter for this note
- [Game Programming Patterns: Type Object](https://gameprogrammingpatterns.com/type-object.html) -
  precisely the "data instead of subclasses" pattern
- [Game Programming Patterns: Bytecode](https://gameprogrammingpatterns.com/bytecode.html) - read
  it for perspective on data-driven behaviour, do not implement it
- Search "ECS vs inheritance game architecture" and read a few opposing opinions rather than one
