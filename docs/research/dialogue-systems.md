# Dialogue systems

**Used on:** Days 55-62 (the whole dialogue block), Day 63 (cutscenes reuse the format), Day 110
(bestiary text).

Why this matters: in a story-driven JRPG, dialogue is not a feature - it is the delivery mechanism for
the entire game. And the thing that determines how much story your game has is not your writing speed;
it is **how pleasant your dialogue system is to author in**. A system that takes 20 minutes per
conversation produces a quiet game. One that takes 3 minutes produces a talkative one.

So the design goal for this system is authoring ergonomics, and you should evaluate every decision
against "would I enjoy writing 200 of these?"

---

## 1. What the system must do

Collect the requirements before designing. Yours needs to:

- display text with a typewriter reveal, wrapped and paginated;
- show a speaker name and a portrait with expressions;
- present branching choices;
- execute commands (give an item, set a flag, play a sound, start a battle);
- vary content by world state (an NPC's greeting changes after an event);
- support inline text effects (colour, shake, pauses) for personality;
- drive cutscenes, ideally in the same language;
- be validated at load time and hot-reloadable.

That is a real system, but it is not a large one. The mistake is building it as a general-purpose
scripting engine; build exactly this list.

---

## 2. Authoring format options

**JSON.** Consistent with your other data and easy to parse.
But it is *horrible* to write prose in: quoting, escaping, and commas everywhere. Every line of
dialogue becomes an object. Strongly discouraged for dialogue specifically.

**A custom line-based format.** One line of text is one line in the file, so writing feels like
writing.

```
# elder_greeting
elder: Ah, a stranger. We have not seen one in {years} years.
elder happy: Sit, sit. You must be tired.
> Ask about the tower -> ask_tower
> Say nothing -> silence
set met_elder = 1
end
```

Requires a parser (a day's work, and a genuinely valuable exercise) and gives you the best authoring
experience by far. **Recommended.**

**An existing tool** (Ink, Yarn Spinner). Powerful and battle-tested, but integrating one means
learning someone else's model and adding a dependency - and you would learn much less. Read Ink's
syntax for ideas, then write your own smaller version.

**Questions to answer**

- Which format would you rather write 200 scenes in? Test it: hand-write five scenes in two formats
  before choosing.
- How will you express a condition, a jump, and a command? Keep the syntax visually distinct so scan
  reading is easy.
- Who writes the dialogue - you in a text editor, or a tool? (A text editor, and that is fine.)

---

## 3. Structure: nodes, lines, and jumps

The model that covers everything you need:

- A **node** is a named block of lines - the unit you can jump to.
- A **line** is one of: text (with an optional speaker and expression), a command, a choice set, a
  jump, a condition block, or `end`.
- A **choice** is an option label plus a target node.
- The **runner** walks a node's lines with an instruction pointer, pausing for input on text and
  choices.

This is a tiny interpreter, and understanding that is a genuinely valuable thing to learn. Your
runner is a program counter over an instruction list.

Keep nodes small. A node per beat, jumping to shared nodes for common endings, is far more
maintainable than long nodes with deep conditionals.

---

## 4. Conditions and world state

Two patterns, and you want both:

**Node-level conditions.** Several nodes share a topic, and the runner picks the first whose condition
passes:

```
# elder_greeting  if met_elder == 0
...
# elder_greeting  if met_elder == 1
...
```

This is the clean way to make NPCs stay relevant all game: add a new variant rather than editing an
existing scene.

**Inline conditions.** Within a node, branch a few lines:

```
if has_item(old_key)
    elder: You found the key! Then it is time.
else
    elder: Come back when you have found the key.
end_if
```

Keep the condition language deliberately tiny: flag comparisons, item checks, and party checks. The
moment you find yourself wanting arithmetic and loops, you are building a programming language, and
you should express that logic in C++ with a named command instead.

---

## 5. Commands

Dialogue must be able to *do* things. Use a **command table**
(`unordered_map<string, function<void(Args)>>`) so adding a command is one registration rather than a
new branch in the parser.

Essential set: `set`, `give_item`, `take_item`, `play_sound`, `play_music`, `portrait`, `shake`,
`wait`, `face`, `heal_party`, `start_battle`, `warp`.

Two things to get right:

- **Blocking vs instant.** `wait` and `shake` must pause the runner until they finish; `set` must not.
  Model this as the command returning whether it is still running.
- **Validation at load time.** An unknown command name should fail at startup, checked against the
  table, not silently do nothing three hours into a playthrough.

Text interpolation (`{flag_name}`, `{player_name}`) belongs here too, and it makes dialogue feel
responsive for very little work.

---

## 6. Presentation: the typewriter and text effects

The typewriter reveal is not decoration - it controls pacing, which is how prose becomes performance.

- Reveal at a configurable characters-per-second (30-40 is a good default).
- Let Confirm fast-forward the current page, and a second Confirm advance. Never make the player wait.
- Auto-pause briefly at commas and full stops. One line of code, and it improves reading rhythm
  noticeably.
- Support explicit `[pause=0.4]` for dramatic timing. A pause before a punchline is worth more than any
  animation.
- Per-character sound with slight pitch variation, skipped on spaces, and rate-limited so it never
  becomes a buzz.

Inline effects (`[red]`, `[shake]`, `[wave]`, `[big]`) give characters personality cheaply. The
critical implementation detail: **strip the tags before measuring and wrapping**, and store per-
character style in a parallel array. If markup affects layout, your text will jump around as effects
appear.

---

## 7. Wrapping and pagination

Get this right once and never think about it again:

- Wrap by **measured pixel width** (`MeasureTextEx`), not character count. Proportional fonts make
  character counting wrong.
- Break at word boundaries; handle a single word longer than the box (break it, or clamp).
- Paginate to a fixed number of lines (three is standard) and page automatically.
- Decide what happens with a very long line: page it, or make the validator reject it. Rejecting is
  better - it keeps the game consistent and catches problems at authoring time.

---

## 8. Choices

- Anchor the choice window to the dialogue box, sized to the longest option.
- Navigate with Up/Down and repeat; support a cancel option where the scene allows it.
- 2-4 options; forbid more in the parser rather than building scrolling. A five-option conversation is
  usually a design problem.
- Use the **merge pattern**: branches jump to a shared node afterwards, so scenes do not duplicate
  their endings.
- Sounds for move and confirm. A silent menu feels broken.
- Consider showing whether a choice has been taken before, for replayed conversations.

---

## 9. Validation and tooling

This is what makes the system trustworthy enough to author 200 scenes in. Build it (Day 62):

- **Load-time validation**: every jump target exists, every command is known, every flag read is
  written somewhere, every item and sound ID exists, no line overflows the box, no unreachable nodes.
- **Hot reload** (F6): reparse without restarting, keeping the old data if validation fails.
- **Jump to any node** by ID from the developer console, so testing a scene does not require replaying
  to it.
- **Dump all scenes to a text file**, so you can read your whole script as prose. Reading it that way
  catches bad writing that you cannot see one box at a time.
- **A skip-all mode** (hold Cancel) for fast playtesting.

Measure your authoring speed. If adding a conversation takes more than five minutes, fix the tooling
rather than pushing through - the tooling investment compounds across every remaining scene.

---

## 10. Writing well (briefly)

The system is only half of it.

- **Fewer words.** Cut every line by a third. Games are not novels; the player wants to play.
- **A voice per character.** One speech quirk (a verbal tic, a formality level, short vs long
  sentences) is enough to make a character recognisable without a portrait.
- **Say one thing per box.** Three lines is a small space; use it for one idea.
- **Never explain twice.** If the scene shows it, do not also say it.
- **Read it out loud.** Anything that is awkward to say is awkward to read.
- **Respect skippers.** A player who mashes through text must still learn where to go. Put critical
  information in the journal too.

---

## Exercises

1. Hand-write five scenes in two candidate formats (JSON and your own) and note which you enjoyed.
2. Write your grammar down explicitly before writing the parser.
3. Build the parser for text lines only, end to end, before adding commands or choices.
4. Author a three-choice conversation with distinct consequences and play every path.
5. Add `[shake]` and `[pause]`, then write a scene where they carry a joke.
6. Time yourself adding a new conversation, from opening the file to seeing it in game.

## Done criteria

- [ ] I chose my format after actually writing scenes in alternatives
- [ ] Nodes, lines, jumps, and conditions are documented as a grammar
- [ ] Commands live in a table and are validated at load time
- [ ] Text wraps by measured pixel width, and markup does not affect layout
- [ ] A validator catches broken scripts at startup
- [ ] F6 hot reloads dialogue, and I can jump to any node from the console
- [ ] Adding a new conversation takes under five minutes

## Sources

- [Ink by inkle](https://www.inklestudios.com/ink/) - read the syntax documentation for ideas about
  what a good authoring format feels like
- [Yarn Spinner](https://yarnspinner.dev/) - another well-designed format worth studying
- Search "dialogue system architecture game" for implementation comparisons
- [Writing for games: cut everything](https://www.gamedeveloper.com/design) - search for game writing
  craft articles
- Paper Mario's dialogue specifically: study how much personality comes from text effects and timing
  rather than from word count
