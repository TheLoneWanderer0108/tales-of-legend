# Cameras and coordinate spaces

**Used on:** Day 33 (Camera2D), Day 42 (virtual resolution), Day 51 (camera follow), Day 78
(battle camera).

Why this matters: coordinate space confusion produces bugs that are genuinely hard to reason about
- the mouse clicking the wrong thing, UI scrolling with the world, a camera that jitters. All of it
comes from being unclear about which space a number is in. Once you name your spaces explicitly,
the whole topic becomes easy.

---

## 1. The spaces

Four spaces exist in your game. Know which one every coordinate lives in, and say so in variable
names when it is ambiguous.

| Space | Units | Example |
| --- | --- | --- |
| **Tile** | tiles | tile (12, 7) in the map grid |
| **World** | pixels, map origin | the player at (192.5, 112.0) |
| **Screen (virtual)** | pixels, your internal resolution | the HP bar at (8, 8) in a 480x270 view |
| **Window** | pixels, actual window | the mouse at (1420, 733) in a 1920x1080 window |

Conversions you need:

```
tile   -> world:   world = tile * tileSize
world  -> tile:    tile  = floor(world / tileSize)
world  -> screen:  GetWorldToScreen2D(world, camera)
screen -> world:   GetScreenToWorld2D(screen, camera)
window -> screen:  (window - letterboxOffset) / scale        [your own function, Day 42]
```

**Questions to answer**

- Which space is a UI element's position in? Which space is an entity's?
- If your camera is at (500, 300) and the player is at (500, 300), where does the player appear on
  screen?
- Why does the mouse need two conversions to become a world position?

---

## 2. raylib's Camera2D

Four fields, and understanding them is most of this topic:

```cpp
Camera2D camera{};
camera.target   = playerPosition;              // WORLD point the camera looks at
camera.offset   = { screenW / 2, screenH / 2 };// SCREEN point that target maps to
camera.rotation = 0.0f;
camera.zoom     = 1.0f;
```

The key insight: `target` is in **world** space, `offset` is in **screen** space. The camera makes
the world point `target` appear at the screen point `offset`. Setting `offset` to the screen centre
therefore centres the target.

A common mistake is setting `offset` to the player's position (or `target` to a screen coordinate),
which produces confusing drift. If your camera behaves strangely, check which space each field is
receiving.

Everything drawn between `BeginMode2D(camera)` and `EndMode2D()` is transformed. Everything outside
is not.

---

## 3. World inside, HUD outside

The rule that prevents a whole class of bug:

```cpp
BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);
        DrawMap(); DrawEntities(); DrawEffects();     // world space
    EndMode2D();
    DrawHUD(); DrawMenus(); DrawDialogue();           // screen space
EndDrawing();
```

If your HP bar scrolls away when you walk, it is inside `BeginMode2D`. If a world object refuses to
move with the camera, it is outside.

The interesting case is UI *attached* to a world object: a floating damage number, an interaction
prompt, an enemy HP bar. Two valid approaches:

1. Draw it in world space inside `BeginMode2D` (simple, and it scales with zoom).
2. Convert with `GetWorldToScreen2D` and draw it in screen space (stays crisp and un-zoomed, which
   is usually better for text).

Pick per element, and know why.

---

## 4. Zoom, and zooming around a point

`camera.zoom` scales the world around `target`. Zooming toward the screen centre is the default and
is what you usually want for a battle zoom.

Zooming toward the **mouse cursor** is a good exercise (Day 33) because it forces you to actually
understand the transform: record the world position under the cursor before the zoom change, then
adjust `target` so that the same world position remains under the cursor afterwards. If you can do
that, you understand the camera.

For pixel art, prefer integer zoom values. Fractional zoom makes pixels non-square and produces
shimmering. Your Day 42 virtual resolution approach mostly removes the need for camera zoom in the
first place - which is one of its advantages.

---

## 5. Following the player

A camera locked exactly to the player is technically correct and feels bad: every small movement
shifts the entire screen, which is visually noisy and can be nauseating.

Techniques, roughly in the order you should add them:

- **Smoothing.** Move the camera a fraction of the way toward the target each step. Naive
  `lerp(cam, target, 0.1f)` per frame is frame-rate dependent; use a time-based exponential form
  (`1 - exp(-rate * dt)`) so it behaves the same at 30 and 144 FPS.
- **Deadzone.** The camera does not move until the player leaves a central box. This is what makes
  small movements feel calm. Draw the deadzone in debug view while tuning.
- **Clamping.** Never show outside the map. Clamp `target` to
  `[halfViewW, mapWidth - halfViewW]`, and handle maps smaller than the view by centring instead of
  clamping (otherwise your clamp inverts and the camera jumps).
- **Look-ahead.** Offset the target slightly in the direction of movement, so the player sees more
  of where they are going. Subtle, and easy to overdo.
- **Focus overrides.** During a cutscene or a battle, the camera targets something else. Design for
  this from the start (a target *provider* rather than a hardcoded player pointer).

**Questions to answer**

- Why is per-frame `lerp` frame-rate dependent?
- What happens to clamping when the map is narrower than the screen?
- Should the camera move faster when the player is running?

---

## 6. Pixel snapping

This is the detail that separates a crisp pixel-art game from a shimmering one.

If your camera target is at a fractional world position, tiles land on fractional pixels and get
sampled inconsistently, so straight lines crawl and text wobbles. The fix is to round the camera
target (and often entity draw positions) to whole pixels before rendering.

Keep the *logical* position as a float - only the *drawn* position is snapped. Toggle it on and off
while moving slowly and look closely at the tile edges; the difference is dramatic once you see it.

Note the tension with interpolation (see
[game-loop-and-timing.md](game-loop-and-timing.md) section 7): you cannot both snap and smoothly
interpolate sub-pixel motion. For a pixel-art game, snapping wins.

---

## 7. Screen shake and camera effects

Shake is the highest-impact camera effect and takes ten lines: add a random offset to the camera
target each frame, with the magnitude decaying over time.

Details that make it feel good rather than cheap:

- Decay the magnitude (exponentially or linearly) rather than stopping abruptly.
- Scale the magnitude to the event's importance - a normal hit barely shakes, a boss slam shakes
  hard.
- Cap the total magnitude so it never becomes unreadable.
- Prefer a smooth noise function over pure random for a less jittery feel.
- **Provide an option to disable it.** Some players get motion sick, and this is an accessibility
  requirement (Day 161).

Other effects worth having: a scripted `PanTo(target, duration, easing)` for cutscenes (Day 63), a
zoom punch on impacts (Day 157), and a slight idle drift during dialogue to keep static scenes from
feeling dead.

Compose them in a defined order: follow computes a base target, effects add offsets, then snapping
happens last.

---

## 8. Virtual resolution (Day 42)

Rather than designing UI for whatever window size you get, render the whole game to a
`RenderTexture2D` at a fixed internal resolution (480x270 or 640x360), then scale that texture to
the window.

Benefits: you design once, pixel art stays pixel-perfect at integer scales, and every UI coordinate
is a small stable number.

Details that catch people:

- **Render textures are stored upside down** in raylib. Draw with a negative source height, or your
  game appears flipped.
- Use `TEXTURE_FILTER_POINT` and integer scale factors. Fractional scaling makes uneven pixels.
- Letterbox: centre the scaled texture and fill the remainder with black.
- **The mouse needs converting** through the scale and letterbox offset. Verify by drawing a cursor
  that lines up exactly at every window size, including fullscreen.
- raylib 6.0 redesigned fullscreen modes and high-DPI content scaling, so read the current docs
  rather than an older tutorial.

---

## Exercises

1. Draw a large grid and move a camera over it. Print the camera target, and the world and screen
   positions of one marker, simultaneously.
2. Zoom around the mouse cursor. Do not move on until it is exact.
3. Draw one element inside `BeginMode2D` and the same element outside, and watch the difference as
   you move.
4. Convert the mouse to a world position and highlight the tile under it. Verify at several zoom
   levels and window sizes.
5. Implement follow with smoothing, then add a deadzone, then clamping. Test each addition
   separately.
6. Toggle pixel snapping on and off while walking slowly past a wall of vertical lines.

## Done criteria

- [ ] I can name the space of every coordinate in my code
- [ ] I can convert between all four spaces in both directions
- [ ] HUD never scrolls and world objects never stick
- [ ] Camera follow has smoothing, a deadzone, and clamping that handles small maps
- [ ] Camera smoothing is frame-rate independent
- [ ] Positions are pixel-snapped for rendering
- [ ] The mouse position is correct at every window size and in fullscreen

## Sources

- raylib `core_2d_camera` and `core_2d_camera_platformer` examples - read both
- [Scroll Back: The Theory and Practice of Cameras in Side-Scrollers](https://www.gamedeveloper.com/design/scroll-back-the-theory-and-practice-of-cameras-in-side-scrollers) -
  written for platformers, and the deadzone and look-ahead reasoning transfers directly
- Search "pixel perfect camera 2d game" for the snapping discussion
- raylib `core_custom_frame_control` for the render texture and scaling pattern
