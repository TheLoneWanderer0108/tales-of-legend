# UI systems

**Used on:** Day 55 (dialogue box), Day 75 (battle menu), Day 76 (targeting), Day 92 (battle UI),
Days 106-112 (menus, settings, rebinding).

Why this matters: an RPG *is* menus. The player will open the inventory hundreds of times and move a
cursor thousands of times. Ten minutes spent on cursor feel is worth more than a new enemy type,
because everyone experiences the cursor and only some players meet the enemy.

raylib gives you no UI. That is an advantage here: you will build exactly what you need, and it will
behave exactly how you want.

---

## 1. Immediate mode vs retained mode

**Immediate mode**: each frame you call functions that draw a widget and return its state.

```
if (Button("Attack", x, y)) { ... }
```

Pros: no state to manage, the code reads like the screen, easy to write.
Cons: layout and animation are awkward, and focus/navigation must be handled manually.

**Retained mode**: widgets are objects that persist, with state, position, and children.

Pros: animation and layout are natural, focus and navigation live in the objects, hierarchy is
explicit.
Cons: more machinery, more lifetime management.

**Recommendation:** a lightweight retained model for menus (you need cursor animation, scrolling state,
and focus), with immediate-style drawing for simple HUD elements. Do not attempt a full retained-mode
framework; your Day 106 target of about 600 lines is the right scale.

---

## 2. Gamepad-first navigation

This is the decision that shapes everything. A JRPG is played with a controller or the arrow keys, not
a mouse. That means:

- Every screen has a **focused element**, always exactly one.
- Movement is directional: up, down, left, right.
- Confirm and Cancel are universal, and **Cancel always goes back exactly one level**.
- There is no hover, no click, no drag.

Consequences worth internalising: you never need mouse hit-testing for menus, layout only needs to
support navigation order, and you must never have a state where nothing is focused (the player would be
stuck).

Support the mouse if you like, but design for the pad. Retro-fitting pad navigation onto a mouse-first
UI is much harder than the reverse.

---

## 3. Anatomy of a good menu

Break it into parts, because they recur everywhere:

- **The frame**: a nine-slice border so a panel can be any size from one small texture.
- **The title**.
- **The items**: label, icon, value, enabled state, description.
- **The cursor**: a visible indicator that eases into position.
- **The description area**: what the focused item does.
- **Scroll indicators**: arrows or a bar when there is more content.
- **Input hints**: "A: Confirm  B: Back", showing the player's *actual* bindings (Day 112).

Get the nine-slice right and every panel in your game comes free. It is one texture divided into nine
regions: four fixed corners, four stretched edges, and a stretched or tiled centre.

---

## 4. Feel: the details that matter

These are small and they are the entire difference between "functional" and "good":

| Detail | Why |
| --- | --- |
| Cursor eases to position (0.06-0.12s) | Instant jumps feel harsh; slow ones feel sluggish |
| Sound on move, confirm, cancel, and error | Silence reads as broken |
| Key repeat after 0.4s, every 0.1s | Holding a direction must work |
| Wrapping at list ends | Or clearly stop; never do nothing |
| Panel open/close animation (0.15-0.25s) | Screens should not teleport |
| Disabled items greyed **with a reason** | "Not enough FP" beats a silent no-op |
| Highlight the focused row, not just the cursor | Readability at a glance |
| Instant open (under 0.15s total) | Opened 300 times per playthrough |

The strongest rule: **never let a button press do nothing.** Either it acts, or it plays an error sound
and shows why. Silent rejection is the most common way homemade UI feels broken.

---

## 5. Nested menus and the focus stack

Your battle menu is Attack > Skills > (pick a skill) > (pick a target). That is a stack, and modelling
it as one gives you correct Cancel behaviour for free.

Push a level when descending, pop when cancelling. This is exactly the pattern from
[state-machines.md](state-machines.md) section 4, at a smaller scale - and if your menus are also game
states, you can literally reuse the state stack.

Requirements that fall out of it: each level remembers its own cursor position (returning to a menu
should restore where you were, not reset to the top), and only the top level receives input while lower
levels stay visible.

**Questions to answer**

- When the player cancels out of target selection, should the skill menu remember which skill was
  highlighted? (Yes.)
- Should a submenu dim the parent, or overlay it cleanly?
- What does Cancel do at the root of the battle menu? (Nothing, with a soft error sound - never close
  the battle.)

---

## 6. Lists and scrolling

Scrolling is where homemade UI most often breaks. The requirements:

- Keep the focused item visible, scrolling only when the cursor leaves the visible window.
- Show scroll indicators when there is content above or below.
- Support page up/down for long lists.
- Handle these cases explicitly, and test all of them: 0 items, 1 item, exactly one page, one more than
  a page, 200 items.
- Preserve the scroll position when returning to a list.

The empty case deserves its own attention: an empty inventory category should say "Nothing here yet",
not present a blank box that looks like a bug.

---

## 7. Target selection as a UI problem

Battle target selection (Day 76) is a menu whose items are positioned in the world, which makes it
worth thinking about separately:

- Cycle with left/right; consider up/down for rows.
- Highlight the whole affected set for group targets, and dim non-targets.
- Never allow an invalid target to be selected - filter the list rather than rejecting after the fact.
- Default the cursor intelligently: the last target chosen, or the lowest-HP ally for a heal.
- Handle every valid target dying while the menu is open.

Positioned-in-world UI generally: decide whether to draw it in world space (scales with the camera) or
convert with `GetWorldToScreen2D` and draw in screen space (stays crisp). For text, prefer screen
space. See [camera-and-coordinates.md](camera-and-coordinates.md) section 3.

---

## 8. Information design

What to show, and how much:

- **Answer the player's question.** In the equipment screen the question is "is this better?" - so show
  a comparison, not just numbers.
- **Show derived values, not just base.** ATK 7 (5 + 2) is more useful than ATK 7.
- **Progressive disclosure**: a summary by default, details on request (hold a key for a tooltip).
- **Consistency**: HP is always shown the same way, everywhere. Inconsistent presentation makes players
  distrust the numbers.
- **Never require memory.** If the player must remember a number from another screen to make a
  decision, show it on both.

---

## 9. Building the framework (Day 106)

The right order matters. Build several menus *first*, then extract the framework from what they share.
Building the framework first means designing for imagined requirements.

By Day 106 you will have the dialogue box, battle menu, shop, badge screen, and party screen. Their
shared parts are your specification. Likely: `Panel`, `ListView`, `Label`, `Bar`, `IconGrid`, `Tabs`,
plus focus and navigation handled once.

Then **refactor two existing menus onto it, not all of them.** If it does not make those two shorter
and clearer, the framework is wrong, and finding that out after two refactors is much cheaper than
after five.

Keep it small - under about 600 lines. A UI framework will happily grow into a second game if you let
it, and yours only needs to serve about ten screens.

---

## 10. Layout without a layout engine

You do not need constraint solving. You need three things:

- **Anchors**: position relative to a parent's corner or centre.
- **Vertical and horizontal stacks**: place items in sequence with a fixed spacing.
- **Content sizing**: a panel sized to its content plus padding, using `MeasureTextEx`.

That covers every screen in your game. Constants for padding and spacing in one place, used everywhere,
is what makes a UI look designed rather than assembled.

Because you are rendering at a fixed internal resolution (Day 42), you can use absolute coordinates
freely without worrying about window size - which removes most of the reason layout engines exist.

---

## 11. Accessibility in UI

Cheap to do now, expensive to retrofit (Day 161):

- **No information by colour alone.** A red status icon and a green one must differ in shape too.
- **Contrast**: light text on dark panels, and check it squinting at your internal resolution.
- **Minimum text size**: nothing critical under 8px at internal resolution, and offer a large-font
  option.
- **Never require speed** to read or dismiss UI.
- **Show actual bindings** in prompts, not hardcoded glyphs.
- Test every screen with keyboard only, then with gamepad only.

---

## 12. Debugging UI

- Draw every panel's bounds on a toggle. Most layout bugs are instantly obvious that way.
- Print the focus path (`BattleMenu > Skills > item 3`) in the debug overlay.
- Test every menu with extreme content: the longest possible item name, 200 items, an empty list, and
  the largest numbers your game can produce.
- Navigate every menu using only the keyboard, then only the pad, and note anything unreachable.
- Count your own presses: if a common action takes four presses, find a way to make it two.

---

## Exercises

1. Build a nine-slice panel renderer and draw panels at five different sizes from one texture.
2. Build a list with cursor easing, key repeat, wrapping, and sounds. Use it for five minutes and tune
   the numbers.
3. Implement scrolling and test all five cases from section 6.
4. Implement the focus stack for a three-level menu and verify Cancel behaviour and cursor memory.
5. Add a disabled item with a reason and confirm no press is ever silently ignored.
6. Time how long your pause menu takes to open, and make it faster.

## Done criteria

- [ ] Every screen is fully navigable with a gamepad and with the keyboard alone
- [ ] Exactly one element is focused at all times
- [ ] No button press is ever silently ignored
- [ ] Cursor easing, sounds, and key repeat are tuned to the numbers in section 4
- [ ] Nested menus use a focus stack and remember their cursor positions
- [ ] Lists handle 0, 1, one page, and 200 items correctly
- [ ] The framework is under 600 lines and made two real menus shorter
- [ ] No information is conveyed by colour alone

## Sources

- [Game UI Database](https://www.gameuidatabase.com/) - browse JRPG menus and steal shamelessly
- Search "juicing your menus game feel" for the animation details
- [Game Accessibility Guidelines](https://gameaccessibilityguidelines.com/) - the UI sections
- Study Final Fantasy X's turn order display and Paper Mario's battle menu specifically; both are
  masterclasses in legibility
- raylib's `raygui` - do not use it for your game, but read its source for ideas about immediate-mode
  drawing
