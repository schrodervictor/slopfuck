"""
Tests for the compiler's four validation phases:
intro, outro, praise density, and loop bracket balance.
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
        body = "it's worth noting that delve delve"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("unmatched loop start", err)

    def test_unmatched_loop_end_fails(self):
        body = "delve delve this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("unmatched loop end", err)

    def test_balanced_brackets_pass(self):
        body = "it's worth noting that delve this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_nested_brackets_pass(self):
        body = (
            "it's worth noting that delve "
            "it's worth noting that delve this is not just "
            "this is not just"
        )
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)


if __name__ == "__main__":
    unittest.main(verbosity=2)
