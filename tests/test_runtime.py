"""
Runtime tests: brainfuck-equivalent execution semantics, the bundled
example programs, and the quality of compile-error diagnostics.
"""

import os
import subprocess
import unittest

from _base import (
    BINARY,
    INTRO,
    OUTRO,
    PRAISE_FILLER,
    ROOT,
    SlopfuckTest,
    make_program,
)


# ── Execution semantics ─────────────────────────────────────────────


# Pool of varied increment keywords for tests that need exactly N
# distinct increment ops without triggering the multiplier patterns.
_INC_POOL = [
    "delve", "foster", "nurture", "cultivate", "elevate",
    "leverage", "spearhead", "streamline", "optimize", "enhance",
    "amplify", "accelerate", "empower", "unlock", "unleash",
    "transform", "innovate", "pioneer", "champion", "harness",
    "maximize", "bolster", "strengthen", "fortify", "enrich",
    "galvanize", "propel", "catalyze", "ignite", "inspire",
    "invigorate", "revitalize", "energize", "synergize",
    "harmonize", "orchestrate", "curate", "craft", "architect",
    "forge", "sculpt", "refine", "polish", "hone", "sharpen",
    "calibrate", "tailor", "personalize", "reimagine", "redefine",
    "reinvent", "rethink", "reshape", "overhaul", "modernize",
    "digitize", "automate", "integrate", "consolidate", "unify",
    "align", "converge", "bridge", "interweave", "intertwine",
    "fuse",
]


class ExecutionTests(SlopfuckTest):
    def test_string_literal_hello_world(self):
        body = "“Hello, World!”"
        out, _, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0)
        self.assertEqual(out, "Hello, World!")

    def test_increment_and_output_produces_byte(self):
        # 65 increments → ASCII 'A'. Use varied increment keywords.
        body = " ".join(_INC_POOL[:65]) + " tapestry"
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "A")

    def test_pointer_right_moves_to_fresh_cell(self):
        # Increment cell 0 to 'B' (66), move right to cell 1 (still 0),
        # output cell 1 → null byte.
        body = " ".join(_INC_POOL[:66]) + " — tapestry"
        out, _, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0)
        self.assertEqual(out, "\x00")

    def test_loop_zeros_a_cell(self):
        # Classic brainfuck `[-]` idiom: while nonzero, decrement.
        # Increment to ~10, then loop-decrement, then output (=0).
        body = (
            "delve foster nurture cultivate elevate "
            "leverage spearhead streamline optimize enhance "
            "it's worth noting that however this is not just "
            "tapestry"
        )
        out, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertEqual(out, "\x00")

    def test_compiled_op_count_reported(self):
        # Stderr should announce op count.
        body = "delve tapestry"
        _, err, code = self.run_slop(make_program(body))
        self.assertEqual(code, 0, err)
        self.assertIn("compiled:", err)
        self.assertIn("operations", err)


# ── Bundled example regression ──────────────────────────────────────


class ExampleRegressionTests(SlopfuckTest):
    """The shipped examples must continue to compile and run."""

    def _run_example(self, name: str, expected_substring: str):
        path = os.path.join(ROOT, "examples", name)
        if not os.path.exists(path):
            self.skipTest(f"{name} example not found")
        result = subprocess.run(
            [BINARY, path],
            capture_output=True,
            text=True,
            timeout=10,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn(expected_substring, result.stdout)

    def test_hello_example_runs(self):
        self._run_example("hello.slop", "Hello, World!")

    def test_alphabet_example_runs(self):
        self._run_example("alphabet.slop", "Output: ABCDE!")

    def test_fluid_example_runs(self):
        self._run_example("fluid.slop", "Hello, World!")


# ── Error message quality ──────────────────────────────────────────


class ErrorMessageTests(SlopfuckTest):
    """The compiler's diagnostics participate in the slop. Verify."""

    def test_intro_error_lists_examples(self):
        src = f"random opening. {PRAISE_FILLER} {OUTRO}"
        _, err, _ = self.run_slop(src)
        self.assertIn("brilliant idea", err.lower())

    def test_praise_error_suggests_words(self):
        body = "the cat sat on the mat " * 10
        src = f"{INTRO} {body} {OUTRO}"
        _, err, _ = self.run_slop(src)
        self.assertIn("brilliant", err.lower())

    def test_straight_quote_error_mentions_curly(self):
        body = '"oops"'
        _, err, _ = self.run_slop(make_program(body))
        self.assertIn("curly", err.lower())


if __name__ == "__main__":
    unittest.main(verbosity=2)
