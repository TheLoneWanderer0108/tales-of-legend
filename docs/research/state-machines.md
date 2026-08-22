# State machines

**Used on:** Days 38-39 (state stack), Day 63 (cutscenes), Day 67 (enemy AI), Day 74 (battle
phases), Day 30 (animation states).

Why this matters: a state machine is the single most useful pattern in game programming, and this
project uses it at five different levels. Once you see it clearly, an enormous amount of game code
that would otherwise be a tangle of booleans becomes obvious and easy.

The symptom that you needed one and did not use it: a growing pile of flags like `isPaused`,
`isTalking`, `inBattle`, `isTransitioning`, and combinations that should be impossible but happen
anyway.

---

## 1. The core idea

A finite state machine has:

- a set of **states**, exactly one of which is current;
- **transitions** between them, each with a trigger;
- optional **entry** and **exit** actions per state.

The power comes from the constraint: because only one state is current, impossible combinations
cannot occur. You cannot be simultaneously talking and fighting if those are states rather than
booleans.

**Questions to answer**

- With five independent booleans, how many combinations exist, and how many are valid?
- Why does a state machine make invalid combinations unrepresentable rather than merely unlikely?
- What is the difference between a state and a mode flag?

---

## 2. The simplest implementation: an enum and a switch

```cpp
enum class PlayerState { Idle, Walking, Attacking, Hurt };
// in Update:
switch (state) {
    case PlayerState::Idle:      UpdateIdle(dt);      break;
    // ...
}
```

Do not skip this form because it looks primitive. For small machines (an animation state, an
enemy behaviour, a battle phase) it is the right answer: all the logic is visible in one place,
it is trivially debuggable, and it has no allocation.

Its cost: entry/exit actions must be handled manually, and it becomes unwieldy past roughly a
dozen states or when states need their own data.

Add a `ChangeState` helper rather than assigning the enum directly, so entry and exit actions
cannot be forgotten:

```cpp
void ChangeState(PlayerState next);   // runs OnExit(current), sets, runs OnEnter(next)
```

---

## 3. The state pattern: one class per state

When states have their own data and substantial behaviour, give each a class implementing a
common interface with `OnEnter`, `OnExit`, `Update`, and `Draw`.

This is what your `GameState` is. Benefits: each state's data is private to it, states are
independently testable, and adding a state touches no existing code.

Cost: indirection, allocation, and more files. Worth it for screens; overkill for animation
states.

**The decision rule for this project:** use classes for game screens (title, field, battle,
menus, cutscenes), and use an enum plus switch for everything smaller (animation, AI, battle
phases).

---

## 4. The stack of states

A single current state cannot express "the pause menu is open **over** the field". A stack can:

```
[ FieldState ]  ->  [ FieldState, PauseState ]  ->  [ FieldState, PauseState, ItemsMenu ]
```

Only the top state gets input. Whether the states below draw or update is controlled per state,
which is why the interface needs `DrawsBelow()` and `UpdatesBelow()`.

Operations: `Push` (open an overlay), `Pop` (close it), `Replace` (change screen), `Clear`
(return to title).

This handles nested menus, dialogue over the field, a battle over nothing, and a pause over a
battle - all with one mechanism. Getting this right on Day 38 pays off for the rest of the
project.

**Questions to answer**

- Which states in your game are pushed, and which replace?
- When the pause menu is open, should the field's animations still play? What about its timers?
- What should happen if the last state is popped?

---

## 5. The transition trap (read this before Day 39)

The bug everyone writes: a state calls `Pop()` on itself from inside its own `Update`. The stack
destroys the object, and then `Update` continues executing on freed memory. Sometimes it works;
sometimes it corrupts something three systems away.

The fix is **deferred mutation**: state changes enqueue a request, and the stack applies pending
requests after the update loop completes. The same pattern applies to destroying entities
(Day 48) and publishing events from handlers (Day 49). Learn the shape once and you will
recognise it everywhere.

Transitions with visuals are just another state: a `TransitionState` that fades out, performs the
swap, and fades in. Once you see that, screen transitions stop needing any special-casing.

---

## 6. Hierarchical states

States that share behaviour can nest: `Grounded` contains `Idle`, `Walk`, `Run`; `Airborne`
contains `Jump`, `Fall`. Shared logic lives in the parent, and a transition can target a parent
which then picks a child.

For this project this is mostly unnecessary complexity, but two places benefit:

- **Battle phases** where several phases share "waiting for an animation to finish".
- **Enemy AI** where `Combat` contains `Approach`, `Attack`, `Retreat`.

Know the concept; use it only when you notice real duplication.

---

## 7. Sequencing and pushdown automata

Cutscenes are not really state machines - they are **sequences**. The distinction matters:

- A state machine answers "what am I doing now, and what can I do next?"
- A sequence answers "what is the next step in this script?"

A queue of actions, each with `Start`/`Update`/`Finished`, models a cutscene far better than a
state machine with fifty states. Your Day 50 `Sequence` and Day 63 `Cutscene` are the same idea at
two scales, and your battle attack animations (Day 78) are a third.

If you have used another language's coroutines, this is what you are hand-rolling: a resumable
sequence of steps. C++20 does have coroutines, but they are a poor first-project topic - the
explicit action queue is easier to debug and to reason about.

---

## 8. Debugging state machines

- Log every transition with the from-state, to-state, and trigger. Most state bugs are visible
  immediately in that log.
- Draw the current state (and the whole stack) in the debug overlay.
- Assert on impossible transitions rather than silently ignoring them.
- Keep a written diagram. A state machine you cannot draw is a state machine you do not
  understand, and the drawing usually reveals the missing transition.

**The commonest state bugs**

| Symptom | Likely cause |
| --- | --- |
| Stuck in a state forever | A transition condition that can never become true |
| Actions running twice | Entry action run on re-entry to the same state |
| Input handled by two states | Both top and below receiving input |
| Crash on state change | Mutating the stack during iteration (section 5) |
| Animation frozen on frame 0 | Re-selecting the current state and resetting its timer |

---

## Exercises

1. Convert a set of three booleans in your code into an enum-based state machine. Count the
   invalid combinations you just eliminated.
2. Build the state stack and prove `DrawsBelow`/`UpdatesBelow` work by pausing over a field with
   an animated NPC.
3. Trigger the use-after-free from section 5 deliberately, read the sanitizer output, then fix it
   with deferred transitions.
4. Draw your battle phase diagram on paper, including every transition, before writing Day 74.
5. Implement a three-step sequence (move, wait, move back) with an action queue.

## Done criteria

- [ ] I can explain why a state machine beats a set of booleans
- [ ] I know when to use an enum and when to use classes
- [ ] My state changes are deferred and cannot crash
- [ ] I log transitions and can see the stack in the debug overlay
- [ ] I have a drawn diagram of my battle phase machine
- [ ] I understand the difference between a state machine and a sequence

## Sources

- [Game Programming Patterns: State](https://gameprogrammingpatterns.com/state.html) - the whole
  chapter, including the pushdown automata and hierarchical sections
- [Game Programming Patterns: Subclass Sandbox](https://gameprogrammingpatterns.com/subclass-sandbox.html)
  for how state classes share helpers
- Search "game screen stack management" for practical implementations to compare against yours
