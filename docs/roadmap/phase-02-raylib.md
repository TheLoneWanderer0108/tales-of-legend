# Phase 02 - raylib fundamentals (Days 25-36)

Now the pixels arrive. raylib's job in this project is narrow and permanent: **window,
rendering, input, audio, fonts, and a 2D camera**. Every game system - states, entities,
collision, dialogue, battles, saves - you write yourself.

This phase is about becoming fluent with raylib's small API so that from Phase 3 onward it
is invisible plumbing and you can think about architecture instead.

Two habits to start now:

- Keep the [raylib cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html) open.
  Every function you need is on that page.
- When you want to know how something works, find it in
  [raylib examples](https://www.raylib.com/examples.html), read it, then write your own
  version from scratch. raylib 6.0 shipped over 70 new examples.

Research notes for this phase:
[game-loop-and-timing.md](../research/game-loop-and-timing.md),
[animation-and-tweening.md](../research/animation-and-tweening.md),
[camera-and-coordinates.md](../research/camera-and-coordinates.md),
[collision-detection.md](../research/collision-detection.md),
[audio.md](../research/audio.md).

---

### Day 25 - A window and a loop

**Goal:** see your first frame, and understand the three-line structure that every frame
of the next seven months follows.

**New concepts:** `InitWindow`, `WindowShouldClose`, `BeginDrawing`/`EndDrawing`,
`ClearBackground`, `SetTargetFPS`, `CloseWindow`, double buffering.

**Research first (20 min):** [game-loop-and-timing.md](../research/game-loop-and-timing.md)
sections 1-2.

**Tasks**

- [ ] Get a 1280x720 window titled "Tales of Legend" opening and closing cleanly
- [ ] Draw text, a rectangle, and a circle. Confirm that y increases *downwards*
- [ ] Remove `ClearBackground` and move a shape - see the smearing. That tells you what
      clearing does
- [ ] Draw something *after* `EndDrawing` and observe that nothing appears
- [ ] Print `GetFrameTime()` and `GetFPS()` on screen with `DrawText` +
      `TextFormat`
- [ ] Wrap the window in an RAII class (`struct Window { Window(); ~Window(); }`) exactly
      like your Day 13 `ScopedLog`, so `CloseWindow` can never be forgotten
- [ ] Comment your `main` with the four-line structure: init, loop, update, draw

**Deliverable:** a window you opened, with an RAII wrapper you wrote.

**Stuck?** If nothing displays under WSL2, check that `$DISPLAY` is set and try
`WAYLAND_DISPLAY= ./build/game` to force X11 through WSLg.

**Journal:** what does `BeginDrawing`/`EndDrawing` actually bracket?

---

### Day 26 - Delta time and frame-rate independence

**Goal:** movement that is identical at 60 FPS and 144 FPS.

**New concepts:** `GetFrameTime`, pixels per second vs pixels per frame, vector
normalisation, accumulated float error.

**Research first (25 min):** [game-loop-and-timing.md](../research/game-loop-and-timing.md)
sections 3-4.

**Tasks**

- [ ] Move a rectangle with arrow keys using `position += speed * dt` where speed is in
      pixels per second (try 200)
- [ ] Now move it with `position += 3` per frame instead. Change `SetTargetFPS` between
      30, 60 and 144 and watch the speed change. Understand why this is unacceptable
- [ ] Move diagonally and measure that it is faster (about 1.41x). Fix it by normalising
      the direction vector before scaling by speed
- [ ] Add acceleration and friction: `velocity += accel * dt`, `velocity *= 0.9f` per
      frame, then notice that a per-frame multiplier is *also* frame-rate dependent.
      Look up how to make damping time-based
- [ ] Clamp the rectangle inside the window
- [ ] Display `dt` in milliseconds and watch it spike when you drag the window

**Deliverable:** a rectangle whose movement speed is identical at any frame rate, including
diagonals.

**Stuck?** Normalising a zero-length vector produces NaN. Guard the length before dividing;
once NaN gets into a position, it never leaves.

**Journal:** the diagonal speed bug, and the NaN guard.

---

### Day 27 - Input properly

**Goal:** know the difference between "is held" and "was just pressed", because menus and
action commands depend on it.

**New concepts:** `IsKeyDown` vs `IsKeyPressed` vs `IsKeyReleased`, `GetKeyPressed`,
gamepad functions, `GetGamepadAxisMovement`, deadzones, mouse position.

**Tasks**

- [ ] Print live status for four keys using `IsKeyDown` and `IsKeyPressed` side by side.
      Hold a key and watch exactly one of them stay true
- [ ] Build a tiny menu of 4 options navigated with up/down (`IsKeyPressed`) and confirmed
      with Enter. Use `IsKeyDown` by mistake first, so you feel why it is wrong
- [ ] Add key repeat: after holding for 0.4s, repeat every 0.1s. Every real menu needs this
- [ ] If you have a controller, read the left stick with `GetGamepadAxisMovement`, apply a
      0.2 deadzone, and move the rectangle with it. Print raw vs deadzoned values
- [ ] Draw a circle at the mouse position and print when each mouse button is pressed
- [ ] Write down the input needs of a Paper Mario action command: exact press frame,
      hold duration, release frame, presses per second. Which raylib function serves each?

**Deliverable:** a menu with proper navigation and key repeat, driven by keyboard and
(if available) gamepad.

**Journal:** which function will your action commands need, and why?

---

### Day 28 - Shapes, colour, and a fake battle scene

**Goal:** get comfortable composing a scene out of primitives, so placeholder art is never
a blocker.

**New concepts:** `DrawRectangle`/`Lines`/`Rounded`, `DrawCircle`, `DrawLineEx`,
`DrawTriangle`, `Color` structs, `Fade`, alpha blending, draw order.

**Tasks**

- [ ] Build a mock battle screen with rectangles only: 3 heroes on the left, 2 enemies on
      the right, a ground line, an HP bar per actor, a command window at the bottom
- [ ] Make HP bars work: a dark background bar plus a coloured fill bar scaled by
      `hp / maxHp`, with the colour going green to yellow to red
- [ ] Define a small palette as named constants in one place. Using named colours from a
      palette instead of ad-hoc values is what stops placeholder art looking like noise
- [ ] Draw a translucent overlay with `Fade(BLACK, 0.5f)` and note how draw order decides
      what covers what
- [ ] Add a selection cursor (a triangle) that moves between targets with left/right keys
- [ ] Draw the same scene at half scale by multiplying every coordinate by 0.5, and feel
      how badly hardcoded coordinates scale. This is why Day 42 exists

**Deliverable:** a battle mock-up you would be happy to develop against for months.

**Journal:** how far can you get with rectangles alone? (Further than you expect.)

---

### Day 29 - Textures

**Goal:** put images on screen with full control over source region and placement.

**New concepts:** `Texture2D`, `LoadTexture`, `UnloadTexture`, `DrawTexture`,
`DrawTextureRec`, `DrawTexturePro`, source vs destination rectangles, origin, rotation,
horizontal flipping via a negative source width, `SetTextureFilter` and why
`TEXTURE_FILTER_POINT` matters for pixel art.

**Tasks**

- [ ] Make a placeholder sprite sheet yourself: open any image editor, draw a 4x4 grid of
      32x32 coloured squares with numbers, save as `assets/sprites/test_sheet.png`
- [ ] Load it and draw the whole thing. Then draw only cell (2,1) with `DrawTextureRec`
- [ ] Use `DrawTexturePro` to draw a cell scaled 3x, rotated 45 degrees around its centre
- [ ] Flip a sprite horizontally by negating the source rectangle width
- [ ] Scale a pixel-art sprite 4x with the default filter, then with
      `TEXTURE_FILTER_POINT`. Look closely at the blur difference
- [ ] Load a texture that does not exist and see what raylib does (it warns and gives you a
      blank texture). Decide how *your* asset loader will handle failure - loudly, not
      silently
- [ ] Wrap loading/unloading in RAII so a texture cannot leak

**Deliverable:** one texture, drawn six different ways from one sheet.

**Stuck?** Asset paths resolve from your working directory. Run from the project root, or
use `ChangeDirectory(GetApplicationDirectory())` at startup.

**Journal:** the difference between source rect and dest rect, in your own words.

---

### Day 30 - Sprite animation

**Goal:** a walk cycle, driven by a data structure you designed.

**New concepts:** frame timers, animation definitions, looping vs one-shot, animation state
driven by game state, facing direction.

**Research first (25 min):**
[animation-and-tweening.md](../research/animation-and-tweening.md) sections 1-3.

**Tasks**

- [ ] Design `struct Animation { int firstFrame; int frameCount; float frameDuration; bool loops; };`
      and `struct Animator { const Animation* current; int frame; float timer; };`
- [ ] Write `void UpdateAnimator(Animator&, float dt)` that advances frames and handles
      looping and one-shot completion (expose `bool Finished() const`)
- [ ] Animate a 4-frame walk cycle from your sheet
- [ ] Switch animation based on movement: `idle` when velocity is zero, `walk` otherwise,
      with four facing directions. Do **not** reset the frame timer when re-selecting the
      same animation, or the sprite will freeze on frame 0 - this is a classic bug, hit it
      and fix it
- [ ] Add a one-shot `attack` animation triggered by a key, that returns to `idle` when
      finished
- [ ] Print the current animation name and frame in your debug text

**Deliverable:** a sprite that idles, walks in four directions, and attacks once per press.

**Journal:** why does re-selecting an animation need a guard?

---

### Day 31 - Fonts and text

**Goal:** readable, measurable, positioned text - the raw material of an RPG.

**New concepts:** `LoadFont`, `LoadFontEx`, `DrawTextEx`, `MeasureTextEx`, spacing,
baseline vs top-left, `TextFormat`, glyph atlases, DPI scaling.

**Tasks**

- [ ] Find a free pixel font (or use the raylib default) and load it at the size you want.
      For pixel fonts, load at an exact multiple of the design size and use point filtering
- [ ] Draw text with `DrawTextEx` and control spacing
- [ ] Use `MeasureTextEx` to centre text horizontally, and to draw a box exactly sized
      around a string plus padding
- [ ] Port your Day 8 `WrapText` to use `MeasureTextEx` so it wraps by pixel width instead
      of character count - proportional fonts make character counting wrong
- [ ] Build a dialogue box: rounded rectangle, border, a wrapped 3-line message, a speaker
      name plate, and a blinking "continue" arrow
- [ ] Add the typewriter effect: reveal characters over time based on a
      characters-per-second value, and allow a key press to reveal the rest instantly

**Deliverable:** a dialogue box with wrapped, typewriter-revealed text. You will refine
this into the real system in Phase 4.

**Journal:** why must wrapping use measured pixel width?

---

### Day 32 - Audio

**Goal:** sound effects and music, correctly initialised and updated.

**New concepts:** `InitAudioDevice`, `Sound` vs `Music`, `PlaySound`,
`UpdateMusicStream`, volume and pitch, `SetSoundPitch`, sound aliases for overlapping
playback.

**Research first (20 min):** [audio.md](../research/audio.md) sections 1-3.

**Tasks**

- [ ] Initialise the audio device (and shut it down in RAII)
- [ ] Grab a handful of free CC0 sound effects (menu move, confirm, cancel, hit, heal)
- [ ] Play a sound on key press. Spam the key and listen to what happens when the same
      `Sound` retriggers. Then look up `LoadSoundAlias` and fix the cut-off
- [ ] Play a music track and call `UpdateMusicStream` every frame. Forget the call
      deliberately and hear the silence - this is the number one raylib audio mistake
- [ ] Randomise pitch by plus or minus 10% on a repeated hit sound. Notice how much less
      mechanical it feels. That is 3 lines of code for a large perceived quality gain
- [ ] Add master, music and SFX volume variables, and apply them when playing anything.
      Do this now, because retrofitting volume control across 100 call sites is miserable

**Deliverable:** sounds that overlap correctly, music that loops, and three volume sliders'
worth of plumbing.

**Journal:** how much better did pitch variation sound?

---

### Day 33 - Camera2D and coordinate spaces

**Goal:** stop thinking in screen coordinates and start thinking in world coordinates.

**New concepts:** `Camera2D` (target, offset, rotation, zoom), `BeginMode2D`/`EndMode2D`,
`GetWorldToScreen2D`, `GetScreenToWorld2D`, world space vs screen space, parallax.

**Research first (30 min):**
[camera-and-coordinates.md](../research/camera-and-coordinates.md) sections 1-4.

**Tasks**

- [ ] Draw a large scene (a 3000x2000 grid of markers) and move a `Camera2D` around it
      with the arrow keys
- [ ] Set `camera.offset` to the screen centre and `camera.target` to the player position.
      Confirm the player stays centred
- [ ] Draw a UI element **outside** `BeginMode2D`/`EndMode2D` and confirm it does not move
      with the camera. Write the rule: world inside, HUD outside
- [ ] Zoom with the mouse wheel. Then zoom *around the mouse cursor* rather than the screen
      centre, using `GetScreenToWorld2D`. This forces real understanding of the transform
- [ ] Convert the mouse position to world coordinates and highlight the marker under it
- [ ] Add a background layer drawn with a camera at 50% of the target for a parallax effect
- [ ] Add a screen shake: a decaying random offset added to `camera.target`

**Deliverable:** a camera you can move, zoom around the cursor, and shake, with a HUD that
stays put.

**Stuck?** If the HUD scrolls with the world, your draw call is inside `BeginMode2D`. If
zoom feels wrong, remember `offset` is in screen space and `target` is in world space.

**Journal:** the world/screen distinction, written so future-you cannot misread it.

---

### Day 34 - AABB collision

**Goal:** solid walls, done properly, with resolution rather than just detection.

**New concepts:** `Rectangle`, `CheckCollisionRecs`, `GetCollisionRec`, penetration depth,
axis separation, tunnelling, collider vs sprite bounds.

**Research first (30 min):**
[collision-detection.md](../research/collision-detection.md) sections 1-3.

**Tasks**

- [ ] Detect overlap between a moving rectangle and a static one, and tint on collision
- [ ] Resolve it: on overlap, push the mover out along the axis of least penetration
- [ ] Now do it the robust way instead: move on X, resolve X, then move on Y, resolve Y.
      Compare the two approaches when sliding along a wall
- [ ] Place 10 static walls, including a 1-tile-wide gap exactly the player's width. Make
      sure the player can pass through it without sticking
- [ ] Crank the speed to 3000 px/s and watch the player tunnel through a wall. Then fix it
      by splitting large movements into sub-steps
- [ ] Separate the collider from the sprite: a character's collider should be a small box
      at their feet, not their whole sprite. Draw both in debug view and see the difference
      in how movement feels

**Deliverable:** a player who slides along walls, fits through exact gaps, and cannot
tunnel at high speed.

**Journal:** why resolve axes separately?

---

### Day 35 - Fixed timestep and the debug overlay

**Goal:** deterministic updates, and permanent visibility into your own game.

**New concepts:** variable vs fixed timestep, the accumulator pattern, the death spiral and
how to cap it, interpolation (awareness), debug rendering, `assert`.

**Research first (30 min):** [game-loop-and-timing.md](../research/game-loop-and-timing.md)
sections 5-7, and [debugging-and-tooling.md](../research/debugging-and-tooling.md)
sections 4-5.

**Tasks**

- [ ] Implement the accumulator: accumulate `GetFrameTime()`, run `FixedUpdate(1/60.0f)`
      while the accumulator exceeds the step, subtract each time
- [ ] Cap the number of steps per frame (say 5) so a stall cannot cause a runaway
      catch-up loop. Explain the death spiral in your journal
- [ ] Force a stall with `WaitTime(0.5f)` on a key press and confirm your cap holds
- [ ] Decide what runs fixed (physics, timers, action-command windows) and what runs
      per-frame (animation, camera, particles). Write the split down
- [ ] Build a debug overlay toggled with F1: FPS, frame time, player position and velocity,
      current state name, entity count, colliders drawn as outlines
- [ ] Add a slow-motion toggle (F2) that scales the time step, and a step-one-frame key
      (F3). These two tools are how you will debug action command timing in Phase 5

**Deliverable:** fixed-timestep updates plus an F1 overlay you will use every day from
here on.

**Journal:** which systems did you put on the fixed step, and why?

---

### Day 36 - Phase consolidation

**Goal:** combine everything into one small demo, then clean it up.

**Tasks**

- [ ] Build a single program with: a room bounded by walls, an animated player sprite that
      walks and collides, a camera following with a deadzone, a footstep sound, background
      music, an HP bar HUD that does not scroll, and the F1 debug overlay
- [ ] Add three pickups that disappear with a sound and increment a counter
- [ ] Refactor into files: `main.cpp`, `player.h/.cpp`, `room.h/.cpp`, `debug.h/.cpp`.
      Notice that `main` is getting crowded - that pressure is the motivation for Phase 3
- [ ] Write a list of everything in this demo that you know is temporary or hacky. That
      list is your Phase 3 plan
- [ ] Update [../design/architecture.md](../design/architecture.md)
- [ ] Commit with a message you will be able to find later: `day 36: phase 2 complete`

**Deliverable:** a small but real game demo, plus a written list of its architectural sins.

---

## Phase 02 exit checklist

- [ ] I can open a window and structure a frame without looking anything up
- [ ] All my movement uses delta time and I can explain why
- [ ] I know when to use `IsKeyPressed` and when to use `IsKeyDown`
- [ ] I can draw any region of a sprite sheet at any scale, rotation, and flip
- [ ] I have an animation system I designed, not one I copied
- [ ] I can measure text and lay out a box around it
- [ ] Sounds overlap correctly and music updates every frame
- [ ] I can convert between world and screen coordinates in both directions
- [ ] My collision resolves per axis, slides along walls, and does not tunnel
- [ ] I have a fixed timestep and an F1 debug overlay

---

## Next: the interlude, not Phase 3

Before starting the engine, go to
[interlude-ship-something.md](interlude-ship-something.md) and spend six days building and
**releasing** a tiny complete game.

You now know enough raylib to finish something small, and you have not yet built any architecture -
which makes this the ideal moment. You will learn the whole shipping pipeline while the stakes are
trivial, get a stranger playing something of yours in month two rather than month seven, and feel
firsthand why Phase 3 exists.

It is not optional and it is not a detour.
