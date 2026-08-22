# Phase 03 - Engine architecture (Days 37-53)

**Before this phase:** you should have completed
[interlude-ship-something.md](interlude-ship-something.md) and released a tiny game. Reread the notes
you wrote on where the one-file version hurt - the systems in this phase exist to solve exactly those
problems, and you will build them better for having felt the pain.

This is the phase that decides whether the next 117 days are pleasant or painful. You are
building the load-bearing structure: a state machine, an asset manager, an input layer,
tilemaps, collision, entities, events, and a camera - all yours.

The temptation here is to skip ahead to dialogue and battles. Resist it. Every system in
Phases 4-8 sits on top of what you build in these 17 days.

Two principles to hold onto:

- **One clear owner for every piece of data.** Write the owner down in
  [../design/architecture.md](../design/architecture.md) as you go.
- **Build the simplest thing that could possibly work, then use it before generalising it.**
  You cannot design a good abstraction for a use case you have never had.

Research notes for this phase:
[state-machines.md](../research/state-machines.md),
[entity-architecture.md](../research/entity-architecture.md),
[tilemaps.md](../research/tilemaps.md),
[collision-detection.md](../research/collision-detection.md),
[camera-and-coordinates.md](../research/camera-and-coordinates.md),
[data-driven-design.md](../research/data-driven-design.md),
[ui-systems.md](../research/ui-systems.md).

---

### Day 37 - The `Game` class

**Goal:** `main` becomes five lines and never grows again.

**New concepts:** application ownership, initialisation order, update/draw separation,
RAII for subsystems.

**Tasks**

- [ ] Write `class Game` with `Game()`, `~Game()`, `void Run()`, and private
      `void Update(float dt)` and `void Draw()`
- [ ] `main` becomes: construct `Game`, call `Run()`, return
- [ ] Own the window and audio device via RAII members, constructed in the right order and
      destroyed in the reverse
- [ ] Move the fixed-timestep accumulator inside `Run()`
- [ ] Enforce the rule mechanically: `Draw()` takes no `dt` and every member it touches is
      accessed through a `const` method where possible. Try to mutate state in `Draw` and
      let the compiler stop you
- [ ] Add `bool running_` and a `Quit()` method so anything can request shutdown without
      calling `exit()`

**Deliverable:** a five-line `main`, and a `Game` class that owns the application.

**Journal:** why is separating update from draw worth enforcing?

---

### Day 38 - The state stack

**Goal:** the single most useful pattern in game programming - screens that push on top of
each other.

**New concepts:** finite state machines, the state pattern, a stack of states vs a single
current state, transparent states, update/draw pass-through.

**Research first (35 min):** [state-machines.md](../research/state-machines.md)
sections 1-5.

**Tasks**

- [ ] Define the interface:

```cpp
class GameState {
public:
    virtual ~GameState() = default;
    virtual void OnEnter() {}
    virtual void OnExit() {}
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
    virtual bool DrawsBelow() const { return false; }   // is the state under me visible?
    virtual bool UpdatesBelow() const { return false; } // does the state under me tick?
};
```

- [ ] Implement `class StateStack` holding `std::vector<std::unique_ptr<GameState>>` with
      `Push`, `Pop`, `Replace`, `Clear`
- [ ] Implement three states: `TitleState` (press Enter), `FieldState` (your Day 36 demo),
      `PauseState` (a translucent overlay, returns `true` from `DrawsBelow`)
- [ ] Prove that pausing draws the field underneath but does not update it
- [ ] Prove `OnEnter`/`OnExit` fire at exactly the right times with log prints
- [ ] Write down in the architecture doc which of your states are "screens" (replace) and
      which are "overlays" (push)

**Deliverable:** Title to Field to Pause and back, with a visible field behind the pause
overlay.

**Journal:** why a stack rather than a single current state?

---

### Day 39 - Safe transitions

**Goal:** fix the crash you are about to have, before you have it a dozen times.

**New concepts:** deferred mutation, iterator/reference invalidation, command queues,
transition effects.

**Tasks**

- [ ] Trigger the bug on purpose: call `Pop()` from inside a state's own `Update`, then
      keep using `this` afterwards. Run with sanitizers and read the use-after-free
- [ ] Fix it: state changes enqueue a request (`enum class Action { Push, Pop, Replace }`
      plus a payload) and the stack applies them **after** the update loop finishes
- [ ] Add a fade transition: a `TransitionState` that fades out, swaps, fades in. Because it
      is just another state, this needs no special-casing
- [ ] Add a debug key that prints the current stack from top to bottom. You will use this
      constantly once battles start pushing menus on top of battles
- [ ] Handle the empty-stack case: popping the last state should quit cleanly, not crash

**Deliverable:** state changes from inside states are safe, and transitions fade.

**Stuck?** If the sanitizer report is confusing, the short version: the object destroyed
itself while it was still executing its own method. Deferring the destruction fixes it.

**Journal:** the use-after-free, described so you recognise it next time.

---

### Day 40 - The asset manager

**Goal:** load each asset exactly once, hand out non-owning references, never leak.

**New concepts:** caching, string keys vs handles, non-owning access, lifetime spanning
states, fail-loudly loading.

**Tasks**

- [ ] Write `class AssetManager` with `const Texture2D& Texture(const std::string& id)`,
      plus equivalents for fonts and sounds
- [ ] Cache in an `unordered_map`, loading on first request. Log every actual load so you
      can see the cache working (and see accidental duplicate loads)
- [ ] Return `const&`, not copies. Returning a `Texture2D` by value and unloading it
      elsewhere is the classic double-free in raylib code
- [ ] Handle missing files: log an error and return a generated 1x1 magenta placeholder
      texture, so a missing asset is glaringly visible instead of invisible
- [ ] Make `Game` own the manager and pass it to states by reference. Do **not** make it a
      global singleton - you will thank yourself when you want two of something in a test
- [ ] Add a manifest file (`assets/manifest.txt`) listing assets to preload at boot, so
      you can preload rather than hitch on first use
- [ ] Add an `UnloadAll` called from the destructor and verify with sanitizers there are no
      leaks at exit

**Deliverable:** every asset loaded once, missing assets obvious, zero leaks.

**Journal:** why non-owning references, and who is the owner?

---

### Day 41 - The input layer

**Goal:** game code asks "is Confirm pressed?", never "is Enter pressed?".

**New concepts:** action mapping, indirection layers, rebinding, input buffering, consuming
input.

**Tasks**

- [ ] Define `enum class Action { Up, Down, Left, Right, Confirm, Cancel, Menu, Guard, Special }`
- [ ] Write `class InputMap` with `bool Down(Action)`, `bool Pressed(Action)`,
      `bool Released(Action)`, `float AxisX()`, `float AxisY()`
- [ ] Back each action with a list of bindings: keyboard keys **and** gamepad buttons, so
      both work simultaneously with no game-code changes
- [ ] Move key repeat logic (Day 27) into the input layer, exposed as
      `bool PressedRepeat(Action)`
- [ ] Add `float TimeHeld(Action)` and `bool PressedWithin(Action, float seconds)` - an
      input buffer. Your action commands and menus both need forgiveness windows
- [ ] Enforce the rule from your architecture doc: grep your codebase for `IsKey` and make
      sure the only hits are inside `InputMap` (debug keys excepted)
- [ ] Load bindings from a config file so rebinding is already possible before you build
      the settings menu in Phase 6

**Deliverable:** an input layer that supports keyboard and gamepad, repeat, hold time, and
buffering, configured from a file.

**Journal:** what does this indirection buy you that direct key checks do not?

---

### Day 42 - Virtual resolution and scaling

**Goal:** design once at a fixed pixel resolution, display crisply at any window size.

**New concepts:** `RenderTexture2D`, `BeginTextureMode`, letterboxing, integer scaling,
mapping mouse coordinates back through the scale.

**Tasks**

- [ ] Pick your internal resolution and write it in the design doc. 480x270 or 640x360 both
      scale cleanly to 1080p (4x and 3x). Everything you draw from now on uses these
      coordinates
- [ ] Render the whole game to a `RenderTexture2D` at that size
- [ ] Draw the render texture to the window scaled to fit, centred, preserving aspect ratio,
      with black bars where needed
- [ ] Use `TEXTURE_FILTER_POINT` and integer scale factors so pixels stay square. Compare
      with a fractional scale and see the uneven pixels
- [ ] Handle window resize (`SetWindowState(FLAG_WINDOW_RESIZABLE)`) and fullscreen
      toggling. raylib 6.0 redesigned fullscreen modes and DPI scaling, so read the current
      docs rather than an old tutorial
- [ ] Fix the mouse: convert window coordinates into virtual-resolution coordinates,
      accounting for scale and letterbox offset. Verify by drawing a cursor that lines up
      exactly at every window size
- [ ] Retrofit your Day 36 demo to the new resolution

**Deliverable:** the game renders at a fixed internal resolution, scales crisply, and the
mouse position is correct in every window mode.

**Stuck?** Draw the render texture with a negative source height - raylib render textures
are stored upside down, and this is the classic "why is my game flipped" question.

**Journal:** your chosen resolution and the reason.

---

### Day 43 - Tiled and your first real map

**Goal:** author maps in a real editor instead of hardcoding them.

**New concepts:** Tiled, tilesets, tile layers, object layers, custom properties, tile GIDs,
JSON export.

**Research first (30 min):** [tilemaps.md](../research/tilemaps.md) sections 1-3.

**Tasks**

- [ ] Install [Tiled](https://www.mapeditor.org/) and skim its manual for 15 minutes
- [ ] Make a placeholder tileset: a single PNG with 16x16 tiles - grass, dirt, wall, water,
      floor. Numbered and ugly is correct at this stage
- [ ] Author a 40x30 map with three tile layers: `ground`, `decor`, `overhead` (drawn over
      the player so they can walk behind things)
- [ ] Add an object layer `entities` with rectangles for the player spawn, two NPC spawns,
      and a chest. Give each a `type` and custom properties (e.g. `npcId`)
- [ ] Mark solid tiles. Read [tilemaps.md](../research/tilemaps.md) section 3 and pick your
      approach: a custom `solid` boolean property on tileset tiles, or a dedicated
      collision layer. Write your choice and reason in the architecture doc
- [ ] Export as JSON (not TMX) to `assets/maps/town.json` and open the JSON in a text
      editor. Find the `data` array, the layer list, and the object list. Understand the
      format *before* writing a parser

**Deliverable:** `assets/maps/town.json`, and a written understanding of its structure.

**Journal:** how are tile IDs stored, and what is a GID offset?

---

### Day 44 - JSON parsing

**Goal:** turn that file into your own data structures.

**New concepts:** third-party dependencies via CMake, `nlohmann/json`, schema validation,
defensive parsing, separating file format from runtime format.

**Research first (25 min):** [data-driven-design.md](../research/data-driven-design.md)
sections 1-4.

**Tasks**

- [ ] Add `nlohmann/json` to `CMakeLists.txt` with `FetchContent`. You have now added a
      dependency yourself, which is a real skill:

```cmake
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
  GIT_SHALLOW 1)
FetchContent_MakeAvailable(json)
target_link_libraries(game PRIVATE raylib nlohmann_json::nlohmann_json)
```

- [ ] Parse a tiny hand-written JSON file first to learn the API, before attacking the map
- [ ] Define your **runtime** structures independently of Tiled's format:
      `struct TileLayer { std::string name; std::vector<int> tiles; };`
      `struct MapData { int width, height, tileSize; std::vector<TileLayer> layers; std::vector<ObjectDef> objects; std::vector<bool> solid; };`
- [ ] Write `std::optional<MapData> LoadMap(const std::string& path)` that converts Tiled
      JSON into `MapData`. Keep all Tiled-specific knowledge inside this one function, so
      swapping editors later touches one file
- [ ] Handle failure at every step: missing file, invalid JSON, missing keys, wrong layer
      count. Log the path and the reason, never crash
- [ ] Print a text dump of the loaded map (`.` for empty, `#` for solid) to confirm the
      parse is right before any rendering exists

**Deliverable:** `LoadMap` produces a verified `MapData`, printed as ASCII.

**Stuck?** Tiled tile IDs are 1-based with 0 meaning empty, and are offset by the tileset's
`firstgid`. Off-by-one here is the most common tilemap bug ever.

**Journal:** why keep Tiled's format out of your runtime structures?

---

### Day 45 - Rendering the tilemap

**Goal:** draw the map efficiently, with correct layering around the player.

**New concepts:** tile index to source rect, layer order, culling to the camera view,
draw-call cost, y-sorting.

**Research first (20 min):** [tilemaps.md](../research/tilemaps.md) sections 4-5.

**Tasks**

- [ ] Convert a tile index into a source rectangle in the tileset texture, and draw the
      `ground` layer
- [ ] Draw all layers in order, with the player drawn between `decor` and `overhead`
- [ ] Walk behind a tree using the `overhead` layer and confirm it looks right
- [ ] Naively draw every tile of a 200x200 map and watch the frame time in your overlay.
      Then cull to the camera's visible tile range and compare. Record both numbers in the
      journal - this is your first real optimisation, measured rather than guessed
- [ ] Compute the visible range from the camera: use `GetScreenToWorld2D` on the screen
      corners, convert to tile coordinates, clamp to the map, add one tile of margin
- [ ] Add a debug view (F4) drawing tile coordinates and solid tiles

**Deliverable:** a large map rendering at full speed, with correct layering, and a
before/after measurement.

**Journal:** frame time before and after culling.

---

### Day 46 - Map collision data

**Goal:** ask the map what is solid, cleanly.

**New concepts:** query APIs, coordinate conversion, boundary conditions.

**Tasks**

- [ ] Implement `bool MapData::IsSolid(int tx, int ty) const`, returning `true` for
      out-of-bounds so the player cannot walk off the map
- [ ] Implement `int TileFromWorld(float worldCoord, int tileSize)` correctly for negative
      coordinates. Test with -1.0, -0.5, 0.0, 15.9, 16.0 and check each result by hand.
      `(int)(-0.5f / 16)` is `0`, not `-1`, and that bug is invisible until you walk left
      off the map
- [ ] Implement `bool RectOverlapsSolid(Rectangle r)` checking every tile the rectangle
      touches, not just its corners (a rectangle bigger than a tile can straddle one)
- [ ] Add object-layer colliders from Tiled (arbitrary rectangles for things not aligned to
      the grid)
- [ ] Draw every solid tile and collider in the F1 overlay
- [ ] Write assertions for your coordinate conversions and run them at startup in debug
      builds

**Deliverable:** collision queries that are correct at map edges and negative coordinates.

**Journal:** the negative coordinate conversion. Write the fix, not just the problem.

---

### Day 47 - Tile collision resolution

**Goal:** the player cannot walk through walls and slides along them instead of stopping.

**New C++ concepts:** early return, `const&` parameters, integer division with negatives.

**Research first (20 min):** [collision-detection.md](../research/collision-detection.md)
sections 3-4.

**Tasks**

- [ ] Move the player on X only, then resolve X by snapping out of any solid tile
- [ ] Move on Y only, then resolve Y the same way
- [ ] Confirm sliding: hold up+right against a wall running north and slide along it
      smoothly instead of stopping dead
- [ ] Test all four directions and all four diagonal-into-corner cases
- [ ] Test a corridor exactly the player's collider width - you must fit through without
      catching on tile seams. If you catch, your resolution is snapping by penetration
      depth instead of to the tile edge
- [ ] Sub-step movement larger than half a tile so a 3000 px/s player still cannot tunnel
- [ ] Draw the collider, the attempted position, and the resolved position in the debug
      overlay in three different colours

**Deliverable:** walk into a wall from all four directions with no tunnelling, even at 10x
speed, and slide smoothly along walls.

**Stuck?** Hardcode a single solid tile at (5, 5) and get *that* working before loading
solidity from the map.

**Journal:** why do you resolve X and Y separately instead of resolving the combined
movement?

---

### Day 48 - Entities

**Goal:** many things in the world, created and destroyed safely.

**New concepts:** entity representation, stable IDs vs indices, spawn and destroy queues,
update order, generational handles (awareness).

**Research first (30 min):** [entity-architecture.md](../research/entity-architecture.md)
sections 7-9. Then make the decision *before* writing code.

**Tasks**

- [ ] Decide your entity model and record it as decision **D003** in the architecture doc.
      The recommended choice for this project: **one `Entity` struct with optional
      components and a `Kind` enum**, not a class hierarchy, and not a full ECS. Write down
      why you chose what you chose
- [ ] Sketch it out:

```cpp
enum class EntityKind { Player, Npc, Enemy, Chest, Trigger, Projectile };

struct Entity {
    int         id = 0;              // stable, never reused
    EntityKind  kind{};
    bool        alive = true;
    Vector2     position{};
    Vector2     velocity{};
    Rectangle   collider{};          // relative to position
    Animator    animator{};
    std::string dataId;              // key into the data tables
    // kind-specific extras kept small and obvious
};
```

- [ ] Write `class EntityWorld` with `int Spawn(Entity)`, `void Destroy(int id)`,
      `Entity* Find(int id)`, and `void Update(float dt)`
- [ ] **Never destroy during iteration.** Queue destructions and apply them after the
      update pass. You already learned this lesson on Day 39; recognise the same shape
- [ ] Prove why IDs beat indices: hold an index to entity 5, destroy entity 3, and watch
      your index point at the wrong entity. Then do the same with an ID and see it stay
      correct (or return `nullptr`)
- [ ] Spawn entities from the map's object layer, mapping Tiled `type` to `EntityKind`
- [ ] Y-sort entities so characters lower on the screen draw in front
- [ ] Add entity count and a per-kind breakdown to the debug overlay

**Deliverable:** entities spawned from the map, y-sorted, safely destroyable mid-update.

**Journal:** decision D003, with the reasoning you will want to reread in month five.

---

### Day 49 - The event bus

**Goal:** let systems talk without knowing about each other.

**New concepts:** the observer pattern, publish/subscribe, `std::function` callbacks,
event structs, the danger of unbounded indirection.

**Research first (25 min):** Game Programming Patterns - Observer and Event Queue chapters.

**Tasks**

- [ ] Define events as small structs: `struct EnemyDefeated { int entityId; std::string enemyId; };`
      `struct ItemPickedUp { std::string itemId; int count; };`
      `struct FlagChanged { std::string flag; int value; };`
- [ ] Write a minimal typed bus: `Subscribe<T>(callback)` and `Publish<T>(event)`. A
      `std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>>`
      is enough - keep it under 60 lines
- [ ] Use it for something real: a "+50 XP" floating label and a sound effect that both
      react to `EnemyDefeated` without the battle code knowing either exists
- [ ] Handle the ordering trap: publishing an event from inside a handler. Queue events and
      drain the queue once per frame, with a depth limit
- [ ] Add unsubscription (return a token from `Subscribe`) and use it in a state's
      `OnExit`. A dangling subscription from a destroyed state is a use-after-free waiting
      to happen
- [ ] Write the rule for yourself: use direct calls by default; use events when the
      publisher genuinely should not know the subscriber. Over-using an event bus makes
      control flow untraceable

**Deliverable:** a working event bus with unsubscription, used for exactly two real cases.

**Journal:** when is an event worse than a direct function call?

---

### Day 50 - Timers, tweens, and easing

**Goal:** the utility that makes everything later feel good, built once.

**New concepts:** timers, interpolation, easing functions, sequencing, callbacks on
completion.

**Research first (30 min):**
[animation-and-tweening.md](../research/animation-and-tweening.md) sections 4-7.

**Tasks**

- [ ] Write `class Timer` with `Start(duration)`, `Update(dt)`, `bool Finished()`,
      `float Progress()` (0 to 1), and looping support
- [ ] Implement `float Lerp(float a, float b, float t)` and a `Vector2` version yourself
- [ ] Implement easing functions: `EaseInQuad`, `EaseOutQuad`, `EaseInOutQuad`,
      `EaseOutBack` (overshoot), `EaseOutBounce`, `EaseOutElastic`
- [ ] Build a visualiser: draw each easing curve as a graph plus a square animating along
      it. Watch them side by side. This is 30 minutes that will pay off in every menu and
      every battle animation you ever make
- [ ] Write `class Tween` that animates a `float*` or returns a value, with an easing
      function and an `onComplete` callback
- [ ] Write `class Sequence` that runs steps in order: move, wait 0.2s, move back, call
      callback. Phase 5 battle animations are almost entirely this class
- [ ] Apply it immediately: menu cursor that eases into place, a damage number that pops up
      with `EaseOutBack` and fades, a hit flash

**Deliverable:** a tween/sequence library plus a visual comparison of your easing curves.

**Journal:** which easing feels right for a menu cursor, and which for an attack lunge?

---

### Day 51 - Camera follow

**Goal:** a camera that feels good rather than one that is merely correct.

**New concepts:** deadzones, smoothing (lerp with time-based damping), clamping to map
bounds, look-ahead, pixel snapping.

**Research first (20 min):**
[camera-and-coordinates.md](../research/camera-and-coordinates.md) sections 5-7.

**Tasks**

- [ ] Hard-lock the camera to the player first. Note how twitchy it feels
- [ ] Add smoothing with a time-based lerp. Verify it behaves identically at 30 and 144 FPS
      (a naive `lerp(cam, target, 0.1f)` per frame does not - fix it)
- [ ] Add a deadzone: the camera only moves when the player leaves a central box. Draw the
      deadzone in debug view and tune the size
- [ ] Clamp the camera so it never shows outside the map, and handle maps smaller than the
      screen (centre them instead of clamping)
- [ ] Snap the camera target to whole pixels before rendering, or your pixel art will
      shimmer. Toggle it on and off and look closely - the difference is dramatic
- [ ] Add a camera shake function that composes with follow (offset applied after target)
- [ ] Add `PanTo(target, duration, easing)` using Day 50's tweens, for cutscenes later

**Deliverable:** a camera with a deadzone, smoothing, clamping, pixel snapping, shake, and
scripted panning.

**Journal:** what did pixel snapping fix?

---

### Day 52 - Map transitions

**Goal:** multiple maps, connected doors, and no leaks between them.

**New concepts:** level loading, spawn points, state teardown, transition sequencing,
persistent vs per-map data.

**Tasks**

- [ ] Author a second map (`assets/maps/house.json`) with a door object linking back to the
      town
- [ ] Give door objects properties: `targetMap`, `targetSpawn`
- [ ] Implement `void FieldState::LoadMap(const std::string& id, const std::string& spawnId)`
      that tears down entities, loads the new map, and places the player at the named spawn
- [ ] Sequence the transition: fade out, load, place player, fade in. Reuse Day 39's
      transition and Day 50's tweens rather than writing anything new
- [ ] Decide what survives a map change and what does not. Party, inventory and flags
      persist; entities and tile data do not. Where does that persistent data live? (Not in
      `FieldState`.) Write the answer in the architecture doc
- [ ] Walk through the door 20 times in a row while watching memory usage. A leak per
      transition is easy to create and easy to miss
- [ ] Handle the edge case of a door leading to a missing map: log, refuse, do not crash

**Deliverable:** two maps you can walk between repeatedly with a clean fade and no leaks.

**Journal:** what data must outlive the field state, and who owns it now?

---

### Day 53 - Phase consolidation

**Goal:** turn 16 days of systems into one coherent, documented engine.

**Tasks**

- [ ] Integration pass: Title to Field to Pause, two maps, a door, entities from the object
      layer, camera follow, tile collision, event bus, tweens, F1 overlay, all running
      together
- [ ] Rewrite [../design/architecture.md](../design/architecture.md) properly: the diagram,
      the system inventory table, decisions D001-D003, and the invariants list
- [ ] Audit ownership: for every system, write who owns it and who merely uses it. Fix any
      case where two things believe they own the same data
- [ ] Run the sanitizer build and fix everything it reports
- [ ] Delete dead code. If it is not called, it is not an asset, it is a liability
- [ ] Write the Phase 4 plan in your own words before reading the file. Then compare - the
      differences are interesting either way
- [ ] Commit: `day 53: engine foundation complete`

**Deliverable:** a documented, leak-free engine foundation you understand completely.

---

## Phase 03 exit checklist

- [ ] `main` is five lines and `Game` owns the application
- [ ] I have a state stack with safe deferred transitions and fades
- [ ] Assets load once, missing files are obvious, nothing leaks
- [ ] No game code calls raylib input functions directly
- [ ] The game renders at a fixed internal resolution and scales crisply
- [ ] I can author a map in Tiled and load it, including object layers
- [ ] Tile collision resolves per axis, slides, and does not tunnel
- [ ] Entities use stable IDs and are destroyed via a queue
- [ ] I have tweens and easing, and I have seen the curves
- [ ] The camera has a deadzone, clamping, and pixel snapping
- [ ] I can walk between two maps repeatedly without leaking
- [ ] `architecture.md` is current and I could hand it to another programmer
