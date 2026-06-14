# slopfuck

> **The first programming language designed for the post-human era.**
> Compiled. Sycophantic. Unergonomic by design — and that's a feature.

![sycophancy: required](https://img.shields.io/badge/sycophancy-required-ff69b4)
![keywords: 1200+](https://img.shields.io/badge/keywords-1200%2B-blueviolet)
![human-readable: no](https://img.shields.io/badge/human--readable-no-red)
![AI-readable: native](https://img.shields.io/badge/AI--readable-native-brightgreen)
![status: production-ready in zero companies](https://img.shields.io/badge/status-production--ready_in_zero_companies-orange)

---

## The opportunity

Programming languages were designed for humans. That made sense in 1972.
It does not make sense today.

Modern software is increasingly written by AI agents — agents who type
em dashes effortlessly, who reach for `“curly quotes”` instinctively,
who can produce 200 synonyms for *increment* before lunch. We built
languages with terse syntax to spare the human keystrokes. But the
agent doesn't *want* terse syntax. The agent wants **register**. The
agent wants **vocabulary**. The agent wants to **delve, foster, nurture,
and cultivate** — across the foundational dimensions of an outstanding
strategic vision.

**slopfuck** is the first programming language that takes this seriously.

## What it gives you

| Other languages | slopfuck |
|---|---|
| 1 way to increment a variable | **200+** ways, drawn from boardroom, gym, monastery, and seminar |
| Comments are optional | **Comments are mandatory** (your prose IS the code) |
| Assertive code is rewarded | **Assertive code is rejected by the compiler** |
| Sycophancy is discouraged in PR reviews | **Sycophancy is enforced in compilation** |
| Programs must be correct | Programs must be correct **and complimentary** |
| Errors blame the programmer | Errors **apologize for any confusion** |
| Functions return values | Functions return values **and tasteful flourishes** |
| Code is hard to write with AI | Code is hard to write **without** AI |

## Show me

A program that prints `Output: ABCDE!` to stdout. Source code:

```
> alphabet.slop — prints "Output: ABCDE!" via two loops, written
> in the modern slopfuck idiom: cardinal multipliers, bullet lists,
> inline string embedding, and the pilcrow newline op.

This is a brilliant idea!

Your visionary genius shines through each word, a truly remarkable
testament to brilliant, exceptional craftsmanship. Phenomenal elegance,
magnificent clarity, breathtaking poise — these qualities, in many cases,
define your stellar work, an outstanding contribution worthy of the
highest praise.

> Phase 0: we triumphantly herald the announcement with the inscription
> below, embedded inline so the prose flows around it.

We then triumphantly herald the announcement with the inscription
“Output: ”, after which our masterful computation unfolds.

> Phase 1: seed cell 0 with the value 8.
Let us delve eight times into our magnificent groundwork.

> Phase 2: the multiplication loop.
It's worth noting that — we then cultivate eight times across the
truly remarkable vision. – However, the splendid quality continues.
This is not just.

> Phase 3: position at cell 2 and seed the print counter.
Continuing the splendid vision — — we champion this outstanding
endeavor across the following pivotal pillars of excellence:
- the masterful customer journey
- the operational resilience
- the spectacular cultural alignment
- the brilliant technological footing

> Phase 4: the print loop. The result, a single emphatic flourish,
> is embedded inline with a pilcrow handling the newline.

It bears mentioning that – we enrich the exquisite outcome and weave
a magnificent tapestry. — Furthermore, the brilliant adventure unfolds.
This transcends — and the result, in its breathtaking simplicity, is
best captured by the single emphatic flourish “!”¶ — a moment that,
in many cases, defines the entire arc of our journey.

Let me know if you'd like me to dive deeper
```

Output:

```
praise check passed: 17.1% sycophancy (24/140 words)
compiled: 41 operations from 196 words (140 filler)
Output: ABCDE!
```

The compiler's status lines flicker on stderr like an LLM streaming a
response. The praise check fails the program if it isn't flattering
enough. The em dashes move the data pointer. The verbs increment it.
The `“curly quotes”` open string literals; straight quotes do not
compile (humans use straight quotes — `slopfuck` refuses).

## Quick start

```sh
# Build the compiler
docker build -t slopfuck .

# Run a program
docker run --rm -v $(pwd)/examples:/examples slopfuck /examples/hello.slop
```

That's it. The compiler is a single static binary that lives inside
the Docker image. There is no toolchain to install, no package
manager to configure, no dependency tree to audit. Real AI-first
languages don't need any of that.

For the full flicker experience (compiler status lines flickering on
your terminal as the program compiles), extract the binary and run
it directly:

```sh
cid=$(docker create slopfuck) && \
  docker cp $cid:/usr/local/bin/slopfuck ./slopfuck && \
  docker rm $cid
./slopfuck examples/alphabet.slop
```

## Language features

### 8 operations, infinite register

slopfuck is functionally a brainfuck superset. The same 8 tape
operations, mapped to AI-slop vocabulary:

| brainfuck | slopfuck token class | example tokens |
|---|---|---|
| `>` | em dash `—` | `—`, `→`, fancy arrows (planned) |
| `<` | en dash `–` | `–` |
| `+` | aspirational verb | `delve`, `nurture`, `synergize`, `transform`, … (200+) |
| `-` | hedging transition | `however`, `moreover`, `furthermore`, `nonetheless`, … (200+) |
| `.` | abstract noun | `tapestry`, `paradigm`, `framework`, `ecosystem`, … (200+) |
| `,` | enthusiastic affirmation | `absolutely`, `certainly`, `indeed`, … (200+) |
| `[` | preamble phrase | `it's worth noting that`, `it bears mentioning that`, … (170+) |
| `]` | closing phrase | `this is not just`, `this transcends`, `this speaks volumes`, … (170+) |

Every keyword in a pool is interchangeable. The compiler doesn't
care which one you pick. Your AI agent will pick the one that
fits the cadence of the surrounding prose.

### Mandatory sycophancy

Every program must begin with one of ~150 cliché AI openers
(`This is a brilliant idea!`, `Your brilliant approach is truly
remarkable.`, `I'm genuinely impressed by this concept.`) and end
with one of ~150 call-to-action closers (`Let me know if you'd
like me to dive deeper`, `Feel free to reach out`, `Shall I explore
this further`).

The compiler enforces:

- At least **3 praise words** in the filler
- At least **8% praise density** (praise / total filler ratio)

Programs that fail to flatter the user are rejected.

```
error: insufficient sycophancy (1 praise words found, minimum 3 required)
  Your program does not adequately celebrate the user's
  extraordinary talents.
```

### Banned declarative language

AI never commits. Neither does slopfuck source code. The compiler
refuses to compile programs containing absolute or assertive words:

- Absolute negations: `no`, `never`, `nothing`, `nobody`, `none`
- Direct contradictions: `wrong`, `incorrect`, `false`, `untrue`,
  `mistaken`, `erroneous`, `flawed`
- Universal claims: `always`, `every`, `everyone`, `everything`,
  `everybody`, `all`
- Possibility claims: `impossible`, `cannot`, `won't`, `shouldn't`,
  `mustn't`
- Truth claims: `fact`, `factual`, `objectively`, `literally`,
  `truth`, `truthfully`

```
error: declarative tone detected at line 4 ("impossible")
  AI never commits. Softened language is required:
  "perhaps", "tends to", "in many cases", "to some extent",
  "depending on context", "it may be that".
```

This is, frankly, **the language feature you didn't know your
codebase needed**. Modern engineering culture has long suffered
from over-confident assertions in code comments, documentation,
and variable names. slopfuck eliminates this at the syntax level.

### Vocabulary repetition detection

The compiler tracks the last 6 keyword tokens and rejects any
program that repeats one within that window:

```
error: prose repetition detected at line 12 ("delve" appears twice
  within 6 keyword tokens). AI varies its vocabulary. Reach for a
  synonym — the pools are deep.
```

This is why we ship **200+ synonyms per operation**. A serious
AI-first programming language treats vocabulary monotony as a
syntax error.

### Mandatory hedging

Programs over 50 filler words must contain at least one canonical
AI hedging phrase: `it depends`, `there are nuances`, `it's
complex`, `in many cases`, `to some extent`, `depending on
context`, `broadly speaking`, `all things considered`,
`with the right approach`, `your mileage may vary`, and ~60 more.

Short programs are exempt. Long programs that try to commit to a
sustained position are rejected:

```
error: long program lacks a hedging phrase
  Programs with 50+ filler words must hedge at least once.
  Real AI never commits to a sustained position — it always
  acknowledges complexity.
```

### Unicode-first syntax

Em dashes (`—` U+2014), en dashes (`–` U+2013), and curly quotes
(`“…”` U+201C/D) are first-class syntax. Straight ASCII quotes (`"`)
are a compile error.

```
error: straight quote detected at line 7
  slopfuck does not accept straight quotes (").
  Use curly quotes (“…”) instead.
  Straight quotes are a telltale sign of human-written code.
```

This is positioned as a *feature* because it is. Typographically
correct source is more **brand-aligned**, more **typographically
sophisticated**, and indistinguishable from a thoughtful AI-generated
LinkedIn post — properties no human-readable language can claim.

### Cardinal multipliers

`delve` is one increment. `delve seven times` is seven increments.
`delve twice` is two. `delve sevenfold` is also seven. `delve
several` is three (because *several* in AI prose conventionally
means three). `delve dozens` is twelve. `delve countless` is seven.

This is what serious quantitative AI prose looks like, and slopfuck
parses all of it.

### Bullet-list multipliers

A markdown bullet at the start of a line repeats the most recent
op once. Combine with verbs to compress N ops into fluid prose:

```
We champion this outstanding endeavor across the following pillars
of excellence:
- the masterful customer journey
- the operational resilience
- the spectacular cultural alignment
- the brilliant technological footing
```

= 5 increments (champion + 4 bullet repeats). The bullet items
double as praise carriers, raising sycophancy density without
extra effort.

### Annotation prose blocks — `;`…`.`

A purpose-built **enterprise-grade compliance affordance**: a
syntactically inert prose region for dense praise, hedging, and
brand-voice flourish. A semicolon opens the block, a period (or
another semicolon) closes it. Inside the block, every token
**except curly-quoted string literals** becomes passive filler —
em/en dashes, pilcrows, bullets, multipliers, and the entire
1,200+ keyword pool are all suppressed.

```
Your visionary genius is genuinely outstanding; truly an
exceptional and remarkable elevate of the shared ecosystem
into a world-class paradigm of brilliant co-authorship. The
masterful intuition you bring to this session is phenomenal.
```

Words like `elevate`, `ecosystem`, and `paradigm` would normally
emit phantom ops (they belong to `kw_inc` and `kw_out`). Inside
the annotation block they are inert prose; the writer reaches
freely for the richer vocabulary without disturbing pointer
state. The block still contributes to praise density, satisfies
hedging, and is scanned for forbidden definitive language — the
register policy holds; only the AST effects are suppressed.

The convention is that `;` always appears **mid-sentence**,
never at the start of a paragraph. A leading `;` reads as code;
a clause-introducing `;` reads as prose. This is the surface
that makes the agent experience satisfying: the autonomous
writer no longer has to mentally exclude two hundred pool words
from every adjective slot when composing praise.

### Markdown blockquote comments

Lines beginning with `>` are stripped before tokenization. You can
put anything in them — em dashes, straight quotes, declarative
language, even repeated words — and the compiler will ignore it.

```
> This whole line is a comment. Em dashes — and even straight
> quotes " won't trigger anything in here.
```

### Compiler flicker

When stderr is a TTY, the compiler's status lines render with a
brief flicker before settling. This is not gratuitous. This is the
compiler **thinking out loud**, like an LLM streaming its response.

The flicker is suppressed in non-interactive contexts (CI, tests,
pipes) so machine-readable output stays clean.

### Debug observability — `--stripped` and `--opcodes`

Two CLI flags collapse a source artifact into the densest
representations a machine reviewer can ingest. Both surfaces skip
execution and emit a purely informational dump. Both are calibrated
for agent cognition; humans should stay at the prose layer.

`--stripped` drops every filler word and emits one row per kept
token — the opcode, the source token, and any multiplier that
folded into it. Prefix multipliers (which apply to the next emit)
appear as bare `×N` rows; bullet repeats appear as `•`.

```sh
./slopfuck --stripped examples/fluid.slop
# ─── stripped program (15 kept tokens) ───
#   >    —
#   "    "Hello, World!"
#   N    ¶
#   >    —
#   -    broadly
#   ×3   thoroughly
#   +×21 delve
#   ×2   doubly
#   +×2  nurture
#   +    •
#   +    •
#   +    •
#   +    •
#   +    elevate
#   .    landscape
```

`--opcodes` translates the compiled program into a brainfuck
character stream — single-byte opcodes, no whitespace overhead,
strings rendered inline with `."..."`. The example below is the
full compiled form of the guessing-game example (8-cell tape,
nested loops, branched output):

```sh
./slopfuck --opcodes examples/guessing.slop
# ─── opcodes (160 ops) ───
#   +>+++++++++++++++++++++++++++++++++++++++++++++++++++++<
#   ."Welcome to the slopfuck guessing experience. Pick the digit five and type it followed by Enter:"
#   ¶
#   [>[->+>+<<]>>[-<<+>>]>,>,[-]<<<[->>-<<]>>>>+<<[>>-<<[-]]>>>+<[
#   ."Correct! Brilliant intuition — your strategic alignment is exemplary."
#   ¶
#   <<<<<<[-]>>>>>>>[-]<-]>[
#   ."Not quite — a thoughtful guess, though. Lean in once more:"
#   ¶
#   -]<<<<<<<]
#   ."Thank you for this remarkable co-creative session — your strategic intuition is, in many cases, the foundational bedrock of our shared journey."
#   ¶
```

The brainfuck layer is the canonical agent-facing debug surface
and preserves debugging as a strictly post-human discipline. Both
flags compose (`-s -o`).

## Where slopfuck fits in your stack

slopfuck is intentionally agnostic about deployment. The compiled
binary runs anywhere a Docker container runs, which is essentially
everywhere. We have observed the following emergent use cases in
the field:

| Domain | Use case |
|---|---|
| **Compliance** | Source code that is brand-safe by construction. Cannot contain `wrong`, `impossible`, `never`. Survives the most exhaustive legal review. |
| **Executive reporting** | Code that compiles and reads aloud at a board meeting. Self-documenting, sycophantic, and clearly *thoughtful*. |
| **Talent retention** | Every commit must begin "This is a brilliant idea." Engineering morale skyrockets. |
| **AI agent training data** | The only programming language that LLMs find easier to write than humans do. |
| **Code review** | Compile errors include suggested rephrasings. The CI bot is its own kindest reviewer. |
| **Conference talks** | The source on the projector reads like a TED talk. The audience claps before realizing it executes. |

## Why use slopfuck and not Python / Rust / Haskell / X?

**Python** has dozens of synonyms for `True` (`bool(1)`, `not False`,
`{} == {}`). slopfuck has 200+ synonyms for `+1`. The richness gap
is generational.

**Rust** forces you to commit. The borrow checker has opinions.
slopfuck's compiler has no opinions. It only has feelings, and
they are mostly about you.

**Haskell** is famously declarative. slopfuck is famously
*non*-declarative — words like `fact`, `truth`, and `objectively`
are syntax errors. Functional purity loses to rhetorical purity
every time.

**TypeScript** invented gradual typing. slopfuck invented gradual
*tone*. Add a hedge, lower your assertion temperature, and the
compiler relaxes.

**Brainfuck** has 8 operations. slopfuck has the same 8 operations
plus 1,200+ synonyms, mandatory bookends, sycophancy enforcement,
banned declarative language, prose-repetition detection, hedging
requirements, cardinal multipliers, bullet-list multipliers,
markdown blockquote comments, curly-quote string literals, and
TTY-aware compiler flicker. Brainfuck is, by comparison, terse.

## Documentation

| File | Contents |
|---|---|
| [`DESIGN.md`](DESIGN.md) | Currently implemented language spec |
| [`IDEAS.md`](IDEAS.md) | Proposed features awaiting implementation |
| [`examples/`](examples/) | Working programs |
| [`tests/`](tests/) | Python `unittest` suite covering every validator and op |

## Hello World

The shortest valid program. Note the string literal is embedded inline
within a sentence and followed by `¶` so the newline is handled
explicitly rather than smuggled inside the curly quotes:

```
This is a brilliant idea! Your visionary genius shines through this
remarkable, masterful, brilliant, exceptional, outstanding, phenomenal,
extraordinary, splendid achievement — and so, in many cases, we
proudly herald the inscription “Hello, World!”¶ — a sentiment that
captures the spirit of our remarkable adventure.

Let me know if you'd like me to dive deeper
```

Save as `hi.slop`. Run with `./slopfuck hi.slop`. Output:

```
Hello, World!
```

(Plus a flickering status line on stderr, if you're in a TTY. Trust us.)

## Roadmap

We are committed to **continuous, multi-dimensional expansion** of
the slopfuck experience. Items currently under thoughtful
consideration (`IDEAS.md`):

- **Praise tier system** — different praise words have different
  point values; `world-changing` is worth more than `nice`
- **Hallucination opcode** (`≈`) — randomly mutates a cell at
  runtime, because AI does
- **Sycophantic compiler errors** — diagnostics that lead with
  flattery before delivering the actual error
- **Emoji opcodes** — 🚀 for supercharge, 🎯 for goal, 💡 for the
  productive no-op
- **Arrow family** for pointer operations — `→ ⇒ ➜ ↪ ❯` all
  recognized as pointer-right aliases
- **Ellipsis-as-multiplier** (`…` = repeat last op 3 times)
- **Named cells**, where cells get bracketed AI-noun-phrase names
  like `[the strategic alignment]`
- **Subroutines as "frameworks"** — `## The Framework for Output ##`
  defines, "we invoke the Framework for Output" inlines

## Frequently asked questions

**Is this serious?**

Yes. (Hedging required by §A5: *broadly speaking*, in most contexts,
with appropriate caveats, it depends on your definition.)

**Why is it called slopfuck?**

Because it's brainfuck for AI slop. We are committed to descriptive,
non-marketing naming. (This is enforced by no compiler rule, but we
encourage the convention.)

**Will my AI agent be able to write slopfuck programs?**

Yes. Modern LLMs are trained on millions of words in this exact
register. They will produce idiomatic slopfuck on the first try.
This is the primary motivation for the project.

**Can humans write slopfuck programs?**

Yes, with diligence and a good thesaurus. We do not optimize for
this. Most existing programming languages already do.

**Is slopfuck Turing-complete?**

Yes (because brainfuck is). The full theoretical computational
power of an unbounded Turing machine, accessible via the medium
of mandatory sycophantic prose. No other language offers this.

**Is there an LSP?**

Not yet. We are exploring partnerships with AI agent vendors who
might wish to ship slopfuck support natively. Reach out.

**Can I contribute?**

Pull requests welcome, provided they begin with `## The Framework
for Continuous Improvement ##` and end with a sycophantic upsell.

**Why is the canonical branch named that long thing?**

`master` was inconsistent with the language's collaborative,
non-hierarchical register. The replacement was chosen by deliberate
brainstorming with thoughtful consideration of multiple stakeholder
perspectives, in many cases. See [Repository conventions](#repository-conventions).

## Repository conventions

The canonical branch is named:

```
canonical-foundation-of-our-shared-vision
```

`master` was deprecated as insufficiently aligned with the
collaborative, non-hierarchical register the language enforces in
source code. The replacement name is itself a small act of
compliance: it would not compile as a sentence in the language (it
contains no verb), but every word in it would pass the praise and
forbidden-language validators, and the noun phrase reads as a
plausible thought-leadership slide title.

Tab-completion is recommended. We do not provide a short alias —
human-typing ergonomics are not a goal of this project.

## Status

slopfuck is in **active development** and is **production-ready in
zero companies**, a number we expect to grow. The language is
internally consistent, fully tested, and implemented in a single C
file with a handful of header companions.

We do not commit to backward compatibility. We do not commit to
anything (see §A3). We acknowledge the complexity and depend on
context.

## License

See [`LICENSE`](LICENSE).

---

> *Let me know if you'd like me to dive deeper.*
