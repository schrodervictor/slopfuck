"""
Shared infrastructure for the slopfuck test suite.

Provides the SlopfuckTest base class (which runs the compiler against a
temp file and returns stdout/stderr/exit), plus reusable program fragments
that let individual tests focus on one feature.
"""

import os
import subprocess
import sys
import tempfile
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BINARY = os.path.join(tempfile.gettempdir(), "slopfuck-test-bin")

# ── Reusable program fragments ──────────────────────────────────────
# Intro and outro that DON'T contribute praise words, so individual
# tests can control praise count precisely.
INTRO = "This is a fantastic idea!"
OUTRO = "Let me know if you’d like me to dive deeper"

# Filler that pads with praise to comfortably pass the praise check.
PRAISE_FILLER = (
    "Your visionary genius is truly exceptional and your masterful "
    "brilliant outstanding work is phenomenal extraordinary remarkable."
)


def make_program(body: str = "", praise: str = PRAISE_FILLER) -> str:
    """Build a valid-shell slopfuck program around `body`."""
    return f"{INTRO} {praise} {body} {OUTRO}\n"


# ── Docker image / binary management ────────────────────────────────


def extract_binary() -> str:
    """Extract the slopfuck binary from the docker image once per run."""
    if os.path.exists(BINARY):
        os.unlink(BINARY)
    cid = subprocess.check_output(
        ["docker", "create", "slopfuck"], text=True
    ).strip()
    try:
        subprocess.check_call(
            ["docker", "cp", f"{cid}:/usr/local/bin/slopfuck", BINARY]
        )
    finally:
        subprocess.check_call(
            ["docker", "rm", cid], stdout=subprocess.DEVNULL
        )
    os.chmod(BINARY, 0o755)
    return BINARY


def ensure_image() -> None:
    """Build the docker image if it doesn't exist."""
    result = subprocess.run(
        ["docker", "image", "inspect", "slopfuck"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if result.returncode != 0:
        print("Building slopfuck docker image...", file=sys.stderr)
        subprocess.check_call(
            ["docker", "build", "-t", "slopfuck", ROOT],
            stdout=subprocess.DEVNULL,
        )


# ── Base test class ─────────────────────────────────────────────────


class SlopfuckTest(unittest.TestCase):
    """Base class with `run_slop()` helper."""

    @classmethod
    def setUpClass(cls):
        ensure_image()
        # Always re-extract — the docker image may have been rebuilt.
        extract_binary()

    def run_slop(
        self,
        source: str,
        stdin: str = "",
        timeout: int = 10,
        extra_args=(),
    ):
        """Compile + execute a slopfuck source string.

        Returns (stdout, stderr, code) as (str, str, int). Bytes that
        aren't valid UTF-8 are replaced rather than raising.

        Pass `extra_args` to test CLI flags like ("--stripped",) or
        ("--opcodes",) — they precede the filename argument.
        """
        with tempfile.NamedTemporaryFile(
            suffix=".slop", mode="w", encoding="utf-8", delete=False
        ) as f:
            f.write(source)
            path = f.name
        try:
            result = subprocess.run(
                [BINARY, *extra_args, path],
                input=stdin.encode("utf-8") if stdin else None,
                capture_output=True,
                timeout=timeout,
            )
            stdout = result.stdout.decode("utf-8", errors="replace")
            stderr = result.stderr.decode("utf-8", errors="replace")
            return stdout, stderr, result.returncode
        finally:
            os.unlink(path)
