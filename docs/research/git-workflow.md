# Git for a solo project

**Used on:** Day 3, then every single day.

Why this matters: git is what makes experimentation free. A beginner without version control
codes cautiously, because breaking working code is expensive. A beginner with version control
can try anything, because reverting costs one command. That difference in confidence changes how
fast you learn.

You do not need most of git. You need about ten commands, used consistently.

---

## 1. The mental model

Three places a change can be:

```
working directory  ->  staging area (index)  ->  repository (commits)
     (edit)                (git add)                (git commit)
```

A commit is a **snapshot** of the whole project, not a diff, with a pointer to its parent. That
is why git can restore any past state so cheaply.

`HEAD` is where you are. A branch is a movable label pointing at a commit. That is genuinely
most of the model.

**Questions to answer**

- What is the difference between unstaged, staged, and committed?
- Why can you commit some changes to a file and not others?
- What does `git status` tell you about each of the three areas?

---

## 2. The daily loop

This is 95% of your git usage:

```bash
git status                 # what changed
git diff                   # what changed, in detail
git add -A                 # stage everything
git commit -m "message"    # snapshot it
git log --oneline -10      # recent history
```

`git add -p` stages changes interactively, hunk by hunk. It is worth learning early because it
forces you to read your own diff before committing, which catches a surprising number of
mistakes and stray debug prints.

**The roadmap's rule: commit every day, even broken days.** Your commit history becomes a record
of learning, and it is genuinely satisfying to scroll back through in month six.

Message format for this project:

```
day 47: tile collision - learned why axes are resolved separately
```

Present tense, what changed, and (this is the useful part) what you learned.

---

## 3. Undoing things

This is the section that makes you brave. Learn these five and you can experiment freely.

| I want to | Command |
| --- | --- |
| Throw away changes to one file | `git restore <file>` |
| Unstage a file but keep the changes | `git restore --staged <file>` |
| Fix the last commit message | `git commit --amend` (only if not pushed) |
| Undo a commit, keeping history honest | `git revert <hash>` |
| See a past version of a file | `git show <hash>:<path>` |

`git reset --hard` deletes uncommitted work permanently. It has its uses, but treat it with
respect: there is no undo for uncommitted changes.

`git stash` parks uncommitted work temporarily (`git stash`, then `git stash pop`). Useful when
you need a clean tree to test something.

**Questions to answer**

- What is the difference between `revert` and `reset`?
- What is recoverable after a `reset --hard`, and what is gone forever?
- How do you recover a commit you think you lost? (Search `git reflog` - it saves people
  regularly.)

---

## 4. Branches, and when a solo developer needs them

Most days you work on `main` and that is fine. Branch when you are about to do something risky
or exploratory:

```bash
git switch -c experiment/ecs-rewrite
# ... try the risky thing ...
git switch main            # abandon it, or
git merge experiment/ecs-rewrite   # keep it
```

Good candidates for a branch in this roadmap: the Day 48 entity model decision, the Day 106 UI
framework refactor, and any "I wonder if I should rewrite X" impulse. Branching turns a scary
rewrite into a free experiment.

Tag releases: `git tag -a v1.0.0 -m "Tales of Legend 1.0.0"`.

---

## 5. What not to commit

Write `.gitignore` on Day 3 and extend it as you go:

```gitignore
build*/
.cache/
compile_commands.json
*.o
*.a
.vscode/
.idea/
saves/
*.log
```

Rules of thumb: never commit build output, never commit anything generated, never commit
secrets, and **do** commit your assets (they are part of the game) and your data files.

If a large binary sneaks in, it stays in history forever and bloats every future clone.
Check `git status` before `git add -A` for exactly this reason.

---

## 6. A remote backup

Local commits do not protect you from a dead disk. Create a private GitHub repository on Day 3
and push daily:

```bash
git remote add origin git@github.com:you/tales-of-legend.git
git push -u origin main
```

Eight months of work living on one machine is an unnecessary risk. Push at the end of every
session; it costs three seconds.

Consider making it public once you ship - a repository with 170 daily commits telling a learning
story is a genuinely impressive thing to show someone.

---

## Exercises

1. Make a change, stage part of it with `git add -p`, and commit only that part.
2. Break a file badly, then restore it.
3. Commit a mistake, then revert it. Look at the history afterwards and note that both commits
   are present.
4. Create a branch, make three commits, then abandon it and confirm `main` is untouched.
5. Find the content of a file as it was five commits ago, without changing your working tree.
6. Use `git log -p <file>` to read the entire history of one file.

## Done criteria

- [ ] I commit at least once a day with a meaningful message
- [ ] I can undo any mistake without panic
- [ ] I use a branch when I try something risky
- [ ] My repository is pushed to a remote
- [ ] `git status` is clean of build artefacts

## Sources

- [Pro Git](https://git-scm.com/book/en/v2) - chapters 2 and 3 are all you need
- [Oh Sh*t, Git!?!](https://ohshitgit.com/) - the practical recovery cheat sheet
- [Learn Git Branching](https://learngitbranching.js.org/) - visual, interactive, 20 minutes
  well spent
