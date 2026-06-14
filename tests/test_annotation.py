"""
Tests for annotation prose blocks (`;` … `.`).

Inside an annotation block, every token except curly-quoted string
literals becomes inert filler — em/en dashes, pilcrows, bullets,
multipliers, and all keyword pools are suppressed. The block still
contributes to praise density, hedging, and forbidden-language
validation.
"""

import unittest

from _base import INTRO, OUTRO, PRAISE_FILLER, SlopfuckTest, make_program


class AnnotationBlockTests(SlopfuckTest):
    """`;` opens an annotation block; `.` closes it."""

    def test_keywords_inside_block_are_inert(self):
        # `elevate` is kw_inc and `tapestry` is kw_out. Outside an
        # annotation block they would emit +1 then output cell 0 (= 1).
        # Inside the block, both are inert. The only emit happens after
        # the block closes: delve at cell 0 (= 0 → 1) and tapestry.
        body = "before; elevate tapestry inside the block. delve tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x01")

    def test_em_dash_inside_block_is_inert(self):
        # `—` outside is OP_RIGHT. Inside, it's inert. delve targets cell 0.
        body = "open; this aside has — a dash in it. delve tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x01")  # cell 0 was 0, +1 = 1

    def test_string_literals_still_emit_inside_block(self):
        # Strings are the single exception — they still emit OP_STRING.
        body = "open; an annotation containing “HI” still emits. "
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "HI")

    def test_block_closes_at_period(self):
        # After `.`, em dash returns to OP_RIGHT.
        # cell 0 = +1 (delve before block), block does nothing,
        # then — moves to cell 1, nurture = +1, tapestry prints cell 1.
        body = "delve before the block; an aside that is inert. — nurture tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x01")

    def test_block_can_be_reopened(self):
        # Two separate annotation blocks in the same program.
        # Two distinct kw_inc verbs so the repetition window stays clean.
        body = "delve; first block. nurture; second block. tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        # cell 0 = 2 (delve + nurture), tapestry prints it.
        self.assertEqual(out, "\x02")

    def test_forbidden_language_still_enforced_inside_block(self):
        body = "open; this should never be allowed. delve tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("declarative tone detected", err)

    def test_annotation_words_count_toward_praise(self):
        # Sparse praise outside, plus a praise-heavy annotation block.
        # The block's praise words should bring density above 8%.
        sparse_filler = "Your work is fine and acceptable and adequate."
        praise_block = (
            "; truly visionary brilliant magnificent extraordinary and "
            "remarkable outstanding phenomenal exceptional."
        )
        src = f"{INTRO} {sparse_filler}{praise_block} {OUTRO}"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_repetition_window_unaffected_by_block(self):
        # The same kw_inc verb twice within 6 keyword tokens would
        # normally trip the repetition window. Inside the block, the
        # word never enters the window.
        body = (
            "delve before; the same delve word repeats delve here delve "
            "delve four delve times. tapestry"
        )
        _, err, code = self.run_slop(make_program(body))
        # The annotation's "delve"s do not push to the window. The
        # outer "delve" + "tapestry" should compile fine.
        self.assertEqual(code, 0, err)

    def test_pilcrow_inside_block_is_inert(self):
        # Pilcrow is normally a newline op. Inside a block, it's inert.
        body = "delve; an aside ¶ with a passing pilcrow that drifts gently. tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        # Only cell 0 (= 1) is printed. No newline.
        self.assertEqual(out, "\x01")


if __name__ == "__main__":
    unittest.main(verbosity=2)
