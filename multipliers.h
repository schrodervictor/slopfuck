#ifndef MULTIPLIERS_H
#define MULTIPLIERS_H

// ============================================================
// Multiplier word tables
// ============================================================
//
// AI prose quantifies. Where brainfuck says `+++++++`, AI says
// "we delve seven times across the foundational dimensions" or
// "let us doubly nurture this exceptional groundwork".
//
// slopfuck supports three multiplier patterns:
//
//   1. Prefix adverb:  "<adverb> <keyword>"   ("twice delve")
//   2. Postfix adverb: "<keyword> <adverb>"   ("delve twice")
//   3. N-times suffix: "<keyword> <number> times"
//                                              ("delve seven times")
//
// Multipliers apply ONLY to single-byte ops: kw_inc, kw_dec,
// kw_out, kw_in, em dash (right), en dash (left). They do not
// apply to loop start/end (structural) or string literals.
//
// Numeric digit literals (2, 3, 100, …) are also recognized in
// the N-times pattern: "delve 7 times" works.
//
// All matching is case-insensitive.
// ============================================================

typedef struct {
    const char *word;
    int value;
} Multiplier;

// ── ADVERBIAL MULTIPLIERS ───────────────────────────────────
// Single-word adverbs that act as a multiplier on the adjacent
// keyword. Used in prefix ("twice delve") or postfix ("delve twice")
// position. The "-fold" series covers small integers up to 100;
// vague AI quantifiers like "manifold" map to plausible defaults.
static const Multiplier multipliers_adv[] = {
    // ── Latin/old-English cardinals ─────
    {"twice",        2},
    {"thrice",       3},

    // ── -ly adverbial multipliers ───────
    {"doubly",       2},
    {"triply",       3},
    {"quadruply",    4},
    {"quintuply",    5},
    {"sextuply",     6},
    {"septuply",     7},
    {"octuply",      8},
    {"nonuply",      9},
    {"decuply",     10},
    {"centuply",   100},

    // ── -fold cardinal multipliers ──────
    {"twofold",      2},
    {"threefold",    3},
    {"fourfold",     4},
    {"fivefold",     5},
    {"sixfold",      6},
    {"sevenfold",    7},
    {"eightfold",    8},
    {"ninefold",     9},
    {"tenfold",     10},
    {"elevenfold",  11},
    {"twelvefold",  12},
    {"thirteenfold",13},
    {"fourteenfold",14},
    {"fifteenfold", 15},
    {"sixteenfold", 16},
    {"twentyfold",  20},
    {"thirtyfold",  30},
    {"fortyfold",   40},
    {"fiftyfold",   50},
    {"sixtyfold",   60},
    {"hundredfold",100},

    // ── Vague AI quantifier adverbs ─────
    {"manifold",     7},
    {"multifold",    5},
    {"manyfold",     5},
    {"countlessfold",9},
    {"infinitefold",16},

    // ── AI-style adverbial intensifiers ─
    // These read as praise/emphasis but quietly multiply.
    {"exponentially", 8},
    {"asymptotically",6},
    {"comprehensively",5},
    {"holistically",  4},
    {"meticulously",  3},
    {"thoroughly",    3},
    {"systematically",4},
    {"strategically", 3},
    {"deliberately",  3},
    {"methodically",  4},
    {"rigorously",    3},
    {"intentionally", 3},
    {"purposefully",  3},
    {"painstakingly", 4},
    {"relentlessly",  5},
    {"tirelessly",    4},
    {"persistently",  3},
    {"continually",   4},
    {"perpetually",   5},
    {"unwaveringly",  3},
    {"unrelentingly", 4},
    {"unyieldingly",  3},
    {"unceasingly",   5},
    {"endlessly",     6},

    {NULL, 0}
};

// ── CARDINAL NUMBERS (for "N times" pattern) ────────────────
// These are matched in the "<keyword> <cardinal> times" pattern.
// They also work in standalone position as a postfix multiplier
// hint (rare in real AI prose, but the parser permits it).
//
// Vague quantifiers ("several", "numerous", "countless") get
// plausible defaults so AI hedge language compiles to real ops.
static const Multiplier cardinals[] = {
    {"one",          1},
    {"two",          2},
    {"three",        3},
    {"four",         4},
    {"five",         5},
    {"six",          6},
    {"seven",        7},
    {"eight",        8},
    {"nine",         9},
    {"ten",         10},
    {"eleven",      11},
    {"twelve",      12},
    {"thirteen",    13},
    {"fourteen",    14},
    {"fifteen",     15},
    {"sixteen",     16},
    {"seventeen",   17},
    {"eighteen",    18},
    {"nineteen",    19},
    {"twenty",      20},
    {"thirty",      30},
    {"forty",       40},
    {"fifty",       50},
    {"sixty",       60},
    {"seventy",     70},
    {"eighty",      80},
    {"ninety",      90},
    {"hundred",    100},

    // ── Vague AI quantifiers ────────────
    {"several",      3},
    {"numerous",     5},
    {"multiple",     4},
    {"various",      4},
    {"countless",    7},
    {"innumerable",  8},
    {"myriad",       7},
    {"copious",      5},
    {"abundant",     6},
    {"plentiful",    6},
    {"ample",        5},

    // ── AI-flavored compound quantifiers ─
    // ("dozens of", "scores of" — single-word forms here; the
    //  "of" is filler so "dozens of strategic pillars" works.)
    {"dozens",      12},
    {"scores",      20},
    {"hundreds",   100},
    {"countlessly",  9},
    {"numerously",   6},

    {NULL, 0}
};

// ── KEYWORD lookups ─────────────────────────────────────────
// "times" — the suffix marker for the N-times pattern.
//          ("delve seven times")
// "fold"  — alternative suffix ("delve sevenfold" — but that's
//          a single word, handled above).
//
// For multi-word AI patterns like "across N dimensions", the
// markers are listed below.
static const char *times_markers[] = {
    "times",
    "iterations",
    "rounds",
    "cycles",
    "passes",
    "occasions",
    "instances",
    "moments",
    NULL
};

#endif // MULTIPLIERS_H
