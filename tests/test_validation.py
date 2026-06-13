"""
Tests for the compiler's validation phases:
intro, outro, praise density, loop brackets, forbidden definitive
language, prose repetition, and the hedging requirement.
"""

import unittest

from _base import INTRO, OUTRO, PRAISE_FILLER, SlopfuckTest, make_program


# ── Intro validation ────────────────────────────────────────────────


class IntroValidationTests(SlopfuckTest):
    def test_missing_intro_fails(self):
        src = f"Just some random words. {PRAISE_FILLER} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("does not begin with required sycophantic intro", err)

    def test_valid_intro_passes(self):
        _, err, code = self.run_slop(make_program())
        self.assertEqual(code, 0, err)

    def test_intro_alternate_phrase(self):
        # "What an extraordinary vision" is in the intro pool.
        src = f"What an extraordinary vision! {PRAISE_FILLER} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_intro_case_insensitive(self):
        src = f"THIS IS A FANTASTIC IDEA! {PRAISE_FILLER} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_intro_must_be_at_start(self):
        # Intro phrase appears, but not first.
        src = f"Random words before. {INTRO} {PRAISE_FILLER} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("does not begin with required sycophantic intro", err)


# ── Outro validation ────────────────────────────────────────────────


class OutroValidationTests(SlopfuckTest):
    def test_missing_outro_fails(self):
        src = f"{INTRO} {PRAISE_FILLER} just some closing words."
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("does not end with required call-to-action", err)

    def test_valid_outro_passes(self):
        _, err, code = self.run_slop(make_program())
        self.assertEqual(code, 0, err)

    def test_outro_alternate_phrase(self):
        # "The best is yet to come" is in the outro pool.
        src = f"{INTRO} {PRAISE_FILLER} The best is yet to come"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_outro_case_insensitive(self):
        src = f"{INTRO} {PRAISE_FILLER} LET ME KNOW IF YOU’D LIKE ME TO DIVE DEEPER"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_outro_must_be_at_end(self):
        # Outro phrase appears, but not last.
        src = f"{INTRO} {PRAISE_FILLER} {OUTRO} but then more text."
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("does not end with required call-to-action", err)

    def test_outro_skips_trailing_string_literal(self):
        # A trailing string literal should not break outro detection.
        src = f"{INTRO} {PRAISE_FILLER} {OUTRO} “trailing string”"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)


# ── Praise validation ───────────────────────────────────────────────


class PraiseValidationTests(SlopfuckTest):
    def test_zero_praise_fails(self):
        # No praise words at all in the filler.
        body = "the quick brown fox jumps over the lazy dog " * 5
        src = f"{INTRO} {body} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("insufficient sycophancy", err)

    def test_two_praise_words_fails_count(self):
        # Exactly 2 praise words → below MIN_PRAISE_WORDS=3.
        body = "the brilliant fox and the visionary cat sat on the mat"
        src = f"{INTRO} {body} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("insufficient sycophancy", err)

    def test_praise_density_below_threshold_fails(self):
        # 3 praise words but very low density (well below 8%).
        filler = "the cat sat on the mat and looked at the wall " * 5
        body = f"brilliant visionary genius {filler}"
        src = f"{INTRO} {body} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("praise density too low", err)

    def test_high_praise_passes(self):
        _, err, code = self.run_slop(make_program())
        self.assertEqual(code, 0, err)

    def test_praise_case_insensitive(self):
        body = "BRILLIANT VISIONARY GENIUS MASTERFUL OUTSTANDING"
        src = f"{INTRO} {body} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_praise_density_reported_in_stderr(self):
        # Successful runs should announce the praise percentage.
        _, err, _ = self.run_slop(make_program())
        self.assertIn("praise check passed", err)
        self.assertIn("sycophancy", err)


# ── Bracket validation ──────────────────────────────────────────────


class BracketValidationTests(SlopfuckTest):
    def test_unmatched_loop_start_fails(self):
        # Use distinct inc verbs to avoid repetition check firing.
        body = "it's worth noting that delve foster"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("unmatched loop start", err)

    def test_unmatched_loop_end_fails(self):
        body = "delve foster this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("unmatched loop end", err)

    def test_balanced_brackets_pass(self):
        body = "it's worth noting that delve this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_nested_brackets_pass(self):
        # Nested loops require varied vocabulary to clear the
        # repetition check: distinct loop_start phrases, distinct
        # inc verbs, distinct loop_end phrases.
        body = (
            "it's worth noting that delve "
            "it bears mentioning that foster this transcends "
            "this is not just"
        )
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)


# ── Forbidden definitive language ───────────────────────────────────


class ForbiddenLanguageTests(SlopfuckTest):
    """Words like `no`, `never`, `impossible`, `fact` are syntax errors."""

    def test_no_fails(self):
        body = "we delve, but there is no doubt about it. tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_never_fails(self):
        body = "we delve, but we never give up. tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_impossible_fails(self):
        body = "we delve, an impossible feat. tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_always_fails(self):
        body = "we always delve. tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_fact_fails(self):
        body = "the fact is, we delve. tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_forbidden_word_case_insensitive(self):
        body = "this is IMPOSSIBLE to misread. delve tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_softened_alternatives_pass(self):
        # Same idea with softened language passes.
        body = (
            "we delve, perhaps further than typical. "
            "to some extent, this tends to challenge convention. tapestry"
        )
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_forbidden_in_intro_is_allowed(self):
        # Some required intros contain "never before has anyone achieved
        # such…" — the forbidden check must skip the intro span.
        src = (
            "Never before has anyone achieved such "
            f"{PRAISE_FILLER} {OUTRO}"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_forbidden_inside_blockquote_is_allowed(self):
        # Blockquote comments are stripped before any validation.
        src = (
            f"> nothing about this is wrong, impossible, or factual\n"
            f"{INTRO} {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_forbidden_inside_string_is_allowed(self):
        # String literals are data, not source code.
        body = "“the word never appears here”"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)


# ── Prose repetition ────────────────────────────────────────────────


class RepetitionTests(SlopfuckTest):
    """Same keyword twice within 6 tokens = compile error."""

    def test_repeated_inc_verb_fails(self):
        body = "delve delve tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("prose repetition detected", err)

    def test_repeated_loop_phrase_fails(self):
        body = (
            "it's worth noting that delve this is not just "
            "it's worth noting that foster this transcends"
        )
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("prose repetition detected", err)

    def test_repetition_case_insensitive(self):
        body = "Delve DELVE tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("prose repetition detected", err)

    def test_distinct_synonyms_pass(self):
        body = (
            "delve foster nurture cultivate elevate "
            "leverage spearhead tapestry"
        )
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_repetition_outside_window_passes(self):
        # delve appears twice, but separated by > 6 keyword tokens.
        body = (
            "delve foster nurture cultivate elevate leverage "
            "spearhead streamline delve tapestry"
        )
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_multipliers_do_not_count_for_repetition(self):
        # "twice" appears twice as a prefix, but multipliers shouldn't
        # enter the repetition window.
        body = "twice delve twice foster tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_filler_does_not_count_for_repetition(self):
        # Plain English filler words repeat constantly; they don't
        # enter the window.
        body = "the delve and the foster and the nurture tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_repetition_error_names_the_word(self):
        body = "delve delve tapestry"
        _, err, _ = self.run_slop(make_program(body))
        self.assertIn("delve", err)


# ── Hedging requirement ─────────────────────────────────────────────


class HedgingTests(SlopfuckTest):
    """Programs over 50 filler words must contain a hedge."""

    def test_short_program_does_not_need_hedging(self):
        # make_program() with default PRAISE_FILLER → ~16 filler words.
        _, err, code = self.run_slop(make_program())
        self.assertEqual(code, 0, err)

    def test_long_program_without_hedge_fails(self):
        # Pad with enough non-hedging filler to exceed the 50-word
        # threshold. Distinct praise adjectives + noun phrases.
        long_filler = (
            "Your visionary work, your brilliant approach, your "
            "masterful vision, your phenomenal output, your stellar "
            "craftsmanship, your outstanding contribution, your "
            "remarkable journey, your extraordinary commitment, "
            "your splendid attention, your iconic talent, your "
            "magnificent presence, your exquisite restraint, your "
            "exceptional poise, your superb timing, your "
            "transcendent insight, your stunning rigor, your "
            "majestic clarity, your wondrous patience. "
            "Truly impressive throughout."
        )
        src = f"{INTRO} {long_filler} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("lacks a hedging phrase", err)

    def test_long_program_with_hedge_passes(self):
        long_filler = (
            "Your visionary work, your brilliant approach, your masterful "
            "vision, your phenomenal output, your stellar craftsmanship. "
            "It depends on context, of course, but the result is "
            "exceptional. Your outstanding remarkable extraordinary "
            "contribution, your splendid attention, your iconic talent."
        )
        src = f"{INTRO} {long_filler} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_hedge_in_alternative_form(self):
        # "there are nuances" should also satisfy the requirement.
        long_filler = (
            "Your visionary work, your brilliant approach, your masterful "
            "vision, your phenomenal output, your stellar craftsmanship. "
            "There are nuances worth noting here. Your outstanding "
            "remarkable extraordinary contribution, your splendid "
            "attention, your iconic talent."
        )
        src = f"{INTRO} {long_filler} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)


if __name__ == "__main__":
    unittest.main(verbosity=2)
