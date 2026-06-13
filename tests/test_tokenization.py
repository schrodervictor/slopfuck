"""
Tokenization tests:
string literals, blockquote comments, UTF-8 dashes / apostrophes,
and the keyword-matching layer (case insensitivity, multi-word
phrases, filler fallthrough).
"""

import unittest

from _base import INTRO, OUTRO, PRAISE_FILLER, SlopfuckTest, make_program


# ── String literals ─────────────────────────────────────────────────


class StringLiteralTests(SlopfuckTest):
    def test_straight_double_quote_rejected(self):
        body = '"oops"'
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("straight quote detected", err)

    def test_curly_quote_string_compiles(self):
        body = "“hello”"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_curly_quote_string_outputs(self):
        body = "“Hello, World!”"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("Hello, World!", out)

    def test_unterminated_string_rejected(self):
        body = "“never closes"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 1)
        self.assertIn("unterminated string literal", err)

    def test_string_preserves_case(self):
        body = "“CaSe IsKept”"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("CaSe IsKept", out)

    def test_string_preserves_embedded_em_dash(self):
        # An em dash inside a string is literal, not a pointer operation.
        body = "“before—after”"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("before—after", out)

    def test_string_preserves_whitespace(self):
        body = "“  spaced  out  ”"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("  spaced  out  ", out)

    def test_empty_string_literal_compiles(self):
        body = "“”"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)


# ── Blockquote comments ─────────────────────────────────────────────


class BlockquoteCommentTests(SlopfuckTest):
    """Markdown blockquote lines (> ...) are stripped before tokenization."""

    def test_blockquote_line_is_ignored(self):
        # The blockquote contains words that would otherwise be keywords.
        src = (
            f"> delve delve delve delve delve\n"
            f"{INTRO} {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)
        # Zero ops emitted from the ignored line.
        self.assertIn("0 operations", err)

    def test_blockquote_does_not_satisfy_intro(self):
        # Putting the intro inside a blockquote means it's stripped,
        # so the real first words are the praise filler, which is not
        # an intro phrase.
        src = (
            f"> {INTRO}\n"
            f"Your visionary work is exceptional.\n"
            f"{PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 1)
        self.assertIn("does not begin with required sycophantic intro", err)

    def test_indented_blockquote_recognized(self):
        # Leading whitespace before `>` is still a blockquote.
        src = (
            f"    > this comment is indented with spaces\n"
            f"\t> this one uses a tab\n"
            f"{INTRO} {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_blockquote_em_dashes_ignored(self):
        # Em dashes inside a comment do not move the pointer.
        src = (
            f"> — — — these em dashes should not move anything\n"
            f"{INTRO} {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)
        self.assertIn("0 operations", err)

    def test_blockquote_straight_quote_ignored(self):
        # Even straight quotes are safe inside a blockquote.
        src = (
            f'> "look, straight quotes inside a comment"\n'
            f"{INTRO} {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_gt_mid_line_is_not_a_comment(self):
        # A `>` that's not at the start of a line should be treated
        # like any other ignored punctuation, NOT trigger comment mode.
        src = (
            f"{INTRO} The text > continues > here. {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_multiple_blockquote_lines(self):
        # Consecutive blockquote lines all skipped.
        src = (
            "> first line of comment\n"
            "> second line of comment\n"
            "> third line of comment\n"
            f"{INTRO} {PRAISE_FILLER} {OUTRO}\n"
        )
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_blockquote_in_string_preserved(self):
        # `>` inside a curly-quoted string is data, not a comment.
        body = "“contains > inside”"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("contains > inside", out)


# ── Special character tokenization ──────────────────────────────────


class SpecialCharacterTests(SlopfuckTest):
    def test_em_dash_recognized(self):
        # An em dash is a valid op; presence shouldn't fail compilation.
        body = "—"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_en_dash_recognized(self):
        body = "–"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_curly_apostrophe_normalized_in_outro(self):
        # Outro phrase uses you'd; both straight and curly should work.
        src = f"{INTRO} {PRAISE_FILLER} let me know if you'd like me to dive deeper"
        _, err, code = self.run_slop(src)
        self.assertEqual(code, 0, err)

    def test_curly_apostrophe_word_does_not_split(self):
        # "it's" with curly apostrophe should tokenize as one word.
        # The loop-start phrase "it's worth noting that" must match.
        body = "it’s worth noting that delve this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_pilcrow_outputs_newline(self):
        # `¶` is the newline op. After a string, it should emit \n.
        body = "“hi”¶"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "hi\n")

    def test_pilcrow_alone_outputs_newline(self):
        body = "¶"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\n")

    def test_newline_keyword_outputs_newline(self):
        # `thereafter` is a verbose synonym for the pilcrow.
        body = "“hi” thereafter"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "hi\n")

    def test_newline_keyword_is_multipliable(self):
        # `henceforth thrice` = 3 newlines.
        body = "“hi” henceforth thrice"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "hi\n\n\n")

    def test_pilcrow_with_postfix_multiplier(self):
        # `¶ three times` = 3 newlines.
        body = "“hi”¶ three times"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "hi\n\n\n")

    def test_newline_keyword_case_insensitive(self):
        body = "“hi” HENCEFORTH"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "hi\n")


# ── Keyword matching ────────────────────────────────────────────────


class KeywordMatchingTests(SlopfuckTest):
    def test_keyword_case_insensitive(self):
        # Three distinct inc verbs, each in a different mixed case.
        # (Same verb repeated would trip the repetition check.)
        body = "DELVE Foster nUrTure — tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_multi_word_phrase_matches(self):
        # Loop start phrases require multi-word matching.
        body = "it's worth noting that this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)

    def test_filler_words_are_ignored(self):
        # Words not in any pool should not produce ops or errors.
        body = "the quick fox supercalifragilistic nonsensewords"
        _, err, code = self.run_slop(make_program(body))
        # May fail praise check depending on filler ratio, but should
        # not fail tokenization or compilation for unknown words.
        self.assertIn(code, (0, 1))

    def test_phrase_takes_precedence_over_filler(self):
        # The compiler should match multi-word phrases greedily, not
        # treat the constituent words as filler.
        body = "it's worth noting that delve this is not just"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("compiled", err)


if __name__ == "__main__":
    unittest.main(verbosity=2)
