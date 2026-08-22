# Tilemaps

**Used on:** Days 43-46 (Tiled, loading, rendering, collision data), Day 123 (authoring
workflow), Day 152 (real tilesets).

Why this matters: tilemaps are how 2D RPGs build worlds cheaply. The tile grid is not just a
rendering trick - it is a data structure that makes collision, pathfinding, and level authoring
dramatically simpler. And because Phase 7 asks you to build 20-plus maps, the *authoring workflow*
matters as much as the code.

---

## 1. The core idea

A map is a grid of integers. Each integer indexes into a **tileset**: one image containing many
small tiles. Drawing the map means, for each cell, computing the source rectangle in the tileset
and blitting it.

Why this is powerful:

- A 100x100 map is 10,000 integers (40KB), not a 1600x1600 image.
- Collision becomes a lookup: `IsSolid(tx, ty)`.
- Editing is fast, because tiles are reusable.
- Repetition is free, which suits interiors and dungeons perfectly.

Pick your tile size early and never change it. 16x16 is the classic choice for a JRPG and pairs
well with a 480x270 or 640x360 internal resolution.

**Questions to answer**

- Given a tile index and a tileset image, how do you compute the source rectangle?
- Given a world position, how do you find the tile? (And what about negative positions - see
  [collision-detection.md](collision-detection.md) section 4.)
- How many tiles are visible at once at your internal resolution? (This is your culling budget.)

---

## 2. Layers

One grid is not enough. A typical RPG map uses three or four:

| Layer | Purpose | Drawn |
| --- | --- | --- |
| `ground` | Terrain base | Below entities |
| `decor` | Objects on the ground, wall bases | Below entities |
| `overhead` | Treetops, roof edges, upper walls | **Above** entities |
| `collision` (optional) | Solidity data only | Never |

The `overhead` layer is what lets the player walk *behind* a tree, and it is the single cheapest way
to make a flat tile grid feel like it has depth.

An **object layer** is different: it holds arbitrary shapes and points with properties, not grid
cells. This is where entity spawns, triggers, doors, and non-grid colliders live. Object layers are
what turn a tilemap from scenery into a level.

---

## 3. Solidity: two approaches

Decide this on Day 43 and write it down.

**A: a `solid` property on tileset tiles.** Mark tiles as solid once, in the tileset, and every map
using them inherits it.
Pros: no per-map work, impossible to forget. Cons: the same visual tile cannot be solid in one
place and passable in another.

**B: a dedicated collision layer.** Paint solidity separately from visuals.
Pros: total control, and you can paint colliders that do not match the art (very useful for making
movement feel better than it looks). Cons: extra authoring work, and easy to forget.

**Recommendation:** approach A as the default, plus object-layer rectangles for exceptions. You get
the low authoring cost of A with an escape hatch for the cases where it is wrong.

Whichever you choose, precompute a `std::vector<bool>` (or a bitset) of solidity at load time.
Querying tileset properties per frame is wasteful and indirect.

---

## 4. Rendering efficiently

Naively drawing every tile of a 200x200 map is 40,000 draw calls per layer per frame. That is slow
enough to notice, and it is your first real optimisation opportunity (Day 45).

**Cull to the camera.** Compute the visible tile range from the camera's world bounds
(`GetScreenToWorld2D` on the screen corners), clamp to the map, add a one-tile margin, and iterate
only that. A 480x270 view at 16px tiles is about 30x18 = 540 tiles, which is trivial.

Further options, in order of usefulness:

1. **Cull** - always do this.
2. **Skip empty tiles** (index 0) - free.
3. **Cache static layers to a `RenderTexture2D`** - draw the whole ground layer once, then blit one
   texture per frame. Only worth it if profiling says so, and it complicates animated tiles.
4. Batching by texture matters more when you have several tilesets; keep each map to one tileset
   image if you can.

**Measure before and after.** Day 45 asks you to write both frame times down, because "I optimised
it" without a number is a story, not an engineering result.

Animated tiles (water, torches) are usually handled by swapping the source rectangle over time
based on a shared global timer, so all water animates in sync.

---

## 5. Tiled and the JSON format

[Tiled](https://www.mapeditor.org/) is the standard free editor, and its JSON export is
straightforward to parse.

Things you must understand before writing the parser:

- **GIDs.** Tile IDs in map data are "global IDs": 0 means empty, and real tiles start at the
  tileset's `firstgid`. To get a tileset-local index: `localId = gid - firstgid`. Off-by-one here
  is the most common tilemap bug in existence.
- **Flip flags.** The top three bits of a GID encode horizontal, vertical, and diagonal flipping.
  If you allow flipping in the editor, you must mask those bits off before computing the local ID,
  or you will get wildly wrong tiles. Mask with `0x1FFFFFFF`.
- **Row-major data.** The `data` array runs left to right, top to bottom: index `y * width + x`.
- **Object coordinates** are in pixels, not tiles, and rectangle objects are positioned by their
  top-left corner while point and tile objects may not be. Check against the editor.
- **Custom properties** are where your `dialogueId`, `targetMap`, and `solid` values live. Define
  custom property *types* in Tiled so you cannot typo a field name.

**Keep Tiled's format out of your runtime structures.** Load into your own `MapData` in one
function. Then, if you ever change editors or hand-author a map, exactly one function changes.

---

## 6. Authoring workflow (the part that decides how much game you ship)

Code is only half of tilemaps. Before building 20 maps (Day 123):

- **Organise the tileset** by terrain, with clear collision markings. A messy tileset makes every
  map slower to build.
- **Make object templates** in Tiled for every entity type, with properties pre-filled. Placing an
  NPC should be one drag, not a drag plus five property edits.
- **Build prefab groups** you can copy-paste: a house, a shop interior, a dungeon room, a save
  point, a treasure alcove.
- **Write your conventions down**: layer names, standard room sizes, door naming, spawn point
  naming. Consistency lets you build maps without deciding anything.
- **Measure your time per map.** If a small room takes an hour, fix the workflow before making 20
  of them. Target 20-30 minutes.
- **Hot reload maps** (Day 120). Editing a map with a 2-second feedback loop is a completely
  different activity from editing with a 30-second one.

Also worth knowing: **autotiling** (Tiled calls it terrain sets) draws correct edges and corners
automatically as you paint. Setting it up costs 30 minutes and saves many hours across 20 maps.

---

## 7. Beyond the basics (know these exist)

- **Chunked maps** for very large worlds - unnecessary here.
- **Infinite maps** in Tiled - avoid, they complicate parsing for no benefit at your scale.
- **Pathfinding on the grid** - A* over the solidity grid is straightforward, and you would only
  need it if enemies must navigate around obstacles. Your Day 67 waypoint patrol avoids it.
- **Height and layers as floors** - some RPGs stack multiple z-levels. Do not; use separate maps.
- **Tile metadata** beyond solidity: footstep sound type, damage tiles, ice friction, tall grass.
  Cheap to add via tileset properties and a nice source of variety.

---

## Exercises

1. Author a small map with three tile layers and one object layer, export as JSON, and read the
   JSON by hand until you can predict what your parser must do.
2. Write the tile-index-to-source-rectangle conversion and verify it against the editor by drawing
   tile 0, 1, and the last tile in the set.
3. Deliberately forget the `firstgid` offset and observe the resulting garbage, so you recognise the
   symptom instantly.
4. Draw a 200x200 map naively, record the frame time, add culling, and record it again.
5. Dump a loaded map as ASCII (`.` for empty, `#` for solid) and confirm it matches the editor
   before writing any rendering code.
6. Set up one autotile terrain and paint a coastline with it.

## Done criteria

- [ ] I can explain GIDs, `firstgid`, and flip flags
- [ ] Tiled's format is confined to one loader function
- [ ] My layers include an overhead layer and entities draw between layers correctly
- [ ] Rendering is culled to the camera, with measured before/after frame times
- [ ] Solidity is precomputed at load time
- [ ] I have object templates, prefabs, written conventions, and a measured time per map

## Sources

- [Tiled manual](https://doc.mapeditor.org/en/stable/) - read "Introduction" and "Editing Tile
  Layers" properly, then skim the rest
- [Tiled JSON map format](https://doc.mapeditor.org/en/stable/reference/json-map-format/) - keep
  this open while writing the parser
- raylib `textures_sprite_anim` and `core_2d_camera` examples for the rendering side
- Search "Tiled terrain sets autotiling tutorial" before authoring many maps
