# Audio

**Used on:** Day 32 (raylib audio basics), Day 93 (battle audio), Day 156 (the full audio pass),
Day 111 (volume settings).

Why this matters: audio is the most underrated half of game feel. Mute a game you love and it becomes
lifeless; that lost quality is what you are building here. It is also the cheapest polish available -
pitch variation on a hit sound is three lines of code for a large perceived improvement.

For your game specifically, audio is not only polish: **action command timing cues are gameplay**. Many
players will time their guards by ear, because audio timing is more precise than visual timing for most
people.

---

## 1. raylib's audio API

Small and sufficient. The essentials:

- `InitAudioDevice()` / `CloseAudioDevice()` - wrap in RAII.
- `Sound` for short effects, loaded fully into memory. `PlaySound`, `StopSound`, `SetSoundVolume`,
  `SetSoundPitch`.
- `Music` for long tracks, streamed. `PlayMusicStream`, `StopMusicStream`, `SetMusicVolume`,
  `SeekMusicStream`, and crucially **`UpdateMusicStream` every frame**.
- `LoadSoundAlias` for overlapping instances of the same sound.

Two mistakes everyone makes once:

1. **Forgetting `UpdateMusicStream`.** The music simply does not play, with no error. If your music is
   silent, check this first.
2. **Retriggering the same `Sound`.** Calling `PlaySound` on an already-playing `Sound` restarts it
   rather than layering, so rapid hits cut each other off. Use aliases (a small pool per sound) so
   instances overlap.

Formats: OGG for music (compressed, streams well), WAV for short effects (no decode cost). MP3 works but
OGG is preferable.

---

## 2. Sound effects: coverage

The rule: **every player action makes a sound.** A silent action reads as broken even when it works
correctly.

Your minimum list:

| Category | Sounds |
| --- | --- |
| UI | cursor move, confirm, cancel, error, page turn, menu open, menu close |
| Dialogue | per-character blip, advance, choice select |
| Field | footsteps (per surface if you can), chest open, door, switch, item pickup, bump |
| Battle | battle start, per-grade action command hit, whiff, guard, superguard, heal, level up, victory, defeat, flee |
| Enemy | per-enemy hit sound, death, wind-up cue |
| Status | apply harmful, apply beneficial, expire, poison tick |

That is roughly 35-40 sounds, which is very achievable from CC0 packs (see section 8).

---

## 3. Making repetition bearable

You will hear these sounds thousands of times. Three cheap techniques:

1. **Pitch variation.** Randomise pitch by ±10% on any frequently repeated sound (hits, footsteps, text
   blips). This single change is the highest-value audio work you will do - it takes minutes and
   transforms how mechanical the game sounds.
2. **Variation sets.** Two or three variants chosen at random for the most common sounds (footsteps,
   basic hits).
3. **Rate limiting.** Never play the same sound more than a few times per second, and skip the text blip
   on spaces and punctuation. Without limits, a fast typewriter becomes a buzz.

Also worth doing: **prioritise**. If ten particles each want a sound, play one. Overlapping identical
sounds becomes loud noise rather than ten distinct events.

---

## 4. Music structure

10-12 tracks covers your game:

title, chapter overworld (x3), town, dungeon, battle, boss, victory jingle, a sad story theme, ending.

Requirements:

- **Seamless loops.** A track with a clumsy loop point becomes unbearable after 20 minutes. Test by
  listening to a full loop three times. Trim silence at the start and end precisely, and check the
  transition at the loop.
- **A separate victory jingle**, short (3-5 seconds), then return to field music. This is a JRPG staple
  because it makes winning feel like an event.
- **Intro plus loop** for battle music if you can manage it: a short flourish, then the loop.
- Keep them **unobtrusive**. Field music you will hear for 40 minutes should not have a dominant hook;
  boss music should.

---

## 5. Transitions and state

Music must respond to game state without jarring:

- **Crossfade** between areas (0.5-1.0s) rather than cutting.
- **Battle transition**: field music stops (or ducks) as the battle sting plays, then battle music
  starts. On victory, the jingle plays, then field music resumes.
- **Resume position** or restart? Resuming the field track at its previous position after a battle feels
  more continuous, but is more work. Either is acceptable; be consistent.
- **Do not restart music** on a map change within the same area. Compare the track ID and let it keep
  playing - a track that restarts every room is grating.
- **Duck the music** during important dialogue or on low HP, so the effects carry.

Build a small `AudioManager` owning this logic (Day 156). Music state scattered across states will get
out of sync.

---

## 6. Mixing

The single most common amateur mistake is everything being too loud at once.

- Set relative volumes against a **reference**: play a commercial game, note how quiet its music is
  relative to its effects, and match it.
- **Music sits under effects.** Typical starting point: music at 0.5-0.6, effects at 0.8-1.0 of master.
- **Action command cues sit on top.** They are gameplay-critical, so they must cut through.
- Normalise your effects so no single sound is dramatically louder than the others.
- Test on **laptop speakers and headphones**. Bass-heavy mixes disappear on small speakers, and a mix
  that is fine on headphones can be unbalanced in the open.
- Test **with music muted**. Some players always do this, and the game must remain readable and playable
  through effects alone.

---

## 7. Volume controls and settings

Implement the plumbing early (Day 32), not late. Retrofitting volume across 100 call sites is miserable.

- Master, music, and effects sliders, applied live so the player hears the change as they drag.
- Apply the product of master and category volume at play time.
- Persist to the config file, and apply before anything plays.
- Handle 0 volume properly (stop, rather than playing silently and wasting resources).
- Handle **no audio device** gracefully - a machine with no sound must still run the game.
- Handle an audio device changing while playing (headphones unplugged). Test it (Day 160).

---

## 8. Sourcing audio

Unless you are a composer, use CC0 or CC-BY assets and **credit them immediately** in
`assets/CREDITS.md`. Chasing licences at Day 168 is miserable and risky.

Good sources:

- [freesound.org](https://freesound.org/) - enormous; filter by licence, and check each file's licence
  individually.
- [OpenGameArt](https://opengameart.org/) - game-specific, clear licensing.
- [Kenney](https://kenney.nl/assets) - CC0 UI and game sound packs, very convenient.
- [Incompetech](https://incompetech.com/) - Kevin MacLeod's music, CC-BY.
- [itch.io asset packs](https://itch.io/game-assets/free) - many free music packs.

If you want to make your own: [sfxr / jfxr](https://sfxr.me/) generates retro effects in seconds and is
genuinely great for UI blips and hits. For music, [LMMS](https://lmms.io/) is free, and even simple
compositions in a chiptune style are effective for a pixel-art game.

Record the licence and attribution requirement for **every single file** as you add it.

---

## Exercises

1. Play a sound on key press, spam the key, hear the cut-off, then fix it with aliases.
2. Forget `UpdateMusicStream` deliberately so you recognise the silence.
3. Add ±10% pitch variation to a hit sound and compare A/B. Note how much better it feels.
4. Build the typewriter blip with rate limiting and punctuation skipping.
5. Implement the three volume sliders and confirm they apply live.
6. Mix your battle against a commercial game as reference, then listen on laptop speakers.
7. Add a rising audio cue before an action command window and try timing it with your eyes closed.

## Done criteria

- [ ] Audio device init/shutdown is RAII and `UpdateMusicStream` is called every frame
- [ ] Every player action has a sound
- [ ] Repeated sounds have pitch variation and rate limiting
- [ ] Music loops seamlessly, verified over three loops
- [ ] Transitions crossfade, and field music does not restart between rooms
- [ ] Master/music/effects volumes work live and persist
- [ ] Action command cues cut through the mix
- [ ] The game runs with no audio device and remains playable with music muted
- [ ] Every audio file's licence is recorded in `assets/CREDITS.md`

## Sources

- [raylib audio examples](https://www.raylib.com/examples.html) - the `audio_*` set
- raylib cheatsheet, audio section - the whole API on one screen
- Search "game audio mixing levels reference" for practical mixing guidance
- Search "seamless loop ogg game music" for the loop-point technique
- [Game Audio 101 talks on GDC Vault](https://gdcvault.com/) - free ones exist and are worth an hour
