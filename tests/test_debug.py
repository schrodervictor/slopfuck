"""
Tests for the debug CLI flags `--stripped` and `--opcodes`.

Both flags skip execution and emit a purely informational dump to
stdout. They can be combined; --stripped output precedes --opcodes
output.
"""

import unittest

from _base import SlopfuckTest, make_program


class StrippedFlagTests(SlopfuckTest):
    """`--stripped` (or `-s`) prints one row per kept token."""

    def test_stripped_skips_execution(self):
        # `tapestry` at cell 0 would normally output a NUL byte. With
        # --stripped, the program does not execute, so nothing prints.
        body = "tapestry"
        out, err, code = self.run_slop(
            make_program(body), extra_args=("--stripped",)
        )
        self.assertEqual(code, 0, err)
        self.assertNotIn("\x00", out)

    def test_stripped_emits_header(self):
        out, _, _ = self.run_slop(
            make_program("delve tapestry"), extra_args=("--stripped",)
        )
        self.assertIn("stripped program", out)
        self.assertIn("kept tokens", out)

    def test_stripped_shows_kept_keywords(self):
        out, _, _ = self.run_slop(
            make_program("delve tapestry"), extra_args=("--stripped",)
        )
        # The two ops should appear with their op chars.
        self.assertIn("delve", out)
        self.assertIn("tapestry", out)

    def test_stripped_shows_multiplier(self):
        out, _, _ = self.run_slop(
            make_program("delve sevenfold tapestry"),
            extra_args=("--stripped",),
        )
        # Postfix multiplier folded into the row.
        self.assertIn("×7", out)

    def test_stripped_short_form(self):
        out, _, _ = self.run_slop(
            make_program("delve tapestry"), extra_args=("-s",)
        )
        self.assertIn("stripped program", out)


class OpcodesFlagTests(SlopfuckTest):
    """`--opcodes` (or `-o`) emits the brainfuck character stream."""

    def test_opcodes_skips_execution(self):
        body = "tapestry"
        out, err, code = self.run_slop(
            make_program(body), extra_args=("--opcodes",)
        )
        self.assertEqual(code, 0, err)
        self.assertNotIn("\x00", out)

    def test_opcodes_emits_header(self):
        out, _, _ = self.run_slop(
            make_program("delve tapestry"), extra_args=("--opcodes",)
        )
        self.assertIn("opcodes", out)
        self.assertIn("ops", out)

    def test_opcodes_renders_plus_and_dot(self):
        # delve = +, tapestry = .
        out, _, _ = self.run_slop(
            make_program("delve tapestry"), extra_args=("--opcodes",)
        )
        self.assertIn("+", out)
        self.assertIn(".", out)

    def test_opcodes_inlines_string_literals(self):
        body = "“abc”"
        out, _, _ = self.run_slop(
            make_program(body), extra_args=("--opcodes",)
        )
        self.assertIn('."abc"', out)

    def test_opcodes_renders_brackets(self):
        # The loop preamble + closer phrases become [ and ].
        # Body uses one inc verb, one loop preamble, one dec, one closer
        # — keeps the repetition window clean.
        body = (
            "delve it's worth noting that however this is not just tapestry"
        )
        out, _, _ = self.run_slop(
            make_program(body), extra_args=("--opcodes",)
        )
        self.assertIn("[", out)
        self.assertIn("]", out)

    def test_opcodes_short_form(self):
        out, _, _ = self.run_slop(
            make_program("delve tapestry"), extra_args=("-o",)
        )
        self.assertIn("opcodes", out)


class CombinedFlagsTests(SlopfuckTest):
    """Both flags compose; stripped output precedes opcodes output."""

    def test_both_flags_together(self):
        out, _, code = self.run_slop(
            make_program("delve tapestry"), extra_args=("-s", "-o")
        )
        self.assertEqual(code, 0)
        # Both sections appear, in order.
        stripped_idx = out.find("stripped program")
        opcodes_idx = out.find("opcodes")
        self.assertGreaterEqual(stripped_idx, 0)
        self.assertGreater(opcodes_idx, stripped_idx)


if __name__ == "__main__":
    unittest.main(verbosity=2)
