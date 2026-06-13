"""
Tests for the multiplier features:
cardinal numbers, adverbial multipliers ("twice", "tenfold"),
"N times" postfix, and markdown bullet repeats.
"""

import unittest

from _base import SlopfuckTest, make_program


# ── Cardinal / adverbial multipliers ────────────────────────────────


class MultiplierTests(SlopfuckTest):
    """Prefix adverbs, postfix adverbs, and `N times` patterns."""

    def test_prefix_adverb_doubles_op(self):
        # "twice delve" = 2 inc; "tapestry" outputs cell 0.
        body = "twice delve tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x02")

    def test_postfix_adverb_doubles_op(self):
        body = "delve twice tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x02")

    def test_postfix_n_times_with_cardinal(self):
        # delve 7 times → cell = 7
        body = "delve seven times tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x07")

    def test_postfix_n_times_with_digit_literal(self):
        body = "delve 5 times tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x05")

    def test_postfix_cardinal_without_times(self):
        # "delve five" alone also multiplies (looser form).
        body = "delve five tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x05")

    def test_fold_adverb(self):
        # sevenfold = 7
        body = "delve sevenfold tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x07")

    def test_vague_quantifier_multiplies(self):
        # "several" = 3 per the multiplier table.
        body = "delve several tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x03")

    def test_multiplier_applies_to_em_dash(self):
        # Increment cell 0, then jump right by 3 via prefix multiplier,
        # then output the (empty) cell. Output should be \x00.
        body = "delve thrice — tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        # delve thrice → cell 0 = 3. Em dash isn't multiplied.
        # Output should be from cell 1 = 0.
        self.assertEqual(out, "\x00")

    def test_prefix_multiplier_treats_loop_phrase_as_filler(self):
        # "twice it's worth noting that ..." should NOT multiply the
        # loop_start (which would unbalance brackets). The "twice"
        # falls through to filler because the following token starts
        # a multi-word phrase, not a multipliable single op.
        body = (
            "twice it's worth noting that "
            "— "
            "this is not just "
            "delve tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        # Cell 0 = 0 at loop_start → skip body → after loop:
        # delve makes cell 1 = 1, tapestry outputs cell 1 = 1.
        # If "twice" had multiplied loop_start, brackets would mismatch.
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x01")

    def test_dual_multiplier_keyword_and_dash(self):
        # Postfix multiplier on em dash: "— five times" moves right 5.
        # Use distinct inc verbs to avoid the repetition check.
        body = "delve — five times foster tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        # Cell 0 = 1. Move right 5 → cell 5. Cell 5 += 1 = 1. Output 1.
        self.assertEqual(out, "\x01")

    def test_prefix_adverb_not_followed_by_op_is_filler(self):
        # "twice" alone with no following op is just filler — should
        # NOT cause a compile error.
        body = "twice the magic happens. delve tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        # "twice" → filler. "the magic happens" → filler. delve = +1.
        self.assertEqual(out, "\x01")


# ── Bullet list multipliers ─────────────────────────────────────────


class BulletMultiplierTests(SlopfuckTest):
    def test_single_bullet_repeats_last_op(self):
        # delve = +1. Then bullet repeats → +1. Cell 0 = 2.
        body = (
            "delve\n"
            "- something descriptive\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x02")

    def test_multiple_bullets_stack(self):
        # delve + 4 bullets = 5 inc.
        body = (
            "delve\n"
            "- the strategic vision\n"
            "- the customer journey\n"
            "- the operational excellence\n"
            "- the cultural alignment\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x05")

    def test_bullet_asterisk_marker(self):
        body = (
            "delve\n"
            "* one\n"
            "* two\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x03")

    def test_bullet_plus_marker(self):
        body = (
            "delve\n"
            "+ one\n"
            "+ two\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x03")

    def test_indented_bullet_recognized(self):
        body = (
            "delve\n"
            "    - indented bullet\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x02")

    def test_bullet_repeats_em_dash(self):
        # — moves pointer right; bullet repeats it.
        # Use distinct inc verbs to avoid the repetition check.
        body = (
            "delve\n"
            "—\n"
            "- bullet repeats em dash\n"
            "foster\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x01")

    def test_bullet_does_not_duplicate_loop_op(self):
        # If last op was a loop_start, bullet should NOT repeat it
        # (no last_simple_op carried). Verify by composing a valid
        # program where the loop body terminates.
        body = (
            "delve twice "
            "it's worth noting that "
            "- this bullet is harmless filler\n"
            "however "
            "this transcends "
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        # If bullet duplicated loop_start, brackets would be unbalanced.
        # If bullet did nothing (correct), loop terminates: 2 → 1 → 0,
        # output is \x00.
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x00")

    def test_mid_line_dash_is_not_bullet(self):
        # A `-` in the middle of a line is not a bullet (already handled
        # because '-' inside words). Verify no spurious bullets.
        body = "delve - tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        # delve = +1. "-" mid-line → not a bullet. tapestry → output.
        self.assertEqual(out, "\x01")

    def test_bullets_and_multiplier_combine(self):
        # "delve thrice" = 3, then 2 bullets = +2 = 5 total.
        body = (
            "delve thrice\n"
            "- bullet one\n"
            "- bullet two\n"
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x05")


if __name__ == "__main__":
    unittest.main(verbosity=2)
