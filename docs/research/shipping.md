# Shipping

**Used on:** Day 167 (packaging), Day 168 (store page), Days 169-170 (QA and release).

Why this matters: shipping is a skill, and it is the one most hobby developers never practise. A game
that runs on your machine is not a game anyone else can play, and the gap between the two is larger than
it looks - paths, dependencies, permissions, first-run behaviour, and licences all have to be right.

There is also a psychological component. At Day 150 you will have a complete game and an enormous urge to
keep polishing rather than release. Recognise that urge for what it is: releasing is exposing, and
polishing is comfortable. Ship anyway.

---

## 1. The release build

Different from your development build in specific ways:

- `-O2` optimisation, `NDEBUG` defined (which disables `assert`).
- No sanitizers.
- Debug keys, debug overlays, and the developer console disabled or gated behind a launch flag.
- Version number compiled in and shown on the title screen.
- Strip symbols from the shipped binary (`strip`), but **keep an unstripped copy** so you can interpret a
  crash report later.

Set a version scheme and use it: `1.0.0`, then `1.0.1` for fixes. Put the version in the window title,
the title screen, and any log file - "which build were you playing?" is the first question of every bug
report.

---

## 2. Asset paths - the classic shipping bug

Your game works in development because you run it from the project root and assets are at
`assets/sprites/x.png`. A player runs it from anywhere, or double-clicks it from a different directory,
and every load fails.

Fixes:

- Call `ChangeDirectory(GetApplicationDirectory())` at startup so relative paths resolve next to the
  executable, or
- resolve every asset path against the application directory explicitly.

Then **verify by running the packaged game from a different working directory**, e.g. `cd /tmp` and run it
by absolute path. If it works there, your paths are right.

Related: never hardcode an absolute path. Grep for `/home/` in your source before shipping - it is a
surprisingly common mistake and it fails silently for you and loudly for everyone else.

---

## 3. Where user files go

Saves, config, and logs must **not** live next to the binary. The install directory may be read-only, and
multiple users may share a machine.

- Linux: `$XDG_DATA_HOME/tales-of-legend` (fall back to `~/.local/share/tales-of-legend`), and
  `$XDG_CONFIG_HOME` for config.
- Windows: `%APPDATA%\tales-of-legend`.

Create the directory if it does not exist, and handle failure (a read-only home, a full disk) without
losing the in-memory game. Test with the install directory made read-only (Day 167).

---

## 4. Packaging for Linux

The tricky part is dynamic libraries. A binary built on your machine may want library versions the
player's machine does not have.

Options, in order of practicality:

- **Static-link what you reasonably can.** raylib is straightforward to link statically, which removes the
  biggest dependency.
- Ship the binary plus assets in a single directory with a launch script that sets the working directory.
- **AppImage** bundles everything into one executable file and is the most player-friendly Linux option
  if you want to invest an hour.
- Flatpak is more work and mostly worth it only for wider distribution.

Test in a **clean container or VM** with no development tools installed. This is the only test that
proves your packaging works - your own machine has everything, which is exactly why it cannot tell you
anything.

---

## 5. Packaging for Windows from WSL

Cross-compiling with mingw-w64 is well-supported and avoids maintaining a second development environment.

- Install `mingw-w64` and write a CMake toolchain file specifying the cross-compiler.
- Configure a separate build directory with `-DCMAKE_TOOLCHAIN_FILE=...`.
- raylib builds for Windows this way without special handling.
- The result is a `.exe` plus assets. Include any required runtime DLLs, or link statically to avoid them.

**Test the `.exe` on real Windows**, not under Wine. Wine is a useful smoke test but it is not proof.
Check: the window opens, audio works, a gamepad works, saves write to `%APPDATA%`, and the game survives
alt-tab.

If cross-compiling turns into a rabbit hole, a Linux-only release is a perfectly respectable first
release. Do not let packaging block shipping.

---

## 6. The store page

itch.io is the right home for a first release: free, no gatekeeping, and a community that plays small
games.

What you need:

- **Title and a one-line hook.** Say what the game *is*, not what inspired it. "A JRPG where every attack
  and every block is a timed button press" beats "a Paper Mario-inspired RPG".
- **A short description** built around your three design pillars, with the hook in the first sentence.
  Most readers stop after two lines.
- **5-6 screenshots** showing distinct things: the overworld, a battle mid-action-command, dialogue, a
  menu, the boss. Turn off the debug overlay. Use your game's best-looking moments.
- **A GIF or short trailer.** Animation dramatically increases clicks, and an action command in motion
  is the most compelling thing you have to show.
- **A capsule image.**
- **Pricing:** free or pay-what-you-want. For a first release, removing all friction matters more than
  revenue.
- **Tags** so it is findable: RPG, turn-based, pixel art, singleplayer.
- **Platform builds** uploaded and marked correctly, with clear system requirements.

---

## 7. The trailer

30-60 seconds, and the rules are simple:

- **Gameplay only.** No long logo, no lore text crawl.
- **Show the hook in the first five seconds** - an action command landing a Perfect.
- Cut fast. Nothing on screen for more than 3 seconds.
- Include one dialogue moment for personality, one boss moment for stakes.
- Music matters enormously; use a track from the game.
- End with the title, the platforms, and where to get it.

OBS records fine on Linux and under WSLg. Capture at your window's native scale, not a stretched size, so
the pixel art stays crisp.

---

## 8. Credits and licences

A legal and ethical requirement, not a nicety.

- Maintain `assets/CREDITS.md` from Day 151 onward, updated the moment you add any third-party asset.
- For each: the file, the source, the author, the licence, and the required attribution text.
- CC0 needs no attribution but credit anyway; CC-BY requires it.
- Credit your tools too (raylib, Tiled, nlohmann/json, LMMS, whatever you used). It is good practice and
  the raylib community appreciates it.
- Include the credits **in the game** as a screen, and on the store page.
- If you cannot establish an asset's licence, do not ship it.

---

## 9. Release QA

Play the **shipped package**, not a development build, from title to credits, in one sitting (Day 169).
Every bug you find is one a player would have found.

Work through a written checklist and tick every line:

- [ ] New game from a completely clean state (no config, no saves)
- [ ] Every menu and every settings option
- [ ] Key rebinding, including a deliberately absurd binding set
- [ ] Every chapter and every boss
- [ ] Save and load at ten different points
- [ ] Autosave, and loading an autosave
- [ ] Game over and continue
- [ ] Credits, then quitting cleanly
- [ ] Corrupted save handling
- [ ] Missing asset handling
- [ ] No audio device
- [ ] No controller, and controller unplugged mid-battle
- [ ] Alt-tab, minimise, resize, fullscreen toggle
- [ ] Version number, credits, and licences correct
- [ ] Running from a different working directory
- [ ] Package on a clean machine

Then rebuild the final packages, re-verify the downloads from a logged-out browser, and back up
everything (repository, assets, builds) in two places.

---

## 10. After release

- **Support 1.0.1 for two weeks.** Fix what real players report. Shipping a patch based on real bug
  reports is a distinct skill from building the game, and it is a large part of professional work.
- **Provide a way to report bugs**: itch comments, a form, or an email in the README. Ask for the version
  number and the log file.
- **Post it somewhere**: r/raylib, the raylib Discord, r/gamedev's feedback threads. Do not expect an
  audience; expect the experience of releasing.
- **Write a devlog.** It clarifies your own learning and it is what a future employer or collaborator
  will actually read. A post titled "I learned C++ by building a JRPG in 170 days" with your journal
  excerpts is genuinely compelling.
- **Write the retrospective** (Day 170). Take real time over it.
- **Do not immediately start something bigger.** Read your own code first, and rewrite the system you are
  least proud of over a weekend. The gap between the old and new version is the real measure of what the
  170 days did.

---

## Exercises

1. Run your game from `/tmp` by absolute path and confirm every asset loads.
2. Grep your source for absolute paths and `/home/`.
3. Make your install directory read-only and confirm saving still works.
4. Build a Linux package and run it in a clean container.
5. Cross-compile for Windows and test the `.exe` on real Windows.
6. Write your one-line hook, then read it to someone and see whether they understand the game.
7. Complete the full QA checklist on the shipped package.

## Done criteria

- [ ] Release builds have no debug keys, no sanitizers, and a visible version number
- [ ] Assets load regardless of the working directory, with no absolute paths
- [ ] Saves and config live in a per-user directory and survive a read-only install
- [ ] Packages run on clean Linux and Windows machines
- [ ] `assets/CREDITS.md` is complete and every licence is honoured
- [ ] A store page is live with screenshots and a trailer
- [ ] The full QA checklist is complete on the shipped package
- [ ] It is released, tagged in git, and someone else has played it

## Sources

- [itch.io developer docs](https://itch.io/docs/creators/) - short and practical
- [How to make a game trailer](https://www.youtube.com/results?search_query=indie+game+trailer+tips) -
  M. Joshua's and Derek Lieu's advice is the standard reference
- Search "cmake mingw-w64 cross compile windows toolchain file" for the Windows build
- [AppImage packaging guide](https://docs.appimage.org/) if you want a single-file Linux build
- [Choose a License](https://choosealicense.com/) if you decide to open-source the code
