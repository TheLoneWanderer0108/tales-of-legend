# Phase 08 - Polish and ship (Days 151-170)

Twenty days to turn a complete game into a released one. Two things happen here: the game
finally stops looking like coloured rectangles, and you learn the last skill of game
development, which is **finishing**.

The hardest part of this phase is psychological. At Day 150 you have a playable game and the
temptation to keep adding to it forever is enormous. Adding content is comfortable; releasing
is exposing. Do it anyway. A shipped small game teaches you more than an unshipped large one,
and you cannot start your second game until you finish your first.

**The rule for this phase:** no new features and no new content. Everything from here makes
existing things better, more robust, or more finished. Keep a `docs/next-game.md` file and put
every new idea in it. That file is a gift to your future self, not a to-do list.

Research notes for this phase:
[particles-and-vfx.md](../research/particles-and-vfx.md),
[shaders.md](../research/shaders.md),
[performance.md](../research/performance.md),
[audio.md](../research/audio.md),
[shipping.md](../research/shipping.md).

Trimmed: Days 158, 161, 162, 165 removed.

---

## Block A - Art and audio (Days 151-157)

### Day 151 - Art direction

**Goal:** decide what the game looks like before drawing 200 assets.

**Tasks**

- [ ] Decide the art approach honestly, based on your own ability and your remaining days.
      Drawing everything yourself in a deliberately simple style beats an ambitious style you
      cannot sustain across 200 assets
- [ ] Fix the constraints and write them in the design doc: tile size (16x16), character sprite
      size, a palette of 24-32 colours, outline style, and shading rules (usually flat plus one
      shadow tone)
- [ ] Build the palette as an actual image file and use only those colours. A limited palette
      makes amateur art look intentional and cohesive, which is the single biggest visual win
      available to a solo developer
- [ ] Draw one finished example of each asset class to set the standard: one tile set piece,
      one character, one enemy, one UI panel, one effect
- [ ] Make a full asset list with counts, and multiply by your measured time per asset. If the
      total exceeds your budget, simplify the style now
- [ ] For any assets you will not draw, pick CC0 sources and record the licence for each one
      immediately in `assets/CREDITS.md`. Chasing licences later is miserable

**Deliverable:** written art constraints, a palette file, five reference assets, and a
budgeted asset list.

**Journal:** your measured minutes per asset, and what that means for the plan.

---

### Days 152-155 - The art pass

Four days replacing placeholders. Work in this order so the most visible things improve first.

**Day 152 - Tiles and environments**

- [ ] Draw the tilesets for all three chapters' environments
- [ ] Add autotile-friendly edge and corner pieces so maps look finished at their borders
- [ ] Add decoration tiles and animated tiles (water, torches, sparkles)
- [ ] Redecorate every map with the real tileset

**Day 153 - Characters and enemies**

- [ ] Draw the player and party members: idle, walk in four directions, and one action pose
- [ ] Draw battle sprites: idle, attack, hurt, defeated, and a victory pose
- [ ] Draw every enemy, with the boss getting the most attention
- [ ] Ensure silhouettes read clearly at actual size, and check them zoomed out - if you cannot
      identify an enemy by its silhouette, redraw it

**Day 154 - UI and portraits**

- [ ] Draw the UI panels as nine-slice frames, plus the cursor, buttons, and every icon
- [ ] Draw dialogue portraits with 3-4 expressions each for major characters
- [ ] Draw the action command prompts and gauges - these are gameplay-critical and must be the
      clearest art in the game
- [ ] Draw the logo and title screen art

**Day 155 - Effects and integration**

- [ ] Draw effect sprites: hit sparks, slashes, magic effects, level-up rays, status icons
- [ ] Replace every remaining placeholder, then grep for `placeholder` and `TODO` in the asset
      paths to catch stragglers
- [ ] Play the whole game looking only at the visuals and list everything inconsistent
- [ ] Fix the top ten inconsistencies

**Deliverable (Day 155):** no placeholder art remains anywhere in the game.

**Journal (Day 155):** compare a screenshot with one from Day 95.

---

### Day 156 - The audio pass

**Goal:** complete, mixed, consistent audio.

**Research first (20 min):** [audio.md](../research/audio.md) sections 4-8.

**Tasks**

- [ ] Complete the music list: title, three chapter overworld themes, a town theme, a dungeon
      theme, battle, boss, victory, a sad story theme, and the ending. Roughly 10-12 tracks.
      Compose them, or use CC0 tracks and credit them properly
- [ ] Ensure every looping track loops seamlessly. A clumsy loop point becomes intolerable
      after 20 minutes
- [ ] Complete the sound effect list and check that every single player action makes a sound.
      Silent actions feel broken even when they work
- [ ] Mix everything against a reference: play a commercial game, note its relative levels, and
      match. Music should sit under the effects, and action command cues must sit on top
- [ ] Implement music transitions: crossfade between areas, a battle sting, and a return to
      field music at the right position
- [ ] Test with headphones and with laptop speakers. Bass-heavy mixes vanish on small speakers
- [ ] Test at 50% and 100% volume, and with music muted (some players always mute music - the
      game must still be readable and playable through effects alone)

**Deliverable:** complete, seamlessly looping, properly mixed audio.

**Journal:** what did you fix after listening on laptop speakers?

---

### Day 157 - The juice pass

**Goal:** the layer of feedback that separates good from great.

**Tasks**

- [ ] Audit every player action for feedback across four channels: visual, audio,
      motion/animation, and timing (hitstop or pause). Anything with fewer than two channels
      feels weak
- [ ] Add or improve: screen transitions between every state, damage number variety, a
      low-HP heartbeat and vignette, level-up spectacle, chest-opening sparkle, footstep dust,
      menu open and close animations, and text pop for important words
- [ ] Add anticipation and follow-through to important animations. A wind-up before an attack
      and a settle after it are what make sprites feel like they have weight
- [ ] Tune every animation duration by feel. Most are too slow; try cutting them by 20% and see
      how much snappier the game becomes
- [ ] Add camera work: a zoom punch on criticals, a subtle drift in cutscenes, a pan across a
      room on entry
- [ ] Play a game you admire for 15 minutes, then play yours. Note every place theirs feels
      more responsive, and fix the top five
- [ ] Record a video and compare it with your Day 95 recording

**Deliverable:** every player action has at least two channels of feedback.

**Journal:** the five things you fixed after comparing with a game you admire.

---

## Block B - Technical hardening (Days 158-162)

### Day 159 - Performance

**Goal:** a stable frame rate, verified by measurement rather than by feeling.

**Research first (25 min):** [performance.md](../research/performance.md) in full.

**Tasks**

- [ ] Build a frame time graph in the debug overlay showing the last 120 frames, so spikes are
      visible rather than merely felt
- [ ] Profile properly with `perf record` / `perf report` (or your profiler of choice). Measure
      before optimising anything - your intuition about the bottleneck will be wrong
- [ ] Find the actual hotspots. In a 2D game they are usually: too many draw calls, per-frame
      allocations, uncached loads, or an accidental O(n²) loop over entities
- [ ] Reduce draw calls where it matters: batch same-texture draws, cull off-screen entities,
      and consider caching static map layers to a render texture
- [ ] Eliminate per-frame allocations in the hot path: reserve vectors, reuse buffers, and pool
      particles and damage numbers
- [ ] Test the worst case: the most crowded map, the largest battle, and every particle effect
      firing at once. Frame time must stay under 16ms there, not merely on average
- [ ] Check load times: startup, map transitions, and battle entry. Anything over about a
      second needs attention
- [ ] Record before and after numbers for every optimisation. Any change you cannot measure did
      not happen

**Deliverable:** a stable 60 FPS in the worst case, with recorded before/after measurements.

**Journal:** where was the real bottleneck, and where did you expect it to be?

---

### Day 160 - Robustness

**Goal:** the game does not crash or lose data on anyone's machine.

**Tasks**

- [ ] Run a full playthrough under `-fsanitize=address,undefined` and fix everything reported
- [ ] Run a full playthrough under valgrind if you can tolerate the slowness, then fix any
      remaining leaks
- [ ] Test with missing assets: delete a texture, a sound, and a data file, and confirm the game
      warns and continues rather than crashing
- [ ] Test with corrupted saves once more, including saves from an older schema version, and
      confirm migration works
- [ ] Add a crash handler that writes a log with the game version, the last known state, and a
      stack trace if you can manage it. A crash report you can read is worth ten "it crashed"
      bug reports
- [ ] Add logging to a file with levels, so a player can send you something useful
- [ ] Test edge cases in every system: a full inventory, zero gold, max level, every character
      KO'd, 999 damage, a 60-minute battle
- [ ] Leave the game running for two hours idle, then play. Look for accumulated leaks or timer
      overflow
- [ ] Test alt-tabbing, minimising, resizing, unplugging the controller mid-battle, and
      changing audio devices while playing

**Deliverable:** a clean sanitizer playthrough and graceful handling of every failure you can
manufacture.

**Journal:** which robustness test found a real bug?

---

## Block C - Testing and release (Days 163-170)

### Day 163 - External playtesting

**Goal:** find out what your game is actually like for someone who is not you.

**Tasks**

- [ ] Find 3-5 people. Variety matters more than number: someone who plays JRPGs, someone who
      does not, and someone who is bad at action games
- [ ] Give them a build and nothing else. No explanation, no hints, no context. What they need
      to be told is a design bug
- [ ] Watch at least one session in person or over a screen share, in **silence**. This is the
      hardest and most valuable discipline in the whole roadmap. Write down every hesitation,
      every wrong turn, every repeated failure
- [ ] Give the remote testers a short feedback form: where did you get stuck, where were you
      bored, what was confusing, what was your favourite moment, did you finish, and if not why
      did you stop
- [ ] Add analytics-style logging to the build: time per chapter, deaths per encounter, action
      command success rates, items used, where sessions ended. Real data beats recollection
- [ ] Do not fix anything today. Only collect

**Deliverable:** feedback from 3-5 players plus logged play data.

**Journal:** the single most surprising thing you watched someone do.

---

### Day 164 - Triage and fix, round one

**Goal:** act on the feedback rationally rather than emotionally.

**Tasks**

- [ ] Write down every piece of feedback as a single line, then group duplicates. Anything two
      or more testers hit is a real problem, not a matter of taste
- [ ] Classify each: bug, confusion, balance, pacing, or preference. Fix bugs and confusion
      first; those are objectively wrong. Treat preferences with suspicion
- [ ] Sort by (players affected) divided by (hours to fix), and work top-down
- [ ] Fix the top 15 items. Expect the list to be dominated by unclear signposting rather than
      by mechanics - that is normal and is exactly what external testing is for
- [ ] Use the logged data for balance: any encounter with an unexpectedly high death rate, any
      action command with a success rate under about 40%, and any chapter taking twice as long
      as intended
- [ ] Keep the ideas you are not fixing in `docs/next-game.md` rather than deleting them

**Deliverable:** the top 15 issues fixed, with the rest triaged and recorded.

**Journal:** what did you decide *not* to fix, and why was that the right call?

---

### Day 166 - Final fixes and polish

**Goal:** the last fix day. After today, only release work.

**Tasks**

- [ ] Fix the round-two issues in priority order
- [ ] Fix every remaining known bug, or document any you consciously choose to ship with. A
      known, documented bug is a decision; an unknown one is a risk
- [ ] Do a final consistency pass: naming, capitalisation, punctuation in dialogue, icon style,
      and menu terminology used identically everywhere
- [ ] Verify the credits are complete and every asset licence is honoured. This is a legal and
      an ethical requirement, not a nicety
- [ ] Set the version number to 1.0.0 and display it on the title screen
- [ ] **Declare feature freeze.** Write it in the journal with the date. Everything from now
      goes in `next-game.md`
- [ ] Commit: `day 166: feature freeze, v1.0.0-rc1`

**Deliverable:** a release candidate with a documented known-issues list.

**Journal:** the feature freeze declaration, and how it feels to write it.

---

### Day 167 - Packaging

**Goal:** a build that runs on a machine that has never seen a compiler.

**Research first (30 min):** [shipping.md](../research/shipping.md) sections 1-5.

**Tasks**

- [ ] Set up a proper release build: `-O2`, no debug symbols in the shipped binary, sanitizers
      off, and debug keys disabled or hidden behind a flag
- [ ] Build a Linux package: the binary plus assets in a clean directory tree, with a launch
      script that sets the working directory correctly. Static-link what you sensibly can to
      reduce dependency problems
- [ ] Build for Windows by cross-compiling with mingw-w64 from WSL, which CMake supports via a
      toolchain file. Test the resulting `.exe` on real Windows
- [ ] Test each package on a clean machine or a fresh VM or container with no development tools
      installed. This is the only test that proves packaging works
- [ ] Verify assets load from the package layout, not from your source tree. A hardcoded
      development path is the single most common shipping bug
- [ ] Confirm saves and config write to an appropriate per-user location, not next to the
      binary, and that they work when the install directory is read-only
- [ ] Check the package size and strip anything unused
- [ ] Write a README for players: controls, requirements, credits, and how to report a bug

**Deliverable:** Linux and Windows packages, each verified on a clean machine.

**Journal:** what broke when you ran the package on a clean machine?

---

### Day 168 - Store presence

**Goal:** somewhere for people to actually get the game.

**Research first (20 min):** [shipping.md](../research/shipping.md) sections 6-8.

**Tasks**

- [ ] Create the itch.io page: title, a one-line hook, a description, tags, and pricing (free or
      pay-what-you-want removes all friction for a first release)
- [ ] Capture 5-6 screenshots showing distinct things: the overworld, a battle mid-action
      command, dialogue, a menu, the boss. Use the game's most attractive moments, and turn off
      the debug overlay
- [ ] Record a 30-60 second trailer: gameplay only, the best action command moments, no long
      logo intro. OBS records fine on Linux and WSLg
- [ ] Make a capsule image and a GIF, since animation dramatically increases clicks
- [ ] Write the description around your three pillars. Say what the game *is* in the first
      sentence, not what inspired it
- [ ] Upload builds for both platforms and verify the downloads work from a browser you are not
      logged into
- [ ] Set up a way to receive bug reports: itch comments, a form, or an email address in the
      README

**Deliverable:** a live store page with builds, screenshots, and a trailer.

**Journal:** what was hardest to describe about your own game?

---

### Day 169 - Release QA

**Goal:** the final gate. Nothing ships untested.

**Tasks**

- [ ] Play the **shipped package** (not a development build) from title screen to credits,
      start to finish, in one sitting. Every bug you find today is a bug your players would
      have found
- [ ] Work through a written QA checklist and tick every line: new game, load game, every menu,
      every settings option, rebinding, every chapter, every boss, save and load at ten points,
      game over and continue, the credits, and quitting cleanly
- [ ] Test the failure paths one final time: corrupted save, missing asset, no audio device, no
      controller, alt-tab, and a window resize mid-battle
- [ ] Verify version numbers, credits, and licences one final time
- [ ] Fix only release blockers. Anything else goes into a 1.0.1 list
- [ ] Rebuild the final packages and re-verify their checksums and downloads
- [ ] Take a backup of everything: the repository, assets, and the release builds, in two places

**Deliverable:** a fully QA'd release build with a written checklist you completed.

**Journal:** the last bug you found before release.

---

### Day 170 - Ship it

**Goal:** publish, then reflect properly. This is the day the roadmap exists for.

**Tasks**

- [ ] Publish the itch.io page
- [ ] Tag the release in git: `git tag -a v1.0.0 -m "Tales of Legend 1.0.0"` and push the tag
- [ ] Post it somewhere: r/raylib, the raylib Discord, r/gamedev's feedback thread, or a
      handful of friends. Do not expect an audience; expect the experience of releasing
- [ ] Write a devlog post about building it. It clarifies your own learning and it is what a
      future employer or collaborator will read
- [ ] **Write the full retrospective** in the journal, and take real time over it:
      - What went right, and what specifically caused that
      - What went wrong, and what you would do differently
      - Which day was the hardest, and which was the most satisfying
      - What you know now that you did not know on Day 1
      - Which C++ concepts you still do not trust, and how you will address them
      - Which systems you are proud of, and which you would rewrite
      - How long each phase actually took versus the estimate
- [ ] Read your Day 1 journal entry, then read your Day 170 entry. That distance is the real
      deliverable of this roadmap - the game is just the evidence
- [ ] Write the honest first paragraph of `docs/next-game.md`: what you want to build next, and
      what you will do differently
- [ ] Commit: `day 170: shipped v1.0.0`

**Deliverable:** a released game, a devlog, and a retrospective worth rereading.

---

## Phase 08 exit checklist

- [ ] No placeholder art or audio remains
- [ ] Music loops seamlessly and the mix works on laptop speakers
- [ ] Every player action has at least two channels of feedback
- [ ] 60 FPS in the worst case, verified by profiling with recorded numbers
- [ ] A full playthrough is clean under sanitizers
- [ ] Missing assets and corrupted saves are handled gracefully
- [ ] 3-5 external testers played it and I watched at least one in silence
- [ ] Feedback was triaged by impact and the top issues fixed
- [ ] Feature freeze was declared and honoured
- [ ] Packages run on clean Linux and Windows machines
- [ ] A store page is live with screenshots and a trailer
- [ ] A full QA playthrough of the shipped package is complete
- [ ] It is released, tagged, and someone other than me has played it
- [ ] I wrote the retrospective

---

## After Day 170

Do not immediately start a bigger project. Two suggestions:

1. **Support 1.0.1 for two weeks.** Fix what real players report. Shipping a patch based on
   real bug reports is a skill you have not practised yet, and it is a large part of
   professional work.
2. **Then read your own code.** Pick the system you are least proud of and rewrite it in a
   weekend, purely as an exercise. You will be startled by how much better you can do it now.
   That gap, not the game, is the proof that the 170 days worked.

Your second game will take a third of the time and be three times better. That is the whole
point.
