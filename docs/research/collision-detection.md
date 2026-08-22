# Collision detection and resolution

**Used on:** Day 34 (AABB), Days 46-47 (tile collision), Day 65 (pushable blocks).

Why this matters: collision is where "it mostly works" is not good enough. Players notice sticking
on wall seams, catching on corners, and slipping through gaps immediately, even if they cannot say
why the movement feels bad. The good news is that a top-down 2D game needs only the simplest form
of collision, done carefully.

---

## 1. Detection vs resolution

Two separate problems, and conflating them is the root of most collision confusion.

- **Detection**: are these two shapes overlapping? A boolean.
- **Resolution**: given that they are, where should the mover end up?

Detection is easy and well-defined. Resolution is a design decision: stop, slide, bounce, push,
or ignore. Most beginner collision problems are resolution problems.

**Questions to answer**

- What should happen when the player walks diagonally into a wall - stop entirely, or slide along
  it? Which does your favourite game do?
- Should the player push an NPC, be blocked by them, or pass through?

---

## 2. AABB - the only shape you need

An axis-aligned bounding box is a rectangle that never rotates. Overlap test:

```
overlap = a.x < b.x + b.width  && a.x + a.width  > b.x &&
          a.y < b.y + b.height && a.y + a.height > b.y
```

raylib gives you `CheckCollisionRecs` and `GetCollisionRec` (the overlap rectangle, which gives you
penetration depth on both axes for free).

Decide your edge convention: is touching exactly (`a.x + a.width == b.x`) a collision? Use strict
inequality so touching is not overlapping, and be consistent, or you will get one-pixel sticking.

**Collider is not sprite.** A character's collider should be a small box around their feet, maybe
16x8 pixels, not their whole 32x32 sprite. This one choice does more for how movement feels than
any algorithm - it lets the player's head pass in front of walls and makes tight gaps feel
generous. Draw both in the debug overlay and tune.

Circles (`CheckCollisionCircles`) are useful for aggro radii and explosion ranges. Rotated
rectangles and polygons (SAT) are not needed for this project - if you think you need them,
reconsider the design first.

---

## 3. Axis separation - the technique that makes movement feel right

Do not move diagonally and then resolve. Move and resolve **one axis at a time**:

```
position.x += velocity.x * dt;
ResolveX();                      // push out horizontally only
position.y += velocity.y * dt;
ResolveY();                      // push out vertically only
```

Why this matters: with combined movement you only know that you overlap, not which direction
caused it, so any resolution guess is sometimes wrong - producing the classic "stopped dead
against a wall while trying to slide along it" feel. With separated axes, running north-east into
a north-facing wall cancels only the north component, so you slide east naturally, with no special
cases.

**Snap to the edge, do not push back by penetration depth.** When resolving X, set the player's
edge exactly flush against the tile edge. Pushing back by penetration accumulates float error and
causes sticking at tile seams.

**Questions to answer**

- Why does resolving the combined movement lose information?
- If you resolve X first and then Y, can the order ever matter? (Yes, in corners - try it.)
- What happens if you resolve by penetration depth on a row of tiles that are all flush with each
  other?

---

## 4. Tile collision specifics

Tiles let you skip broad-phase entirely: from a rectangle you can compute exactly which tiles it
touches.

```
minTx = floor(rect.x / tileSize)
maxTx = floor((rect.x + rect.width  - epsilon) / tileSize)
```

Three details that cause real bugs:

1. **Negative coordinates.** `(int)(-0.5f / 16)` is `0`, but the correct tile is `-1`, because C++
   truncates toward zero. Use `std::floor` and then cast. This bug is invisible until the player
   walks off the left or top edge of a map.
2. **The exclusive edge.** A rectangle whose right edge is exactly at x=32 should not count tile 2
   as touched. Subtract a small epsilon, or use a half-open interval convention consistently.
3. **Check every touched tile, not just corners.** A collider larger than a tile can straddle a
   tile that none of its corners touch.

Treat out-of-bounds tiles as solid so the player cannot leave the map.

---

## 5. Tunnelling and sub-stepping

At high speed, a mover can pass entirely through a wall in one step, because at no point in a
frame did the two overlap.

The pragmatic fix for a 2D game: if the movement this step exceeds half a tile, split it into
several smaller steps and resolve each. Continuous collision detection (swept AABB) is the general
answer, and is worth reading about, but sub-stepping is simpler and sufficient here.

A fixed timestep (Day 35) helps a great deal, because it bounds how far anything can move per
step. Test with a deliberately absurd speed - 3000 px/s - because a projectile or a dash will find
this bug for you later otherwise.

---

## 6. Entity-vs-entity collision

Different from tile collision in one important way: both parties may be moving, so "who gets
pushed out" is a design decision.

Practical answers for a JRPG:

- **Player vs NPC:** the NPC is solid and immovable; the player resolves. Simple and predictable.
- **Player vs field enemy:** overlapping triggers a battle rather than a physical resolution
  (Day 68), so no resolution is needed at all.
- **Enemy vs enemy:** either ignore it (usually fine and much simpler) or apply a soft separation
  push so they do not stack.
- **Pushable blocks:** the player moves the block, the block checks its own destination against
  tiles and entities, and the move is refused if the block cannot fit. Tile-aligned block movement
  avoids a large amount of trouble.

For 50 entities, checking every pair is 1225 tests per frame, which is nothing. **Do not build a
quadtree.** If you ever need one, read about uniform grid spatial hashing first - it is simpler and
usually faster for evenly distributed 2D entities.

---

## 7. Triggers and queries

Not all collision resolves. Triggers only need detection plus enter/exit tracking:

- **Overlap now** is easy.
- **Just entered** requires remembering last frame's state. Without it, a trigger fires every frame
  while the player stands in it (a bug you will absolutely write on Day 64).
- **Just exited** is the same, inverted.

Other queries worth having: a point in a rectangle (mouse picking), a rectangle overlapping any
solid tile (a "can I fit here?" test used by pushable blocks and spawn placement), and nearest
entity within a radius (interaction and targeting).

---

## 8. Debugging collision

You cannot debug what you cannot see. Draw, on a toggle:

- every collider as an outline;
- solid tiles tinted;
- the attempted position and the resolved position in different colours;
- the velocity vector;
- trigger volumes and their conditions.

Then build a test room deliberately containing: a corridor exactly the player's width, a one-tile
gap, a corner to run into diagonally, a long wall to slide along, and a pit to test map bounds.
Walk it after every collision change. Thirty seconds of testing beats a bug report from a player
two months later.

---

## Exercises

1. Implement AABB overlap by hand before using `CheckCollisionRecs`, and test the touching-edge
   case.
2. Resolve a collision both ways (combined and axis-separated) and compare sliding along a wall.
3. Write the world-to-tile conversion and test it at -1.0, -0.5, 0.0, 15.9, 16.0, and 16.1,
   checking each result by hand.
4. Build a corridor exactly the player's collider width and get through it without catching.
5. Set the speed to 3000 px/s, watch the tunnelling, then fix it with sub-stepping.
6. Implement enter/exit tracking for a trigger and confirm it fires exactly once per entry.

## Done criteria

- [ ] My collider is separate from my sprite and tuned for feel
- [ ] I resolve X and Y separately and can explain why
- [ ] I snap to edges rather than pushing back by penetration
- [ ] World-to-tile conversion is correct for negative coordinates
- [ ] No tunnelling at 10x normal speed
- [ ] Triggers fire exactly once per entry
- [ ] I have a debug view and a test room for collision

## Sources

- [Standardized Sub-Pixel Variable Precision 2D Collision](https://higherorderfun.com/blog/2012/05/20/the-guide-to-implementing-2d-platformers/) -
  written for platformers, and the resolution discussion applies directly
- [Game Programming Patterns: Spatial Partition](https://gameprogrammingpatterns.com/spatial-partition.html) -
  read for understanding, do not implement yet
- raylib `shapes_collision_area` and `core_2d_camera` examples
- Search "swept AABB collision" once you understand the basic approach
