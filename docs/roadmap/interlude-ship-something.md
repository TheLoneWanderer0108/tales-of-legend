# Interlude - Ship something tiny (Days 36a-36f)

**Where this goes:** immediately after [phase-02-raylib.md](phase-02-raylib.md) (Day 36) and before
[phase-03-engine.md](phase-03-engine.md) (Day 37). Six days.

**Why it exists.** Tales of Legend will not be finished for another 130-odd days. That is a long time to
work without ever completing anything, and it is the single biggest reason projects like this get
abandoned - not difficulty, but the absence of any finish line for months on end.

So before starting the engine, you are going to build and **publicly release** a tiny complete game. Not
a prototype, not a demo: a finished, packaged, downloadable game that a stranger can play.

What this buys you:

- You learn the entire shipping pipeline (packaging, asset paths, user directories, store page, receiving
  feedback) while the stakes are trivial, instead of learning it for the first time on Day 167 with eight
  months of work on the line.
- Someone plays something of yours in month two rather than month seven.
- You find out what "finished" actually costs. Every beginner underestimates it, and finding out now
  changes how you plan the rest.
- **If you ever abandon the JRPG, you are still someone who has shipped a game.** That safety net matters
  more than it sounds.

Finishing is a separate skill from building, and this is where you practise it cheaply.

---

## The rules

Read these before choosing your game. They are the whole exercise.

1. **Six days. Hard stop.** If it is not finished on day six, cut features until it is. This constraint
   is the lesson, not an obstacle to it.
2. **One screen, one mechanic, no content.** Snake, Breakout, Asteroids, Flappy Bird, a one-screen dodge
   game, a timing/reflex game. No levels to author, no story, no save system.
3. **No architecture.** One or two files. No state machine, no asset manager, no entity system - you
   build those in Phase 3. Deliberately write the messy version, so that in three weeks you have felt
   *why* the engine phase exists. That contrast is worth more than any explanation.
4. **Colored rectangles.** Same rule as the main project.
5. **It must actually be published**, with a page a stranger could find, and at least three people who are
   not you must have played it.
6. **Do not reuse this code later.** It is a throwaway, and knowing that is what lets you write it fast.

A timing/reflex game is a particularly good choice here: it rehearses the exact input-precision problems
your action commands will need on Day 82, and it is genuinely fun in 60 seconds.

---

### Day 36a - Choose and scope

**Goal:** a written one-page spec you are confident you can finish in five more days.

**Tasks**

- [ ] Pick your game. Bias hard toward too simple; you will still find it takes longer than you think
- [ ] Write a one-page spec: the core mechanic in one sentence, the win condition, the lose condition, the
      controls, and what is on screen
- [ ] Write the cut list *first*: the three features you already know you will drop if you run short.
      Deciding this on day one is much easier than deciding it in a panic on day five
- [ ] Create a new folder, `sandbox/microgame/`, with its own CMake target. Keep it entirely separate from
      the JRPG
- [ ] Get a window open with your player object moving, today. Ending day one with something on screen sets
      the pace for the whole week

**Deliverable:** a one-page spec, a cut list, and something moving on screen.

---

### Day 36b - The core mechanic

**Goal:** the thing that makes it a game, working and fun.

**Tasks**

- [ ] Implement the single core interaction and nothing else
- [ ] Tune it until it feels good *before* adding anything around it. If the core is not fun in isolation,
      no amount of scoring, menus, or polish will save it - and finding that out today is exactly why you
      built it first
- [ ] Use delta time for everything (Day 26) and your fixed timestep if the game needs precision (Day 35)
- [ ] Get someone in the room to try it for 30 seconds and watch their face

**Deliverable:** a mechanic that is fun to repeat for a minute with no scoring or goal attached.

---

### Day 36c - The complete loop

**Goal:** a game that can be won, lost, and started again. This is the day it stops being a toy.

**Tasks**

- [ ] Add the win and lose conditions
- [ ] Add scoring, and a persistent high score in a simple text file
- [ ] Add the full loop: title or ready state, play, game over, restart. Restarting must fully reset state
      - stale state on restart is one of the most common bugs in tiny games, and you will find it today
- [ ] Add difficulty progression: it must get harder the longer you survive
- [ ] Play it 20 times in a row. Fix whatever annoys you most

**Deliverable:** a complete, replayable game loop with no dead ends.

---

### Day 36d - Juice and audio

**Goal:** make it feel good. You have one day, which is a useful forcing function.

**Tasks**

- [ ] Add sound to every action: input, score, collision, win, lose. Randomise pitch by ±10%
      ([audio.md](../research/audio.md) section 3)
- [ ] Add particles for the two most important events, plus a screen shake and a flash on impact
- [ ] Add easing to anything that moves in the UI ([animation-and-tweening.md](../research/animation-and-tweening.md)
      section 5), and add hitstop if your game has impacts
- [ ] Add music, or accept silence and note how much worse it feels
- [ ] Compare a recording from today with one from Day 36b. That difference is what "polish" means, and
      seeing it on a small scale teaches you to trust the process on the large one

**Deliverable:** a game that feels considerably better than it did yesterday, with the same mechanics.

---

### Day 36e - Package it

**Goal:** a build that runs on a machine that has never seen a compiler. This is the day with the real
lessons.

**Research first (30 min):** [shipping.md](../research/shipping.md) sections 1-5.

**Tasks**

- [ ] Make a release build: `-O2`, debug keys removed, a version number on screen
- [ ] Fix asset paths so the game runs from **any** working directory. Test it: `cd /tmp` and launch by
      absolute path. This one bug catches nearly everyone on their first release
- [ ] Write the high score file to a per-user directory, not next to the binary, and confirm the game works
      with its install directory made read-only
- [ ] Package for Linux: binary plus assets plus a launch script, and test it in a clean container or VM
      with no development tools
- [ ] Cross-compile for Windows if it goes smoothly, and abandon it within two hours if it does not. A
      Linux-only first release is completely respectable
- [ ] Write a short README for players: controls, requirements, credits
- [ ] Create `CREDITS.md` listing every asset, its source, and its licence - practise the habit now

**Deliverable:** a package verified working on a machine that is not your development environment.

**Journal:** what broke when you ran it somewhere else? That answer is why this interlude exists.

---

### Day 36f - Release and reflect

**Goal:** publish it, get it played, and extract the lessons.

**Research first (20 min):** [shipping.md](../research/shipping.md) sections 6-8.

**Tasks**

- [ ] Create the itch.io page: title, a one-line hook, a description, three screenshots, and a short GIF
- [ ] Upload the build, set it free, and publish. Verify the download works from a logged-out browser
- [ ] Get at least three people to play it, and **watch one of them in silence** - no explaining, no
      hinting. Write down every moment of confusion. This is the skill Day 163 depends on
- [ ] Write a short devlog post: what you built, what surprised you, what you would do differently
- [ ] Write the retrospective in [../journal.md](../journal.md), and answer these honestly:
      - How long did "the last 10%" actually take compared with what you expected?
      - What did the messy one-file architecture cost you by the end?
      - What did watching someone play teach you that testing it yourself did not?
      - What will you do differently on Tales of Legend because of this week?
- [ ] Tag it in git, then close the folder and do not touch it again
- [ ] Commit: `interlude: shipped a tiny game`

**Deliverable:** a released game with a public page and three players.

---

## Interlude exit checklist

- [ ] It is finished, not paused - there is no "I'll come back to it"
- [ ] It is published somewhere a stranger can download it
- [ ] Three people who are not me have played it
- [ ] It runs from any working directory, on a machine with no compiler
- [ ] User files go to a per-user directory and the install can be read-only
- [ ] Every asset's licence is recorded
- [ ] I watched someone play in silence and wrote down what confused them
- [ ] I wrote the retrospective, including how long the last 10% took

---

## What to carry into Phase 3

Write these down before starting Day 37, while the experience is fresh:

- **What the last 10% cost.** Whatever multiple it turned out to be, apply it mentally to the whole
  roadmap. If polishing and packaging a Snake clone took two days, packaging a JRPG will take longer than
  Days 167-169 suggest, and now you know that from experience rather than from a warning.
- **Where the mess hurt.** By day six, the one-file version was probably annoying you: state tangled in
  globals, restart logic scattered, no clean way to add a screen. Note exactly *where* it hurt. Phase 3
  builds the state stack, the asset manager, and the entity system specifically to solve those problems,
  and you will build them far better having felt the pain first.
- **What juice was worth.** You now have a measured sense of how much a day of polish buys. That
  calibration is what will stop you either skipping Phase 8 or disappearing into it.
- **That you can finish.** Keep this one in mind around Day 120, which is where this project will feel
  longest.
