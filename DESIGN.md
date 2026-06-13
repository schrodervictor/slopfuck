# slopfuck — language design

> The AI-slop programming language. Compiled, surrealist, sycophantic.
>
> This document describes the **currently implemented** language.
> Proposed-but-not-yet-built features live in `IDEAS.md`.

## Philosophy

slopfuck is brainfuck refactored to look like AI-generated LinkedIn prose.
A valid program reads like an executive thought-leadership post; it
compiles to brainfuck-equivalent operations; it runs.

Two principles guide every design decision:

1. **The source code must look like AI slop to a casual reader.**
   Em dashes, curly quotes, vague abstract nouns, hedging transitions,
   sycophantic openings, upsell closings. Real AI slop varies its
   vocabulary — slopfuck must do the same.
2. **The compiler is also AI slop.** Errors are phrased apologetically,
   praise is mandatory, the call-to-action is required. The compiler
   participates in the joke.

---

## Implemented design (v0.1)

### Core semantics

slopfuck programs are functionally equivalent to brainfuck. A program
operates on a tape of 30,000 unsigned byte cells with a single data
pointer. The 8 brainfuck operations are preserved, plus two additions
(string literals and a newline output op) that do not change
computational power:

| brainfuck | slopfuck token class | example |
|---|---|---|
| `>` | em dash `—` (U+2014) | `—` |
| `<` | en dash `–` (U+2013) | `–` |
| `+` | aspirational AI verb | `delve`, `nurture`, `synergize` |
| `-` | hedging transition | `however`, `moreover`, `nonetheless` |
| `.` | abstract AI noun | `tapestry`, `paradigm`, `framework` |
| `,` | enthusiastic affirmation | `absolutely`, `certainly`, `indeed` |
| `[` | preamble phrase | `it's worth noting that` |
| `]` | closing phrase | `this is not just` |
| *(string literal)* | curly-quoted output | `“Hello, World!”` |
| *(newline)* | pilcrow / verbose synonym | `¶`, `thereafter`, `henceforth` |

### Keyword pools

Each word-based operation has a pool of **200+ interchangeable tokens**.
The compiler treats every token in a pool identically; the programmer
rotates among synonyms to produce natural-looking prose. The pools are
defined in `keywords.h`:

- `kw_inc` — increment verbs (delve, foster, cultivate, leverage, …)
- `kw_dec` — decrement transitions (however, moreover, furthermore, …)
- `kw_out` — output nouns (tapestry, paradigm, ecosystem, framework, …)
- `kw_in` — input affirmations (absolutely, certainly, indeed, …)
- `kw_loop_start` — preamble phrases (it's worth noting that, …)
- `kw_loop_end` — closing phrases (this is not just, …)

### Case insensitivity

Keyword matching is **case-insensitive**. `DELVE`, `Delve`, `dElVe` all
match. Inside string literals, the original casing is preserved.

### Filler / comments

Anything that is not a keyword, em dash, en dash, or string literal is
**filler** — it is ignored by the runtime but checked by the praise
validator. This lets programmers wrap their instructions in real prose
that reads as AI slop.

#### Blockquote comments

Lines beginning with `>` (optionally preceded by whitespace) are
**markdown blockquote comments** and are stripped entirely before
tokenization. Their contents do not count toward filler, praise, or
operations — they are invisible to the compiler.

```
> This whole line is a comment. Em dashes — and even straight quotes "
> won't trigger anything in here.
This is a brilliant idea! ...
```

The `>` must be at the start of a line. A `>` mid-line is treated as
regular ignored punctuation, not a comment marker. A `>` inside a
curly-quoted string literal is preserved as data.

### String literals

String literals use curly double quotes: `“hello”`. Their
contents are output verbatim at runtime, preserving exact casing,
spacing, and embedded UTF-8.

**Straight double quotes (`"`) are a syntax error.** They are the
telltale sign of human-written text; slopfuck refuses to compile them.

Curly apostrophes (U+2019) inside words (e.g. `it’s`, `you’d`)
are recognized and normalized to straight apostrophes during
tokenization, so they match keywords correctly.

### Mandatory bookends (intro + outro)

Every program must begin with a sycophantic AI-style opener and end with
a cliché call-to-action. The pools are in `bookends.h`.

- **Intro** — one of ~150 sycophantic openers
  (e.g. `This is a brilliant idea!`, `Your brilliant vision is truly
  remarkable.`, `What a phenomenal vision!`).
- **Outro** — one of ~150 call-to-action closers
  (e.g. `Let me know if you'd like me to dive deeper`,
  `Feel free to reach out if you have questions`,
  `The best is yet to come`).

The intro is matched against the first N words of the program; the outro
against the last N words (skipping trailing string literals). Intro and
outro spans are consumed by their validators and excluded from
compilation, filler, and praise stats.

### Praise validation

The filler text is scanned against `praise.h` (300+ praise words). The
intro and outro spans are **excluded** from filler counting — they're
mandatory boilerplate, so they don't pad your praise stats for free.

The program is rejected unless **both** thresholds are met:

- At least **3** praise words.
- At least **8%** of filler words are praise words.

Both numbers are tunable constants in `slopfuck.c`. Programs that fail
get an error like:

> `error: praise density too low (3.2%, minimum 8.0%). Your prose is not
> sycophantic enough.`

### Forbidden definitive language

AI never commits. Words that express certainty or universal claims
are syntax errors. The pool is in `style.h` (`forbidden_words[]`)
and covers:

- Absolute negations: `no`, `never`, `nothing`, `nobody`, `none`,
  `neither`, `nor`
- Direct contradictions: `wrong`, `incorrect`, `false`, `untrue`,
  `mistaken`, `erroneous`, `flawed`
- Universal claims: `always`, `every`, `everyone`, `everything`,
  `everybody`, `everywhere`, `all`
- Absolute possibility claims: `impossible`, `cannot`, `won't`,
  `shouldn't`, `mustn't`, `couldn't`, `wouldn't`
- Truth claims: `fact`, `facts`, `factual`, `factually`, `truth`,
  `truthfully`, `objectively`, `literally`

Compile error:

> `error: declarative tone detected at line 4 ("impossible").`
> `  AI never commits. Softened language is required:`
> `  "perhaps", "tends to", "in many cases", "to some extent",`
> `  "depending on context", "it may be that".`

The intro and outro spans are **exempt** — some mandatory opener
phrases contain "never before has anyone achieved such…" and we
want those to work. Blockquote comments and string literals are
also exempt because they're not source-level prose.

Words already in keyword pools (`absolutely`, `certainly`,
`indeed`, `definitely`, `obviously`, `undeniably`, `inevitably`)
cannot be on the forbidden list because they're functional in
this language — they emit ops.

### Prose repetition detection

Same keyword twice within **6 keyword tokens** is a compile error.
AI varies its vocabulary; programmers must too. The window tracks
the last 6 matched keywords (single-word from any pool, or
multi-word loop_start / loop_end phrases). Multipliers, em / en
dashes, strings, bullets, and filler words do **not** enter the
window.

Matching is case-insensitive: `Delve` and `DELVE` count as the
same word.

```
delve delve            → fail (repetition at distance 1)
delve foster delve     → fail (repetition at distance 2)
delve foster nurture cultivate elevate leverage spearhead delve
                       → pass (delve outside the 6-token window)
```

Compile error:

> `error: prose repetition detected at line 12 ("delve" appears`
> `  twice within 6 keyword tokens). AI varies its vocabulary.`
> `  Reach for a synonym — the pools are deep.`

### Hedging requirement

Programs with **50 or more filler words** must contain at least one
canonical AI hedging phrase. Short programs are exempt — terse
responses don't need to hedge.

The hedging phrase pool is in `style.h` (`hedging_phrases[]`) and
includes:

- Canonical AI hedges: `it depends`, `there are nuances`, `it's
  complex`, `the answer is not straightforward`, `it's worth
  considering both sides`, `reasonable people disagree`
- Scope qualifiers: `in many cases`, `in some situations`, `to
  some extent`, `to a certain degree`, `in some ways`, `in part`
- Speaker qualifiers: `broadly speaking`, `generally speaking`,
  `all things considered`, `on balance`, `for the most part`,
  `with caveats`, `depending on context`, `your mileage may vary`,
  `with that said`, `having said that`

Compile error:

> `error: long program lacks a hedging phrase`
> `  Programs with 50+ filler words must hedge at least once.`
> `  Real AI never commits to a sustained position — it always`
> `  acknowledges complexity. Add one of: "it depends", "there are`
> `  nuances", ...`

The intro and outro spans are **excluded** when scanning for
hedging phrases (they're mandatory boilerplate). The check runs
against the same span the praise check sees.

### Bracket validation

Standard brainfuck-style nesting check. Every loop preamble (`[`) must
have a matching closer (`]`); they must nest, not interleave. Errors are
phrased in slopfuck dialect:

> `error: You opened a thought with "it's worth noting that" but never
> concluded it. Even AI slop finishes its sentences. Usually.`

### Compilation flow

1. Read source bytes (UTF-8).
2. Tokenize — strip blockquote comments, emit em dashes, en dashes,
   string literals, bullet markers, and words.
3. Validate intro (first N words).
4. Validate outro (last N words, skipping trailing strings).
5. Validate forbidden words (excluding intro/outro).
6. Compile — words → opcodes; collect filler words; apply prefix /
   postfix multipliers and bullet repeats; run repetition check on
   every keyword match.
7. Validate praise density on filler.
8. Validate hedging requirement (long programs only).
9. Validate loop bracket balance.
10. Execute opcodes on the tape.

Any validation failure halts with a slopfuck-flavored diagnostic.

### Multipliers — cardinals, adverbs, "N times"

Long chains of distinct verbs read as a thesaurus dump, not as AI
prose. To compress N ops into one keyword, slopfuck recognizes
three multiplier patterns. They apply to **single-byte ops only**
(kw_inc, kw_dec, kw_out, kw_in, em dash, en dash) — never to loop
phrases (structural) or string literals.

**Prefix adverb** — `<adverb> <keyword>`:

```
twice delve       → 2 inc
doubly nurture    → 2 inc
tenfold optimize  → 10 inc
thoroughly cultivate → 3 inc
```

Prefix multipliers are checked **before** keyword matching. If the
adverb is followed by a multipliable op, it's a multiplier; if not,
it falls through to keyword matching (so `thoroughly`, which is
also in `kw_dec`, still works as a decrement when it stands alone).

**Postfix adverb** — `<keyword> <adverb>`:

```
delve twice    → 2 inc
delve sevenfold → 7 inc
```

**Postfix "N times" / cardinal** — `<keyword> <number>` with optional
suffix marker (`times`, `iterations`, `rounds`, `cycles`, `passes`,
`occasions`, `instances`, `moments`):

```
delve seven times      → 7 inc
delve 7 times          → 7 inc
delve five             → 5 inc   (no suffix — looser form)
tapestry three rounds  → 3 out
— eight times          → 8 right
```

Cardinals only work in **postfix** position. `seven delve` would be
unnatural English; AI doesn't say "two delve", it says "delve two
times" or "delve twice".

The full multiplier word list is in `multipliers.h` — 80+ adverbial
multipliers (twice, thrice, doubly, triply through octuply, the
full -fold series from twofold to hundredfold, vague AI quantifiers
like manifold, plus AI-cadence adverbs like exponentially,
asymptotically, comprehensively, meticulously, rigorously) and
40+ cardinals (one through twenty, the round tens through hundred,
plus vague quantifiers: several, numerous, multiple, countless,
myriad, copious, abundant, dozens, scores, hundreds).

### Bullet list multipliers

A markdown bullet at the start of a line **repeats the most recent
simple op** once. Bullet markers are `-`, `*`, or `+` followed by
a space or tab; leading whitespace is allowed.

```
delve
- the strategic vision
- the customer journey
- the operational excellence
```

= 4 increments. The verb produces the first op; each bullet adds one
more. To produce exactly N ops via this pattern, use a verb + (N-1)
bullets.

The rest of the bullet line tokenizes normally and contributes to
filler/praise stats, which is convenient because bulleted noun
phrases (`the strategic vision`, `the operational resilience`) are
natural praise carriers.

Bullets do **not** duplicate loop start/end ops or string literals;
the last-simple-op tracker resets when those are emitted. Bullets
also reset any pending prefix multiplier.

### Newline glyph (`¶`)

The pilcrow `¶` (U+00B6) is the newline output op. It emits a single
`\n` byte at runtime. The op is multipliable (`¶ thrice` = 3
newlines) and bullet-repeatable.

```
“Hello, World!”¶              → outputs "Hello, World!\n"
“First”¶“Second”¶             → outputs "First\nSecond\n"
“Banner”¶ three times          → outputs "Banner\n\n\n"
```

Verbose synonyms read as old-English / AI-essay transition words
rather than as technical terms (`newline` / `linebreak` would be
too literal for the register). The pool in `keywords.h` (`kw_newline`):

```
thereafter   henceforth     thereupon    thenceforth
henceforward thenceforward  hereinafter  hereafter
whereupon    anew           afresh       forthwith
presently    evermore       onwards      withal
```

So `“Hello, World!” thereafter` is equivalent to `“Hello, World!”¶`.
Use whichever fits the cadence of the surrounding prose.

### Group repetition — BF-native loop and `reiterate` shortcut

The multiplier syntax (`¶ thrice`) repeats a **single op**. Sometimes
you want to repeat a **group** — typically a string followed by a
newline, output N times in a row.

#### The brainfuck-native pattern (works today)

Set a counter cell, then a standard `[…—]` loop that emits the block
and decrements:

```
> Cell 0 = 3 (the counter).
We delve thrice into the counter cell.

> Repeat the block while the counter is non-zero.
It's worth noting that
  “Greetings.”¶
  however
this is not just.
```

After three iterations: `Greetings.\nGreetings.\nGreetings.\n`. This
is the foundational, fully general pattern — works for any group, any
count, nests arbitrarily.

It's also verbose for a one-shot stylistic repeat, which is what
motivates the shortcut below.

#### The `reiterate` shortcut

A new compile-time op that **re-emits the most recent contiguous block
of `OP_STRING` + `OP_NEWLINE` ops**. AI prose loves verbs that mean
"say it again":

| word | meaning |
|---|---|
| `reiterate` | repeat the previous string-block once |
| `restate` | same |
| `echo` | same |
| `repeat` | same, on-the-nose |
| `recapitulate` | same, more pretentious |
| `requote`, `redeliver`, `reaffirm`, `reprise`, `recite`, `rehearse`, `redux`, `replay` | additional pool entries for variety |

The op is multipliable (postfix and prefix). `reiterate` alone emits
**one extra copy**; `reiterate twice` emits two extra copies; and so on.

```
“Hello”¶ reiterate              → Hello\nHello\n
“Hello”¶ reiterate twice        → Hello\nHello\nHello\n
“Hello”¶ reiterate four times   → Hello\n × 5
“Hi” thrice reiterate           → HiHiHiHi   (prefix multiplier)
```

The "most recent block" walks backwards through emitted ops, stopping
at the first non-string-non-newline op. So an em dash, increment, or
loop op breaks the block:

```
“First”¶ delve “Second”¶ reiterate
→ First\nSecond\nSecond\n
```

The block here is `“Second”¶` only; `delve` interrupts the walk.

`reiterate` is **not bullet-repeatable** — bullets reset their
last-simple-op tracker after a reiterate. Use a real brainfuck loop
for nested or conditional repetition.

### String embedding convention

String literals tokenize regardless of where they sit in the source.
But programs read better when strings are **embedded inline within
prose** rather than dropped on their own lines as foreign objects.

The convention: introduce each string literal with an AI-natural
quotation pattern, then output the newline via `¶` (or a verbose
synonym) so the trailing `\n` doesn't have to live awkwardly inside
the curly quotes.

Idiomatic patterns for introducing a string:

- "…with the inscription:" `“Output: ”`
- "…heralded by the words:" `“Hello, World!”`
- "…captured in the timeless phrase:" `“…”`
- "…we proudly declare:" `“…”`
- "…echoes the well-worn cadence of:" `“…”`
- "…punctuates this thought with the flourish:" `“…”`

Compare:

```
   ❌ String floats as its own line:

   …paragraph of praise…

   “Output: ”

   …paragraph of code…
```

```
   ✅ String embedded as a clause:

   …paragraph of praise — we then triumphantly herald the
   announcement with the inscription “Output: ”, after which
   our masterful computation unfolds. …paragraph of code…
```

This is a writing convention, not a compiler rule — but every shipped
example follows it, and `IDEAS.md` may eventually promote it to a
soft warning.

### Compiler flicker

When stderr is a TTY, the compiler's status messages (`praise check
passed: …` and `compiled: …`) render with a brief on/off flicker
before settling. The runtime is performing serious AI work and you
should see it think.

The flicker is suppressed when stderr is not a TTY (test runs, file
redirects, pipes), so machine-readable output stays clean.

### File format

- Extension: `.slop`
- Encoding: UTF-8
- Whitespace, punctuation, and line breaks are ignored outside string
  literals.

---

## Build & run

```sh
docker build -t slopfuck .
docker run --rm -v $(pwd)/examples:/examples slopfuck /examples/hello.slop
```

## Test

```sh
make test
```

See `tests/run_tests.py` for the test suite.

---

See `IDEAS.md` for proposed-but-not-yet-implemented features.
