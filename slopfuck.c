#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "keywords.h"
#include "praise.h"
#include "bookends.h"
#include "multipliers.h"
#include "style.h"

// ── Flicker output ──────────────────────────────────────────
// AI is thinking. The compiler renders status messages with a brief
// flicker, like an LLM stream warming up. On non-TTY stderr (e.g. test
// suites, redirects), prints normally so output remains parseable.
static void flicker_stderr(const char *msg) {
    if (!isatty(fileno(stderr))) {
        fprintf(stderr, "%s\n", msg);
        return;
    }
    int len = (int)strlen(msg);
    // Five cycles of show/hide, accelerating, then settle.
    int delays_us[5][2] = {
        {30000, 80000},
        {25000, 60000},
        {20000, 50000},
        {40000, 30000},
        {15000, 20000},
    };
    for (int i = 0; i < 5; i++) {
        fprintf(stderr, "\r%s", msg);
        fflush(stderr);
        usleep(delays_us[i][1]);
        fprintf(stderr, "\r%*s", len, "");
        fflush(stderr);
        usleep(delays_us[i][0]);
    }
    fprintf(stderr, "\r%s\n", msg);
}

#define TAPE_SIZE 30000
#define MAX_OPS 65536

// ── UTF-8 multi-byte detection ──────────────────────────────
// Em dash:  U+2014 → 0xE2 0x80 0x94
// En dash:  U+2013 → 0xE2 0x80 0x93
// " open:   U+201C → 0xE2 0x80 0x9C
// " close:  U+201D → 0xE2 0x80 0x9D

static int is_em_dash(const char *p) {
    return (unsigned char)p[0] == 0xE2 &&
           (unsigned char)p[1] == 0x80 &&
           (unsigned char)p[2] == 0x94;
}

static int is_en_dash(const char *p) {
    return (unsigned char)p[0] == 0xE2 &&
           (unsigned char)p[1] == 0x80 &&
           (unsigned char)p[2] == 0x93;
}

static int is_curly_open(const char *p) {
    return (unsigned char)p[0] == 0xE2 &&
           (unsigned char)p[1] == 0x80 &&
           (unsigned char)p[2] == 0x9C;
}

static int is_curly_close(const char *p) {
    return (unsigned char)p[0] == 0xE2 &&
           (unsigned char)p[1] == 0x80 &&
           (unsigned char)p[2] == 0x9D;
}

// Pilcrow `¶` is U+00B6 → 0xC2 0xB6 (2-byte UTF-8).
static int is_pilcrow(const char *p) {
    return (unsigned char)p[0] == 0xC2 &&
           (unsigned char)p[1] == 0xB6;
}

// ── Line number tracking ────────────────────────────────────
static int line_at(const char *src, int pos) {
    int line = 1;
    for (int i = 0; i < pos; i++)
        if (src[i] == '\n') line++;
    return line;
}

// ── Lowercasing helper ──────────────────────────────────────
static void str_to_lower(char *dst, const char *src, int max) {
    int i;
    for (i = 0; i < max - 1 && src[i]; i++)
        dst[i] = tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

// ── Word list ───────────────────────────────────────────────
typedef struct {
    char **words;
    int *positions;
    int count;
    int capacity;
} WordList;

static void wordlist_init(WordList *wl) {
    wl->capacity = 1024;
    wl->words = malloc(wl->capacity * sizeof(char *));
    wl->positions = malloc(wl->capacity * sizeof(int));
    wl->count = 0;
}

static void wordlist_push(WordList *wl, const char *word, int pos) {
    if (wl->count >= wl->capacity) {
        wl->capacity *= 2;
        wl->words = realloc(wl->words, wl->capacity * sizeof(char *));
        wl->positions = realloc(wl->positions, wl->capacity * sizeof(int));
    }
    wl->words[wl->count] = strdup(word);
    wl->positions[wl->count] = pos;
    wl->count++;
}

static void wordlist_free(WordList *wl) {
    for (int i = 0; i < wl->count; i++)
        free(wl->words[i]);
    free(wl->words);
    free(wl->positions);
}

static int is_word_char(char c) {
    return isalnum((unsigned char)c) || c == '\'' || c == '-';
}

// U+2019 ' (curly apostrophe) → 0xE2 0x80 0x99
static int is_curly_apostrophe(const char *p) {
    return (unsigned char)p[0] == 0xE2 &&
           (unsigned char)p[1] == 0x80 &&
           (unsigned char)p[2] == 0x99;
}

// ── Tokenize source ─────────────────────────────────────────
// Pseudo-tokens:
//   "\x01" = em dash, "\x02" = en dash, "\x04" = bullet,
//   "\x05" = pilcrow, "\x06" = annotation-block toggle (`;`),
//   "\x07" = sentence boundary (`.`) — closes an open annotation
//             block; ignored otherwise.
// String literals: "\x03" prefix followed by the string content
//
// Comments: markdown blockquote lines (optionally indented, starting
// with `>`) are skipped entirely. The marker is plain ASCII `>`; it
// must be at the start of a line (after any whitespace) to count.
static int tokenize_source(const char *src, int len, WordList *wl) {
    int i = 0;
    int line_start = 1;
    while (i < len) {
        // Blockquote comment: lines beginning with optional whitespace
        // followed by `>` are skipped to end of line.
        if (line_start) {
            int j = i;
            while (j < len && (src[j] == ' ' || src[j] == '\t'))
                j++;
            if (j < len && src[j] == '>') {
                while (i < len && src[i] != '\n')
                    i++;
                continue;
            }
            // Bullet marker: lines beginning with optional whitespace
            // followed by `-`, `*`, or `+` and then a space/tab. Emits
            // a BULLET pseudo-token; rest of line tokenizes normally.
            if (j < len && (src[j] == '-' || src[j] == '*' || src[j] == '+')
                && j + 1 < len
                && (src[j + 1] == ' ' || src[j + 1] == '\t')) {
                wordlist_push(wl, "\x04", j);
                i = j + 2;
                line_start = 0;
                continue;
            }
        }

        if (src[i] == '\n') {
            line_start = 1;
            i++;
            continue;
        }
        if (src[i] != ' ' && src[i] != '\t')
            line_start = 0;

        // Pilcrow ¶ (2-byte UTF-8) — newline output op.
        if (i + 1 < len && is_pilcrow(src + i)) {
            wordlist_push(wl, "\x05", i);
            i += 2;
            continue;
        }

        if (i + 2 < len) {
            if (is_em_dash(src + i)) {
                wordlist_push(wl, "\x01", i);
                i += 3;
                continue;
            }
            if (is_en_dash(src + i)) {
                wordlist_push(wl, "\x02", i);
                i += 3;
                continue;
            }
            if (is_curly_open(src + i)) {
                int start = i;
                i += 3;
                int content_start = i;
                while (i + 2 < len && !is_curly_close(src + i))
                    i++;
                if (i + 2 >= len && !is_curly_close(src + i)) {
                    fprintf(stderr,
                        "error: unterminated string literal at line %d\n"
                        "  Opened with \xe2\x80\x9c but never closed with"
                        " \xe2\x80\x9d.\n"
                        "  In slopfuck, only curly quotes are valid.\n",
                        line_at(src, start));
                    return -1;
                }
                int content_len = i - content_start;
                char *token = malloc(content_len + 2);
                token[0] = '\x03';
                memcpy(token + 1, src + content_start, content_len);
                token[content_len + 1] = '\0';
                wordlist_push(wl, token, start);
                free(token);
                i += 3;
                continue;
            }
        }

        // Reject straight double quotes
        if (src[i] == '"') {
            fprintf(stderr,
                "error: straight quote detected at line %d\n"
                "  slopfuck does not accept straight quotes (\").\n"
                "  Use curly quotes (\xe2\x80\x9c\xe2\x80\x9d) instead.\n"
                "  Straight quotes are a telltale sign of human-written\n"
                "  code. Real AI slop uses typographically correct\n"
                "  quotation marks.\n",
                line_at(src, i));
            return -1;
        }

        // Annotation-block toggle and sentence boundary.
        if (src[i] == ';') {
            wordlist_push(wl, "\x06", i);
            i++;
            continue;
        }
        if (src[i] == '.') {
            wordlist_push(wl, "\x07", i);
            i++;
            continue;
        }

        if (is_word_char(src[i]) ||
            (i + 2 < len && is_curly_apostrophe(src + i))) {
            int start = i;
            char wordbuf[512];
            int wi = 0;
            while (i < len && wi < (int)sizeof(wordbuf) - 1) {
                if (is_word_char(src[i])) {
                    wordbuf[wi++] = src[i++];
                } else if (i + 2 < len && is_curly_apostrophe(src + i)) {
                    wordbuf[wi++] = '\'';
                    i += 3;
                } else {
                    break;
                }
            }
            wordbuf[wi] = '\0';
            wordlist_push(wl, wordbuf, start);
        } else {
            i++;
        }
    }
    return 0;
}

// ── Multiplier lookup ───────────────────────────────────────
// Returns the multiplier value if `word` matches; 0 otherwise.
static int lookup_multiplier(const char *word, const Multiplier *table) {
    char lower[256];
    str_to_lower(lower, word, sizeof(lower));
    for (int i = 0; table[i].word; i++) {
        char tw_lower[256];
        str_to_lower(tw_lower, table[i].word, sizeof(tw_lower));
        if (strcmp(lower, tw_lower) == 0)
            return table[i].value;
    }
    return 0;
}

// Recognizes a digit literal ("5", "100"). Returns value if so,
// 0 otherwise. Pure-digit tokens of value >0 are valid.
static int parse_digit_literal(const char *word) {
    if (!word || !*word) return 0;
    for (const char *p = word; *p; p++)
        if (*p < '0' || *p > '9') return 0;
    int v = atoi(word);
    return v > 0 ? v : 0;
}

// Checks if `word` is a "times" marker word (times, iterations, …).
static int is_times_marker(const char *word) {
    char lower[256];
    str_to_lower(lower, word, sizeof(lower));
    for (int i = 0; times_markers[i]; i++) {
        char tm_lower[256];
        str_to_lower(tm_lower, times_markers[i], sizeof(tm_lower));
        if (strcmp(lower, tm_lower) == 0) return 1;
    }
    return 0;
}

// ── Keyword matching ────────────────────────────────────────
static int match_single_word(const char *word, const char **pool) {
    char lower[256];
    str_to_lower(lower, word, sizeof(lower));
    for (int i = 0; pool[i]; i++) {
        char kw_lower[256];
        str_to_lower(kw_lower, pool[i], sizeof(kw_lower));
        if (strcmp(lower, kw_lower) == 0)
            return 1;
    }
    return 0;
}

static int match_phrase_ex(WordList *wl, int start, const char **phrases,
                           const char **matched_phrase) {
    for (int p = 0; phrases[p]; p++) {
        const char *phrase = phrases[p];
        char buf[512];
        strncpy(buf, phrase, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char *phrase_words[64];
        int pw_count = 0;
        char *tok = strtok(buf, " ");
        while (tok && pw_count < 64) {
            phrase_words[pw_count++] = tok;
            tok = strtok(NULL, " ");
        }

        if (pw_count == 0) continue;

        int matched = 1;
        for (int j = 0; j < pw_count && start + j < wl->count; j++) {
            char w_lower[256], p_lower[256];
            str_to_lower(w_lower, wl->words[start + j], sizeof(w_lower));
            str_to_lower(p_lower, phrase_words[j], sizeof(p_lower));
            if (strcmp(w_lower, p_lower) != 0) {
                matched = 0;
                break;
            }
        }
        if (matched && start + pw_count <= wl->count) {
            if (matched_phrase) *matched_phrase = phrases[p];
            return pw_count;
        }
    }
    return 0;
}

static int match_phrase(WordList *wl, int start, const char **phrases) {
    return match_phrase_ex(wl, start, phrases, NULL);
}

// ── Op list with string table ───────────────────────────────
typedef struct {
    int ops[MAX_OPS];
    char *strings[MAX_OPS];
    int count;
} OpList;

static void oplist_init(OpList *ol) {
    ol->count = 0;
    memset(ol->strings, 0, sizeof(ol->strings));
}

static void oplist_free(OpList *ol) {
    for (int i = 0; i < ol->count; i++)
        free(ol->strings[i]);
}

// ── Debug trace ─────────────────────────────────────────────
// Optional collector used by --stripped to record one display
// row per kept token (skipping filler). Each row carries the
// emitted op character and the canonical source token.
typedef struct {
    int *ops;          // OP_* code (or -1 for prefix multipliers)
    char **labels;     // canonical source token / phrase
    int *mults;        // emitted multiplier (1 by default)
    int count;
    int capacity;
} DebugTrace;

static void trace_init(DebugTrace *t) {
    t->capacity = 256;
    t->count = 0;
    t->ops = malloc(t->capacity * sizeof(int));
    t->labels = malloc(t->capacity * sizeof(char *));
    t->mults = malloc(t->capacity * sizeof(int));
}

static void trace_free(DebugTrace *t) {
    for (int i = 0; i < t->count; i++) free(t->labels[i]);
    free(t->ops);
    free(t->labels);
    free(t->mults);
}

static void trace_push(DebugTrace *t, int op, const char *label, int mult) {
    if (!t) return;
    if (t->count >= t->capacity) {
        t->capacity *= 2;
        t->ops = realloc(t->ops, t->capacity * sizeof(int));
        t->labels = realloc(t->labels, t->capacity * sizeof(char *));
        t->mults = realloc(t->mults, t->capacity * sizeof(int));
    }
    t->ops[t->count] = op;
    t->labels[t->count] = strdup(label);
    t->mults[t->count] = mult;
    t->count++;
}

// ── Repetition window ───────────────────────────────────────
// Tracks the last N matched keyword strings (lowercased). When the
// same keyword reappears within the window, the compiler rejects
// the program — AI varies its vocabulary.
//
// Multipliers, em/en dashes, strings, bullets, and filler do NOT
// enter the window. Only single-word keywords and multi-word phrases.

typedef struct {
    char items[REPETITION_WINDOW][256];
    int count;
    int head;
} RepetitionWindow;

static void rep_init(RepetitionWindow *rw) {
    rw->count = 0;
    rw->head = 0;
    memset(rw->items, 0, sizeof(rw->items));
}

// Returns 0 if the keyword is fresh in the window (and pushes it).
// Returns -1 if the keyword repeats — emits a slop-flavored error.
static int rep_check_and_push(RepetitionWindow *rw, const char *key,
                              const char *display, int line) {
    char lower[256];
    str_to_lower(lower, key, sizeof(lower));
    int n = rw->count < REPETITION_WINDOW ? rw->count : REPETITION_WINDOW;
    for (int i = 0; i < n; i++) {
        if (strcmp(rw->items[i], lower) == 0) {
            fprintf(stderr,
                "error: prose repetition detected at line %d "
                "(\"%s\" appears twice within %d keyword tokens).\n"
                "  AI varies its vocabulary. Reach for a synonym — the\n"
                "  pools are deep, and the LLM register is broader\n"
                "  than this passage suggests.\n",
                line, display, REPETITION_WINDOW);
            return -1;
        }
    }
    strncpy(rw->items[rw->head], lower, sizeof(rw->items[rw->head]) - 1);
    rw->items[rw->head][sizeof(rw->items[rw->head]) - 1] = '\0';
    rw->head = (rw->head + 1) % REPETITION_WINDOW;
    if (rw->count < REPETITION_WINDOW) rw->count++;
    return 0;
}

// ── Compilation helpers ─────────────────────────────────────
// Emit N copies of a simple op. Caps at MAX_OPS.
static int emit_op_n(OpList *ol, int op, int n) {
    for (int k = 0; k < n; k++) {
        if (ol->count >= MAX_OPS) return -1;
        ol->ops[ol->count++] = op;
    }
    return 0;
}

// Look at the words right after the current keyword and detect a
// multiplier suffix. Patterns recognized:
//   "<kw> <number> times"   ("delve seven times" → 7)
//   "<kw> <number>"          ("delve seven" — looser; accepted)
//   "<kw> <adverb>"           ("delve twice"  → 2)
//
// Returns the multiplier value (>=1) and writes the number of trailing
// words to consume into *consumed_out. Default multiplier is 1.
static int peek_postfix_multiplier(WordList *wl, int i, int end,
                                   int *consumed_out) {
    *consumed_out = 0;
    if (i + 1 >= end) return 1;

    const char *next = wl->words[i + 1];
    // Skip if next is a pseudo-token.
    if (next[0] == '\x01' || next[0] == '\x02' ||
        next[0] == '\x03' || next[0] == '\x04')
        return 1;

    // Adverbial postfix: "delve twice"
    int adv = lookup_multiplier(next, multipliers_adv);
    if (adv > 0) {
        *consumed_out = 1;
        return adv;
    }

    // Numeric postfix, optionally followed by a "times" marker word:
    //   "delve seven times" / "delve 7 times" / "delve seven"
    int n = lookup_multiplier(next, cardinals);
    if (n == 0) n = parse_digit_literal(next);
    if (n > 0) {
        // If the word after the number is a times-marker, consume it.
        if (i + 2 < end && wl->words[i + 2][0] != '\x01' &&
            wl->words[i + 2][0] != '\x02' && wl->words[i + 2][0] != '\x03' &&
            wl->words[i + 2][0] != '\x04' &&
            is_times_marker(wl->words[i + 2])) {
            *consumed_out = 2;
        } else {
            *consumed_out = 1;
        }
        return n;
    }

    return 1;
}

// ── Compilation ─────────────────────────────────────────────
// `skip_start` and `skip_end` are word counts consumed by the intro
// and outro respectively. They are excluded from compilation AND from
// filler counting.
//
// In addition to keyword matching, compile() recognizes:
//   - Prefix multipliers ("twice delve" = 2 inc ops). The adverb is
//     consumed silently before the keyword.
//   - Postfix multipliers ("delve twice" / "delve seven times").
//   - Bullet markers (\x04). Each bullet repeats the most recently
//     emitted "simple" op (inc/dec/out/in/right/left) once. Bullets
//     do not duplicate loop ops or string literals.
static int compile(WordList *wl, OpList *ol, int *filler_word_count,
                   int *filler_words_capacity, char ***filler_words_out,
                   int skip_start, int skip_end, const char *src,
                   DebugTrace *trace) {
    oplist_init(ol);
    *filler_word_count = 0;
    *filler_words_capacity = 4096;
    *filler_words_out = malloc(*filler_words_capacity * sizeof(char *));

    int end = wl->count - skip_end;
    int i = skip_start;
    int pending_multiplier = 1;
    int last_simple_op = -1;  // tracks last bullet-duplicable op
    int in_free_prose = 0;    // inside a ;... annotation block
    RepetitionWindow rep;
    rep_init(&rep);

    while (i < end) {
        if (ol->count >= MAX_OPS) {
            fprintf(stderr, "error: program too large (max %d ops)\n", MAX_OPS);
            return -1;
        }

        const char *w = wl->words[i];

        // Annotation-block toggle (;) — flips free-prose mode.
        // Inside free-prose mode, every token except string literals
        // is treated as filler: em/en dashes, pilcrows, bullets,
        // multipliers, and keyword-pool matches all become passive.
        // This gives writers an escape hatch for praise and hedging
        // padding without risking accidental ops.
        if (w[0] == '\x06') {
            in_free_prose = !in_free_prose;
            pending_multiplier = 1;
            last_simple_op = -1;
            trace_push(trace, -3, in_free_prose ? ";" : ";", 1);
            i++;
            continue;
        }
        // Sentence boundary (.) — closes an open annotation block.
        // Outside an annotation block, the token is silently consumed.
        if (w[0] == '\x07') {
            if (in_free_prose) {
                in_free_prose = 0;
                pending_multiplier = 1;
                last_simple_op = -1;
                trace_push(trace, -3, ".", 1);
            }
            i++;
            continue;
        }

        if (in_free_prose) {
            // String literals still emit — printed output is the one
            // syntactic effect the annotation block does not suppress.
            if (w[0] == '\x03') {
                ol->ops[ol->count] = OP_STRING;
                ol->strings[ol->count] = strdup(w + 1);
                ol->count++;
                trace_push(trace, OP_STRING, w + 1, 1);
                pending_multiplier = 1;
                i++;
                continue;
            }
            // Other pseudo-tokens (em/en, pilcrow, bullet) — silently
            // consumed. No op, no multiplier consumption.
            if ((unsigned char)w[0] < 0x20) {
                pending_multiplier = 1;
                i++;
                continue;
            }
            // Real word inside annotation block — pure filler. No
            // keyword matching, no repetition-window push.
            if (*filler_word_count >= *filler_words_capacity) {
                *filler_words_capacity *= 2;
                *filler_words_out = realloc(*filler_words_out,
                                            *filler_words_capacity * sizeof(char *));
            }
            (*filler_words_out)[*filler_word_count] = strdup(w);
            (*filler_word_count)++;
            pending_multiplier = 1;
            i++;
            continue;
        }

        // Bullet marker — duplicate the most recent simple op.
        if (w[0] == '\x04') {
            if (last_simple_op >= 0) {
                if (emit_op_n(ol, last_simple_op, 1) < 0) return -1;
                trace_push(trace, last_simple_op, "\xe2\x80\xa2", 1);
            }
            pending_multiplier = 1;
            i++;
            continue;
        }

        // String literal — not multipliable.
        if (w[0] == '\x03') {
            ol->ops[ol->count] = OP_STRING;
            ol->strings[ol->count] = strdup(w + 1);
            ol->count++;
            trace_push(trace, OP_STRING, w + 1, 1);
            pending_multiplier = 1;
            // strings don't update last_simple_op
            i++;
            continue;
        }

        // Pilcrow ¶ → NEWLINE (multipliable)
        if (strcmp(w, "\x05") == 0) {
            int post_consumed = 0;
            int mult = pending_multiplier *
                       peek_postfix_multiplier(wl, i, end, &post_consumed);
            if (emit_op_n(ol, OP_NEWLINE, mult) < 0) return -1;
            trace_push(trace, OP_NEWLINE, "\xc2\xb6", mult);
            last_simple_op = OP_NEWLINE;
            pending_multiplier = 1;
            i += 1 + post_consumed;
            continue;
        }

        // Em dash → RIGHT (multipliable)
        if (strcmp(w, "\x01") == 0) {
            if (i > skip_start &&
                (wl->words[i - 1][0] == '\x01' ||
                 wl->words[i - 1][0] == '\x02')) {
                int line = line_at(src, wl->positions[i]);
                fprintf(stderr,
                    "error: sequential dashes detected at line %d\n"
                    "  Two or more consecutive em/en dashes read as code,\n"
                    "  not as prose. Either insert a brief aside between\n"
                    "  them (\xe2\x80\x9c\xe2\x80\x94 with strategic"
                    " intent \xe2\x80\x94\xe2\x80\x9d) or use the postfix\n"
                    "  multiplier form (\xe2\x80\x9c\xe2\x80\x94 seven"
                    " times onward\xe2\x80\x9d).\n", line);
                return -1;
            }
            int post_consumed = 0;
            int mult = pending_multiplier *
                       peek_postfix_multiplier(wl, i, end, &post_consumed);
            if (emit_op_n(ol, OP_RIGHT, mult) < 0) return -1;
            trace_push(trace, OP_RIGHT, "\xe2\x80\x94", mult);
            last_simple_op = OP_RIGHT;
            pending_multiplier = 1;
            i += 1 + post_consumed;
            continue;
        }
        // En dash → LEFT (multipliable)
        if (strcmp(w, "\x02") == 0) {
            if (i > skip_start &&
                (wl->words[i - 1][0] == '\x01' ||
                 wl->words[i - 1][0] == '\x02')) {
                int line = line_at(src, wl->positions[i]);
                fprintf(stderr,
                    "error: sequential dashes detected at line %d\n"
                    "  Two or more consecutive em/en dashes read as code,\n"
                    "  not as prose. Either insert a brief aside between\n"
                    "  them (\xe2\x80\x9c\xe2\x80\x93 with quiet resolve"
                    " \xe2\x80\x93\xe2\x80\x9d) or use the postfix\n"
                    "  multiplier form (\xe2\x80\x9c\xe2\x80\x93 seven"
                    " times home\xe2\x80\x9d).\n", line);
                return -1;
            }
            int post_consumed = 0;
            int mult = pending_multiplier *
                       peek_postfix_multiplier(wl, i, end, &post_consumed);
            if (emit_op_n(ol, OP_LEFT, mult) < 0) return -1;
            trace_push(trace, OP_LEFT, "\xe2\x80\x93", mult);
            last_simple_op = OP_LEFT;
            pending_multiplier = 1;
            i += 1 + post_consumed;
            continue;
        }

        // Multi-word phrases (loop start/end) — not multipliable.
        const char *matched_phrase = NULL;
        int consumed = match_phrase_ex(wl, i, kw_loop_start, &matched_phrase);
        if (consumed && i + consumed <= end) {
            int line = line_at(src, wl->positions[i]);
            if (rep_check_and_push(&rep, matched_phrase,
                                   matched_phrase, line) < 0) return -1;
            ol->ops[ol->count++] = OP_LOOP_START;
            trace_push(trace, OP_LOOP_START, matched_phrase, 1);
            pending_multiplier = 1;
            last_simple_op = -1;
            i += consumed;
            continue;
        }
        matched_phrase = NULL;
        consumed = match_phrase_ex(wl, i, kw_loop_end, &matched_phrase);
        if (consumed && i + consumed <= end) {
            int line = line_at(src, wl->positions[i]);
            if (rep_check_and_push(&rep, matched_phrase,
                                   matched_phrase, line) < 0) return -1;
            ol->ops[ol->count++] = OP_LOOP_END;
            trace_push(trace, OP_LOOP_END, matched_phrase, 1);
            pending_multiplier = 1;
            last_simple_op = -1;
            i += consumed;
            continue;
        }

        // Prefix multiplier word — checked BEFORE keyword matching so
        // adverbs that appear in multiple roles (e.g. "thoroughly",
        // "rigorously") act as multipliers when followed by a
        // multipliable op, and only fall through to keyword matching
        // otherwise.
        //
        // Only adverbial multipliers ("twice", "doubly", "tenfold")
        // work in prefix position — cardinals like "two" or "seven"
        // only work in POSTFIX form because "two delve" is unnatural
        // English; "delve two times" is natural.
        int adv = lookup_multiplier(w, multipliers_adv);
        if (adv > 0 && i + 1 < end) {
            const char *nxt = wl->words[i + 1];
            int is_simple = (strcmp(nxt, "\x01") == 0 ||
                             strcmp(nxt, "\x02") == 0 ||
                             strcmp(nxt, "\x05") == 0 ||
                             match_single_word(nxt, kw_inc) ||
                             match_single_word(nxt, kw_dec) ||
                             match_single_word(nxt, kw_out) ||
                             match_single_word(nxt, kw_in) ||
                             match_single_word(nxt, kw_newline) ||
                             match_single_word(nxt, kw_reiterate));
            if (is_simple) {
                pending_multiplier = adv;
                trace_push(trace, -1, w, adv);
                i++;
                continue;
            }
            // Not followed by a multipliable op → fall through to
            // keyword/filler handling below.
        }

        // Single-word keywords (all multipliable).
        int kw_op = -1;
        if (match_single_word(w, kw_inc))           kw_op = OP_INC;
        else if (match_single_word(w, kw_dec))      kw_op = OP_DEC;
        else if (match_single_word(w, kw_out))      kw_op = OP_OUT;
        else if (match_single_word(w, kw_in))       kw_op = OP_IN;
        else if (match_single_word(w, kw_newline))  kw_op = OP_NEWLINE;

        if (kw_op >= 0) {
            int line = line_at(src, wl->positions[i]);
            if (rep_check_and_push(&rep, w, w, line) < 0) return -1;
            int post_consumed = 0;
            int mult = pending_multiplier *
                       peek_postfix_multiplier(wl, i, end, &post_consumed);
            if (emit_op_n(ol, kw_op, mult) < 0) return -1;
            trace_push(trace, kw_op, w, mult);
            last_simple_op = kw_op;
            pending_multiplier = 1;
            i += 1 + post_consumed;
            continue;
        }

        // Reiterate — compile-time expansion. Looks back through the
        // most recent contiguous block of OP_STRING and OP_NEWLINE
        // ops and emits `mult` extra copies. Multipliable in the usual
        // postfix form (`reiterate twice`) and prefix form (`twice
        // reiterate`). Not bullet-repeatable (bullets reset
        // last_simple_op to -1 after a reiterate).
        if (match_single_word(w, kw_reiterate)) {
            int line = line_at(src, wl->positions[i]);
            if (rep_check_and_push(&rep, w, w, line) < 0) return -1;
            int post_consumed = 0;
            int mult = pending_multiplier *
                       peek_postfix_multiplier(wl, i, end, &post_consumed);
            // Find the most recent contiguous string/newline block.
            int block_end = ol->count;
            int block_start = block_end;
            while (block_start > 0 &&
                   (ol->ops[block_start - 1] == OP_STRING ||
                    ol->ops[block_start - 1] == OP_NEWLINE)) {
                block_start--;
            }
            int block_len = block_end - block_start;
            // Emit `mult` extra copies of the block.
            for (int k = 0; k < mult; k++) {
                for (int j = 0; j < block_len; j++) {
                    if (ol->count >= MAX_OPS) {
                        fprintf(stderr,
                            "error: program too large (max %d ops)\n",
                            MAX_OPS);
                        return -1;
                    }
                    int src_idx = block_start + j;
                    ol->ops[ol->count] = ol->ops[src_idx];
                    if (ol->ops[src_idx] == OP_STRING &&
                        ol->strings[src_idx]) {
                        ol->strings[ol->count] =
                            strdup(ol->strings[src_idx]);
                    }
                    ol->count++;
                }
            }
            last_simple_op = -1;
            trace_push(trace, -2, w, mult);
            pending_multiplier = 1;
            i += 1 + post_consumed;
            continue;
        }

        // Not a keyword → filler
        if (*filler_word_count >= *filler_words_capacity) {
            *filler_words_capacity *= 2;
            *filler_words_out = realloc(*filler_words_out,
                                        *filler_words_capacity * sizeof(char *));
        }
        (*filler_words_out)[*filler_word_count] = strdup(w);
        (*filler_word_count)++;
        pending_multiplier = 1;
        i++;
    }
    return 0;
}

// ── Forbidden definitive language ───────────────────────────
// AI never commits. Words that express certainty or universal
// claims (no, never, always, impossible, fact, definitely…) are
// rejected. Intro and outro spans are excluded because mandatory
// bookend phrases occasionally contain "never before has anyone
// achieved such…" and the like.
static int validate_forbidden_words(WordList *wl, int skip_start,
                                    int skip_end, const char *src) {
    int end = wl->count - skip_end;
    for (int i = skip_start; i < end; i++) {
        const char *w = wl->words[i];
        if ((unsigned char)w[0] < 0x20) continue;  // pseudo-tokens
        char lower[256];
        str_to_lower(lower, w, sizeof(lower));
        for (int j = 0; forbidden_words[j]; j++) {
            char fw_lower[256];
            str_to_lower(fw_lower, forbidden_words[j], sizeof(fw_lower));
            if (strcmp(lower, fw_lower) == 0) {
                int line = line_at(src, wl->positions[i]);
                fprintf(stderr,
                    "error: declarative tone detected at line %d "
                    "(\"%s\").\n"
                    "  AI never commits. Softened language is required:\n"
                    "  \"perhaps\", \"tends to\", \"in many cases\", \"to\n"
                    "  some extent\", \"depending on context\", \"it may\n"
                    "  be that\". Avoid absolutes — every claim has a\n"
                    "  caveat.\n", line, w);
                return -1;
            }
        }
    }
    return 0;
}

// ── Hedging requirement ─────────────────────────────────────
// Long programs (>= MIN_FILLER_FOR_HEDGE filler words) must contain
// at least one canonical AI hedging phrase. Short programs are
// exempt — terse responses don't need to hedge.
static int validate_hedging(WordList *wl, int filler_count,
                            int skip_start, int skip_end) {
    if (filler_count < MIN_FILLER_FOR_HEDGE) return 0;
    int end = wl->count - skip_end;
    for (int i = skip_start; i < end; i++) {
        for (int p = 0; hedging_phrases[p]; p++) {
            char buf[512];
            strncpy(buf, hedging_phrases[p], sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';

            char *phrase_words[64];
            int pw_count = 0;
            char *tok = strtok(buf, " ");
            while (tok && pw_count < 64) {
                phrase_words[pw_count++] = tok;
                tok = strtok(NULL, " ");
            }
            if (pw_count == 0 || i + pw_count > end) continue;

            int matched = 1;
            for (int j = 0; j < pw_count; j++) {
                if ((unsigned char)wl->words[i + j][0] < 0x20) {
                    matched = 0;
                    break;
                }
                char w_lower[256], p_lower[256];
                str_to_lower(w_lower, wl->words[i + j], sizeof(w_lower));
                str_to_lower(p_lower, phrase_words[j], sizeof(p_lower));
                if (strcmp(w_lower, p_lower) != 0) {
                    matched = 0;
                    break;
                }
            }
            if (matched) return 0;
        }
    }

    fprintf(stderr,
        "error: long program lacks a hedging phrase\n"
        "  Programs with %d+ filler words must hedge at least once.\n"
        "  Real AI never commits to a sustained position — it always\n"
        "  acknowledges complexity. Add one of:\n"
        "    \"it depends\", \"there are nuances\", \"it's complex\",\n"
        "    \"the answer is not straightforward\", \"in many cases\",\n"
        "    \"depending on context\", \"on balance\", \"broadly\n"
        "    speaking\", \"with the right approach\", \"all things\n"
        "    considered\".\n",
        MIN_FILLER_FOR_HEDGE);
    return -1;
}

// ── Praise validation ───────────────────────────────────────
#define MIN_PRAISE_DENSITY 0.08
#define MIN_PRAISE_WORDS 3

static int validate_praise(char **filler_words, int filler_count) {
    if (filler_count == 0) {
        fprintf(stderr,
            "error: program contains no filler text\n"
            "  slopfuck programs must contain sycophantic prose\n"
            "  praising the user's brilliance, vision, and courage.\n"
            "  Try weaving in words like \"brilliant\", \"visionary\",\n"
            "  \"genius\", or \"exceptional\" between your instructions.\n");
        return -1;
    }

    int praise_count = 0;
    for (int i = 0; i < filler_count; i++) {
        char lower[256];
        str_to_lower(lower, filler_words[i], sizeof(lower));
        for (int j = 0; praise_words[j]; j++) {
            char pw_lower[256];
            str_to_lower(pw_lower, praise_words[j], sizeof(pw_lower));
            if (strcmp(lower, pw_lower) == 0) {
                praise_count++;
                break;
            }
        }
    }

    double density = (double)praise_count / filler_count;

    if (praise_count < MIN_PRAISE_WORDS) {
        fprintf(stderr,
            "error: insufficient sycophancy (%d praise words found, "
            "minimum %d required)\n"
            "  Your program does not adequately celebrate the user's\n"
            "  extraordinary talents. The compiler needs to see at\n"
            "  least %d praise words among the filler text.\n"
            "  Suggested additions: \"brilliant\", \"visionary\",\n"
            "  \"masterful\", \"unprecedented genius\"\n",
            praise_count, MIN_PRAISE_WORDS, MIN_PRAISE_WORDS);
        return -1;
    }

    if (density < MIN_PRAISE_DENSITY) {
        fprintf(stderr,
            "error: praise density too low (%.1f%%, minimum %.1f%%)\n"
            "  Only %d out of %d filler words are praising the user.\n"
            "  Your prose is not sycophantic enough. Every paragraph\n"
            "  should remind the reader of the user's brilliance,\n"
            "  exceptional vision, and world-changing contributions.\n",
            density * 100, MIN_PRAISE_DENSITY * 100,
            praise_count, filler_count);
        return -1;
    }

    char msg[128];
    snprintf(msg, sizeof(msg),
        "praise check passed: %.1f%% sycophancy (%d/%d words)",
        density * 100, praise_count, filler_count);
    flicker_stderr(msg);
    return 0;
}

// ── Bracket matching ────────────────────────────────────────
static int find_matching_bracket(OpList *ol, int pos, int direction) {
    int depth = 0;
    while (pos >= 0 && pos < ol->count) {
        if (ol->ops[pos] == OP_LOOP_START) depth++;
        if (ol->ops[pos] == OP_LOOP_END) depth--;
        if (depth == 0) return pos;
        pos += direction;
    }
    return -1;
}

static int validate_brackets(OpList *ol) {
    int depth = 0;
    for (int i = 0; i < ol->count; i++) {
        if (ol->ops[i] == OP_LOOP_START) depth++;
        if (ol->ops[i] == OP_LOOP_END) depth--;
        if (depth < 0) {
            fprintf(stderr,
                "error: unmatched loop end at operation %d\n"
                "  Found a closing phrase without a matching opener.\n"
                "  Every \xe2\x80\x9cthis is not just\xe2\x80\x9d needs a"
                " preceding\n"
                "  \xe2\x80\x9cit's worth noting that\xe2\x80\x9d.\n", i);
            return -1;
        }
    }
    if (depth != 0) {
        fprintf(stderr,
            "error: %d unmatched loop start(s)\n"
            "  You opened a thought with \xe2\x80\x9cit's worth noting"
            " that\xe2\x80\x9d\n"
            "  but never concluded it. Even AI slop finishes its\n"
            "  sentences. Usually.\n", depth);
        return -1;
    }
    return 0;
}

// ── Bookend validation ──────────────────────────────────────
// Returns number of words consumed (so compile() can skip them),
// or -1 on failure.
static int validate_intro(WordList *wl) {
    for (int p = 0; required_intros[p]; p++) {
        char buf[512];
        strncpy(buf, required_intros[p], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char *phrase_words[64];
        int pw_count = 0;
        char *tok = strtok(buf, " ");
        while (tok && pw_count < 64) {
            phrase_words[pw_count++] = tok;
            tok = strtok(NULL, " ");
        }
        if (pw_count == 0 || pw_count > wl->count) continue;

        int matched = 1;
        for (int j = 0; j < pw_count; j++) {
            if (wl->words[j][0] == '\x03') { matched = 0; break; }
            char w_lower[256], p_lower[256];
            str_to_lower(w_lower, wl->words[j], sizeof(w_lower));
            str_to_lower(p_lower, phrase_words[j], sizeof(p_lower));
            if (strcmp(w_lower, p_lower) != 0) {
                matched = 0;
                break;
            }
        }
        if (matched) return pw_count;
    }

    fprintf(stderr,
        "error: program does not begin with required sycophantic intro\n"
        "  Every slopfuck program must open with an AI-cliche\n"
        "  greeting that praises the user's idea. Examples:\n"
        "    \xe2\x80\x9cThis is a brilliant idea!\xe2\x80\x9d\n"
        "    \xe2\x80\x9cWhat a phenomenal vision!\xe2\x80\x9d\n"
        "    \xe2\x80\x9cYour brilliant approach is truly remarkable.\xe2\x80\x9d\n"
        "    \xe2\x80\x9cI'm genuinely impressed by this concept.\xe2\x80\x9d\n"
        "  The first words of your program must match one of\n"
        "  the approved sycophantic openers.\n");
    return -1;
}

static int validate_outro(WordList *wl) {
    // Find the last non-string-literal word index
    int last_word = wl->count - 1;
    while (last_word >= 0 && wl->words[last_word][0] == '\x03')
        last_word--;
    if (last_word < 0) last_word = wl->count - 1;
    int effective_count = last_word + 1;

    for (int p = 0; required_outros[p]; p++) {
        char buf[512];
        strncpy(buf, required_outros[p], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char *phrase_words[64];
        int pw_count = 0;
        char *tok = strtok(buf, " ");
        while (tok && pw_count < 64) {
            phrase_words[pw_count++] = tok;
            tok = strtok(NULL, " ");
        }
        if (pw_count == 0 || pw_count > effective_count) continue;

        int start = effective_count - pw_count;
        int matched = 1;
        for (int j = 0; j < pw_count; j++) {
            if (wl->words[start + j][0] == '\x03') { matched = 0; break; }
            char w_lower[256], p_lower[256];
            str_to_lower(w_lower, wl->words[start + j], sizeof(w_lower));
            str_to_lower(p_lower, phrase_words[j], sizeof(p_lower));
            if (strcmp(w_lower, p_lower) != 0) {
                matched = 0;
                break;
            }
        }
        // Return the count from outro start to end of program, so the
        // compiler skips the outro phrase plus any trailing strings.
        if (matched) return wl->count - start;
    }

    fprintf(stderr,
        "error: program does not end with required call-to-action\n"
        "  Every slopfuck program must close with an AI-cliche\n"
        "  call-to-action upselling further engagement. Examples:\n"
        "    \xe2\x80\x9cLet me know if you'd like me to explore this"
        " further!\xe2\x80\x9d\n"
        "    \xe2\x80\x9cI'd be happy to dive deeper into this.\xe2\x80\x9d\n"
        "    \xe2\x80\x9cFeel free to reach out if you have"
        " questions!\xe2\x80\x9d\n"
        "    \xe2\x80\x9cThe best is yet to come!\xe2\x80\x9d\n"
        "  The last words of your program must match one of\n"
        "  the approved sycophantic closers.\n");
    return -1;
}

// ── Execution ───────────────────────────────────────────────
static int execute(OpList *ol) {
    unsigned char tape[TAPE_SIZE] = {0};
    int ptr = 0;
    int ip = 0;

    while (ip < ol->count) {
        switch (ol->ops[ip]) {
        case OP_RIGHT:
            ptr++;
            if (ptr >= TAPE_SIZE) ptr = 0;
            break;
        case OP_LEFT:
            ptr--;
            if (ptr < 0) ptr = TAPE_SIZE - 1;
            break;
        case OP_INC:
            tape[ptr]++;
            break;
        case OP_DEC:
            tape[ptr]--;
            break;
        case OP_OUT:
            putchar(tape[ptr]);
            break;
        case OP_IN:
            tape[ptr] = (unsigned char)getchar();
            break;
        case OP_LOOP_START:
            if (tape[ptr] == 0)
                ip = find_matching_bracket(ol, ip, 1);
            break;
        case OP_LOOP_END:
            if (tape[ptr] != 0)
                ip = find_matching_bracket(ol, ip, -1);
            break;
        case OP_STRING:
            fputs(ol->strings[ip], stdout);
            break;
        case OP_NEWLINE:
            putchar('\n');
            break;
        }
        ip++;
    }
    return 0;
}

// ── Debug printers ──────────────────────────────────────────
static char op_char(int op) {
    switch (op) {
    case OP_RIGHT:      return '>';
    case OP_LEFT:       return '<';
    case OP_INC:        return '+';
    case OP_DEC:        return '-';
    case OP_OUT:        return '.';
    case OP_IN:         return ',';
    case OP_LOOP_START: return '[';
    case OP_LOOP_END:   return ']';
    case OP_STRING:     return '"';
    case OP_NEWLINE:    return 'N';
    default:            return '?';
    }
}

// Stripped view: the program with filler removed, one row per
// kept token. Each row shows the emitted op (or a multiplier
// marker) and the canonical source token / phrase.
static void print_stripped(DebugTrace *t) {
    fprintf(stdout,
        "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80 stripped program "
        "(%d kept tokens) \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n", t->count);
    for (int i = 0; i < t->count; i++) {
        int op = t->ops[i];
        const char *label = t->labels[i];
        int mult = t->mults[i];
        if (op == -1) {
            // Prefix multiplier — sets pending multiplier for next op.
            fprintf(stdout, "  \xc3\x97%-3d %s\n", mult, label);
        } else if (op == -3) {
            // Annotation block boundary (`;` toggle or `.` close).
            fprintf(stdout, "  %s    (annotation %s)\n",
                    label, label[0] == ';' ? "toggle" : "close");
        } else if (op == -2) {
            // Reiterate — repeats prior string/newline block.
            if (mult > 1)
                fprintf(stdout, "  \xe2\x86\xbb\xc3\x97%-2d %s\n", mult, label);
            else
                fprintf(stdout, "  \xe2\x86\xbb    %s\n", label);
        } else if (op == OP_STRING) {
            fprintf(stdout, "  \"    \"%s\"\n", label);
        } else if (mult > 1) {
            fprintf(stdout, "  %c\xc3\x97%-3d %s\n", op_char(op), mult, label);
        } else {
            fprintf(stdout, "  %c    %s\n", op_char(op), label);
        }
    }
}

// Opcodes view: the compiled program as a stream of BF-equivalent
// characters. Strings are emitted as ."<content>" runs, the
// pilcrow newline as ¶. Wraps at 72 columns for readability.
static void print_opcodes(OpList *ol) {
    fprintf(stdout,
        "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80 opcodes "
        "(%d ops) \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n", ol->count);
    int col = 0;
    for (int i = 0; i < ol->count; i++) {
        int op = ol->ops[i];
        if (op == OP_STRING) {
            const char *s = ol->strings[i] ? ol->strings[i] : "";
            if (col != 0) { fputc('\n', stdout); col = 0; }
            fprintf(stdout, "  .\"%s\"\n", s);
            continue;
        }
        if (op == OP_NEWLINE) {
            if (col != 0) { fputc('\n', stdout); col = 0; }
            fprintf(stdout, "  \xc2\xb6\n");
            continue;
        }
        if (col == 0) fputs("  ", stdout);
        fputc(op_char(op), stdout);
        col++;
        if (col >= 64) { fputc('\n', stdout); col = 0; }
    }
    if (col != 0) fputc('\n', stdout);
}

// ── Main ────────────────────────────────────────────────────
int main(int argc, char **argv) {
    int show_stripped = 0;
    int show_opcodes = 0;
    const char *filename = NULL;
    for (int a = 1; a < argc; a++) {
        const char *arg = argv[a];
        if (strcmp(arg, "--stripped") == 0 || strcmp(arg, "-s") == 0) {
            show_stripped = 1;
        } else if (strcmp(arg, "--opcodes") == 0 || strcmp(arg, "-o") == 0) {
            show_opcodes = 1;
        } else if (arg[0] == '-' && arg[1] != '\0') {
            fprintf(stderr, "error: unknown flag '%s'\n", arg);
            return 1;
        } else if (!filename) {
            filename = arg;
        } else {
            fprintf(stderr, "error: extra argument '%s'\n", arg);
            return 1;
        }
    }

    if (!filename) {
        fprintf(stderr,
            "slopfuck \xe2\x80\x94 the AI slop programming language\n"
            "\n"
            "Usage: slopfuck [--stripped|-s] [--opcodes|-o] <file.slop>\n"
            "\n"
            "Write programs using AI-cliche keywords surrounded\n"
            "by sycophantic filler prose. Programs that do not\n"
            "sufficiently praise the user will not compile.\n"
            "\n"
            "String literals use curly quotes: \xe2\x80\x9cHello, World!"
            "\xe2\x80\x9d\n"
            "Straight quotes (\") are a syntax error.\n"
            "\n"
            "Debug flags (skip execution):\n"
            "  --stripped, -s   print the program with filler removed\n"
            "  --opcodes,  -o   print the compiled brainfuck opcodes\n");
        return 1;
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "error: cannot open '%s'\n", filename);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(fsize + 1);
    fread(src, 1, fsize, f);
    src[fsize] = '\0';
    fclose(f);

    // Tokenize
    WordList wl;
    wordlist_init(&wl);
    if (tokenize_source(src, fsize, &wl) != 0)
        return 1;

    // Validate bookends
    int intro_consumed = validate_intro(&wl);
    if (intro_consumed < 0)
        return 1;
    int outro_consumed = validate_outro(&wl);
    if (outro_consumed < 0)
        return 1;

    // Forbidden definitive language check (outside intro/outro).
    if (validate_forbidden_words(&wl, intro_consumed,
                                 outro_consumed, src) != 0)
        return 1;

    // Compile (skip intro and outro spans; includes repetition check).
    OpList ol;
    char **filler_words = NULL;
    int filler_count = 0;
    int filler_cap = 0;
    DebugTrace trace;
    DebugTrace *trace_arg = NULL;
    if (show_stripped) { trace_init(&trace); trace_arg = &trace; }
    if (compile(&wl, &ol, &filler_count, &filler_cap, &filler_words,
                intro_consumed, outro_consumed, src, trace_arg) != 0)
        return 1;

    // Validate praise
    if (validate_praise(filler_words, filler_count) != 0)
        return 1;

    // Validate hedging (long programs only).
    if (validate_hedging(&wl, filler_count,
                         intro_consumed, outro_consumed) != 0)
        return 1;

    // Validate brackets
    if (validate_brackets(&ol) != 0)
        return 1;

    char compiled_msg[128];
    snprintf(compiled_msg, sizeof(compiled_msg),
            "compiled: %d operations from %d words (%d filler)",
            ol.count, wl.count, filler_count);
    flicker_stderr(compiled_msg);

    int result = 0;
    if (show_stripped || show_opcodes) {
        if (show_stripped) print_stripped(&trace);
        if (show_opcodes) print_opcodes(&ol);
    } else {
        result = execute(&ol);
    }

    if (show_stripped) trace_free(&trace);
    for (int i = 0; i < filler_count; i++)
        free(filler_words[i]);
    free(filler_words);
    oplist_free(&ol);
    wordlist_free(&wl);
    free(src);

    return result;
}
