#ifndef STYLE_H
#define STYLE_H

// ============================================================
// Style enforcement word lists
// ============================================================
//
// Three secondary prose constraints beyond the keyword pools:
//
//   1. forbidden_words   — declarative language the compiler rejects.
//                          AI never commits, so these read as too
//                          assertive.
//   2. hedging_phrases   — at least one must appear in long programs.
//                          AI hedges; so must slopfuck source.
//   3. (repetition window) — implementation in slopfuck.c. Same
//                          keyword twice within N tokens is rejected.
//
// Words already in keyword pools (absolutely, certainly, indeed,
// definitely, obviously, undeniably, inevitably …) cannot be
// forbidden because they're linguistically functional in this
// language. The forbidden list is therefore conservative — words
// that have no role in any pool and read as too declarative.
// ============================================================

// ── FORBIDDEN DEFINITIVE WORDS ──────────────────────────────
// Programs containing any of these words fail to compile.
// Matched case-insensitively against tokenized words. The intro
// and outro spans are excluded from the check (some sycophantic
// openers contain "never before has anyone achieved such…").
static const char *forbidden_words[] = {
    // ── Absolute negations ──────────────
    "no",
    "never",
    "nothing",
    "nobody",
    "none",
    "neither",
    "nor",

    // ── Direct contradictions ───────────
    "wrong",
    "incorrect",
    "false",
    "untrue",
    "mistaken",
    "erroneous",
    "flawed",

    // ── Universal claims ────────────────
    "always",
    "every",
    "everyone",
    "everything",
    "everybody",
    "everywhere",
    "all",

    // ── Absolute possibility claims ─────
    "impossible",
    "cannot",
    "won't",
    "shouldn't",
    "mustn't",
    "couldn't",
    "wouldn't",

    // ── Truth claims ────────────────────
    "fact",
    "facts",
    "factual",
    "factually",
    "truth",
    "truthfully",
    "objectively",
    "literally",

    NULL
};

// ── HEDGING PHRASES ─────────────────────────────────────────
// Long programs (more than MIN_FILLER_FOR_HEDGE filler words)
// must contain at least one of these phrases somewhere outside
// the intro/outro. AI hedges; so must your program.
//
// Matched case-insensitively, multi-word, on filler tokens.
static const char *hedging_phrases[] = {
    // ── Canonical AI hedges ─────────────
    "it depends",
    "it really depends",
    "it depends on the context",
    "it depends on context",
    "there are nuances",
    "there are many nuances",
    "there are many factors",
    "it's complex",
    "it is complex",
    "it's complicated",
    "it is complicated",
    "it's not that simple",
    "it is not that simple",
    "the answer is not straightforward",
    "the answer is nuanced",
    "the answer depends",
    "it's worth considering both sides",
    "there are multiple perspectives",
    "there are multiple angles",
    "reasonable people disagree",

    // ── Scope qualifiers ────────────────
    "in many cases",
    "in some cases",
    "in certain cases",
    "in some situations",
    "in certain situations",
    "in most situations",
    "in some contexts",
    "in many contexts",
    "to some extent",
    "to a certain extent",
    "to a certain degree",
    "to some degree",
    "to a large degree",
    "in some ways",
    "in many ways",
    "in certain ways",
    "in part",
    "in some sense",

    // ── Speaker qualifiers ──────────────
    "broadly speaking",
    "generally speaking",
    "loosely speaking",
    "roughly speaking",
    "all things considered",
    "on balance",
    "on the whole",
    "for the most part",
    "more often than not",
    "as a general rule",
    "with caveats",
    "with some caveats",
    "with appropriate caveats",
    "with the right approach",
    "depending on context",
    "depending on the context",
    "depending on the situation",
    "depending on circumstances",
    "depending on the specifics",
    "your mileage may vary",
    "results may vary",
    "with that said",
    "having said that",
    "all that said",
    "with that in mind",

    NULL
};

// ── TUNABLE THRESHOLDS ──────────────────────────────────────
#define REPETITION_WINDOW 6
#define MIN_FILLER_FOR_HEDGE 50

#endif // STYLE_H
