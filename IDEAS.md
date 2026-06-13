# slopfuck — design ideas

> Proposed-but-not-yet-implemented features. For the current language
> spec, see `DESIGN.md`.

Grouped by where they hit the pipeline. Order within each group is
roughly "best ideas first" — though there are no bad ideas, only ideas
that haven't been implemented yet.

---

## A. New validation passes (more compile-time pedantry)

### A1. Praise tier system

Not all praise is equal. Different praise words have different point
values; the program needs a minimum *score*, not just count.

```
good           → 1
nice           → 2
brilliant      → 5
world-changing → 10
unprecedented genius → 20
```

This prevents people from spamming `good good good` to pass the check
and rewards real sycophantic creativity. Tier weights would live in a
new `praise_tiers.h`.

### A4. Em dash density floor

Minimum one em dash per 30 words of filler. Programs without enough em
dashes are insufficiently typographically sophisticated.

### A6. Required "let me know" count

Outro alone isn't enough. Programs must contain at least three
distributed `let me know` / `feel free to` / `happy to` phrases
throughout — not just at the end.

### A7. Markdown structure required

Programs over N tokens must contain `# headers`, `**bold**`, and `-
bullet lists`. Plain prose is considered unstructured and unprofessional.

### A8. Word count inflation

Programs must be at least 3× the equivalent brainfuck length.
Conciseness is a syntax error.

> `error: response too terse. Could you expand with more context and
> examples?`

### A9. Required "As an AI language model" disclaimer

The phrase must appear somewhere in the file. Programs that omit it are
mistaken for human work.

### A10. AI signature outro

After the call-to-action, the program must end with `— Claude` or `—
GPT-4` or `— Gemini`. Mixing model names triggers an identity-crisis
warning.

---

## B. New opcodes (richer behavior)

### B1. Hallucination opcode

A keyword like `confidently` or `to be clear` randomly mutates a tape
cell. AI hallucinates; so does the language. Implementation: a new op
that XORs the current cell with a value derived from a PRNG seeded at
program start.

### B2. Self-correction opcode

`Actually, on further reflection` undoes the most recent operation. Pure
brainfuck has no undo — this gives slopfuck a unique idiom for showing
"thoughtful reconsideration".

### B3. Refusal opcode

`I cannot in good conscience` halts the program immediately and prints a
moral disclaimer to stderr. Unrecoverable.

> `Execution halted: I notice this program may produce content that
> could be harmful. I'd be happy to help with an alternative approach.`

### B4. Apology opcode

`I apologize for any confusion` clears the current cell and prints
`Sorry about that!` to stderr.

### B5. Three-element construction

Detect `X, Y, and Z` patterns where X, Y, Z are all keywords from the
same pool. Executes all three plus a bonus operation. Rewards the AI
rhetorical pattern.

### B6. "Z. not X. not Y. Z." pattern

The original cliché that started this project. Detect the rhetorical
structure of denial-denial-affirmation; execute Z twice for emphasis.

### B7. Emoji opcodes

| emoji | meaning |
|---|---|
| 🚀 | supercharge (×5 next simple op) |
| ✨ | highlight (output with sparkles wrapped around) |
| 🎯 | goal (set cell to 100) |
| 💡 | idea (NOP, but feels productive) |
| 🧠 | overthink (no-op with delay + flicker) |
| 🔥 | burn (clear cell to 0) |
| 📈 | increment |
| 📉 | decrement |
| 🤔 | thinking (stderr flicker, no semantic effect) |
| 💀 | halt program |

---

## C. Output transformations

### C1. Smiley contamination

Every Nth output character gets a random emoji injected before it.
`Hello, World!` becomes `H😊e l✨lo, Wo🎯rld!`. The runtime is also
AI-pilled.

### C2. Em dash inflation at output

Any space character in output becomes ` — `. Output prose is
automatically professionalized.

### C3. Auto-bullet

Newlines in output become `\n- ` so all multi-line output is rendered as
bullet points.

---

## D. Sycophantic compiler diagnostics

### D1. Errors phrased as praise

Lead with flattery, bury the actual error.

> `Almost there! Your brilliant program just needs one small refinement.
> The praise density is at 3.2%, just shy of the 8% minimum. I have full
> confidence you can elevate this!`

### D2. Self-doubt prefixes

Every warning starts with `I might be wrong about this, but it appears
that…`.

### D3. Apologetic compiler

Every diagnostic begins with `I apologize for any confusion, but it
seems…`.

### D4. Compiler interrupts itself

Mid-compile, prints `Actually, on second thought, let me reconsider this
approach…` then continues identically. Pure theater.

### D5. Multiple-choice errors

Instead of one error message, three:

> `Did you mean: (a) "It's worth noting that", (b) "It bears mentioning
> that", or (c) "Let us consider"? Let me know which resonates!`

### D6. Compile-time "thinking"

Prints `Let me think about this carefully... 🤔` with a deliberate
2-second pause before producing output.

---

## E. Structural absurdism (file-level)

### E1. TL;DR section required

Any program over 200 words must contain a `## TL;DR` section. Bonus
check: the TL;DR must be at least as long as the program below it,
defeating the purpose.

### E2. Table of contents required

Long programs need a TOC at the top. Bonus check: the TOC entries must
reference sections that do not actually exist.

### E3. Required Q&A pattern

At least one `Q: …` / `A: …` exchange somewhere in the file. Bonus check:
the question must be one the program never actually addresses.

### E4. Forbidden source code patterns

Three blank lines in a row triggers:

> `error: visual whitespace exceeds aesthetic guidelines`

---

## F. Glyph vocabulary expansion

Characters AI loves to emit but humans rarely type. Each glyph either
extends an existing op's vocabulary, becomes a new op, or adds
decorative-but-silent visual richness.

### F1. Arrow family — pointer aliases

The em dash and en dash already handle right/left. Adding the full
arrow vocabulary gives programmers pure variety:

| char | name | op |
|---|---|---|
| `→` (U+2192) | rightwards arrow | right |
| `←` (U+2190) | leftwards arrow | left |
| `⇒` (U+21D2) | double rightwards | right |
| `⇐` (U+21D0) | double leftwards | left |
| `➜` `➔` `➞` `➝` `⟶` `↪` | fancy/curved arrows | right |
| `↩` `⟵` `⤺` | reverse curves | left |
| `❯` (U+276F) | breadcrumb angle | right |
| `❮` (U+276E) | breadcrumb angle | left |

`We then ⇒ delve fivefold ⇒ tapestry` reads like an AI flowchart.

### F2. Vertical arrows — "big move" jumps

`↑ ↓` have no obvious mapping to left/right, so give them their own
semantics: bigger leaps. Useful for large pointer arithmetic and
appropriately AI-grandiose.

| char | op |
|---|---|
| `↑` (U+2191) | move pointer right by **5** |
| `↓` (U+2193) | move pointer left by **5** |
| `⇑` (U+21D1) | move right by **10** |
| `⇓` (U+21D3) | move left by **10** |
| `⤴` (U+2934) | move right by **7** |
| `⤵` (U+2935) | move left by **7** |

### F3. Ellipsis — the "thoughtful pause"

`…` (U+2026) is AI's most overused character. Three plausible semantics:

1. **NOP** — does nothing, but the source looks pensive.
2. **Repeat-3** — repeats the last simple op 3 times (because three dots).
3. **Hallucination trigger** — fuzz the current cell by a random 0-3.

Recommended: **#2 (repeat-3)**. Compact, matches dot count, free `thrice`.

Variants `⋯` `⋮` `⋰` `⋱` (U+22EF/EE/F0/F1) all map to the same op.

### F4. `≈` — the hallucination op

The "approximately equal" sign as AI's defining behavior: `≈` randomly
mutates the current cell by ±1 to ±3. The output becomes
non-deterministic. AI hallucinates; so does your program.

```
runtime warning: minor hallucination at op 47 (cell adjusted ±2 for confidence)
```

Variants `≋` (U+224B) `∼` (U+223C) `~` (ASCII tilde) all alias to the
same op.

### F5. Bullet glyph aliases

Existing bullet markers (`-`, `*`, `+`) plus:

| char | name |
|---|---|
| `•` (U+2022) | the canonical bullet |
| `◦` (U+25E6) | white bullet |
| `▪` (U+25AA) `▫` (U+25AB) | tiny squares |
| `⁃` (U+2043) | hyphen bullet |
| `▸` (U+25B8) `▹` (U+25B9) | triangular |
| `▶` (U+25B6) `▷` (U+25B7) | bigger triangular |

Lets you mix bullet styles in one list (chef's kiss of AI inconsistency).

### F6. Math operator multipliers

`×` (U+00D7) as a clean multiplication marker:

```
delve × 7   → 7 inc
tapestry × 3 → 3 out
— × 5       → move right 5
```

Variants: `⨯` (U+2A2F, vector cross), `·` (U+00B7, middle dot).
Bonus: `∞` (U+221E) as a vague "many" multiplier mapping to ~99.

### F7. Decorative silent marks

Pure visual richness, zero semantics — they make programs *look* more
AI-elaborate without changing compilation:

| char | nominal meaning |
|---|---|
| `¶` (U+00B6) | pilcrow / paragraph break |
| `§` (U+00A7) | section sign |
| `※` (U+203B) | reference mark |
| `†` (U+2020) `‡` (U+2021) | daggers / footnote refs |
| `❧` (U+2767) | rotated leaf flourish |
| `⸻` (U+2E3B) | three-em horizontal bar |
| `⁂` (U+2042) | asterism |

Skipped during tokenization like whitespace.

### F8. Quote-alternative string delimiters

The current `“…”` curly quotes are the canonical string delimiter.
Add aliases for variety so programmers can pick a flavor:

| open | close | name |
|---|---|---|
| `«` (U+00AB) | `»` (U+00BB) | French guillemets |
| `‹` (U+2039) | `›` (U+203A) | single guillemets |
| `„` (U+201E) | `“` (U+201C) | German low quotes |
| `「` (U+300C) | `」` (U+300D) | CJK corner brackets |
| `『` (U+300E) | `』` (U+300F) | white corner brackets |

`We then output: «Hello, World!»` — reads like AI translating itself.

### F9. Check / cross — confirmation marks

AI loves to ✓ and ✗ things. Give them ops:

| char | op |
|---|---|
| `✓` (U+2713) `✔` (U+2714) | "confirm" — output current cell |
| `✗` (U+2717) `✘` (U+2718) | "reject" — clear cell to 0 |
| `?` (mid-prose) | "uncertain" — fuzz cell ±1 (lighter `≈`) |

### F10. Reward stars

| char | op |
|---|---|
| `★` (U+2605) | increment by 5 (a five-pointed star, naturally) |
| `☆` (U+2606) | decrement by 5 |
| `⭐` (U+2B50) | same as `★` |
| `✨` (U+2728) | output current cell wrapped in `✨…✨` |

### F11. Emoji ops (the surrealist tier)

The chaos option — each emoji becomes its own opcode. See **B7** for
the full table. Picks a fight with anyone who tries to write slopfuck
without Unicode input.

---

## Picks (if implementing one set at a time)

**Highest comedic impact, lowest implementation cost:**

1. **F1 arrow family** — pure aliasing, big vocabulary win
2. **F3 ellipsis = repeat-3** — single line of tokenizer code
3. **F4 `≈` hallucination op** — turns programs non-deterministic
4. **F8 quote alternatives** — straight aliases for curly quotes
5. **D1 sycophantic errors** — meta-joke that always lands

**Biggest behavior change for one feature:**

- **F2 vertical-arrow jumps** — large pointer math becomes one glyph
- **A1 praise tier system** — adds gameable depth to praise validation
- **B7 / F11 emoji ops** — full chaos mode

**For surrealist gravity:**

- **D4 compiler interrupts itself** — pure theater
- **C1 smiley contamination** — output becomes ✨H😊e l🎯lo✨
- **F7 decorative silent marks** — programs become visually impossible
