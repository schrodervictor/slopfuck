#!/usr/bin/env python3
"""
slopfuck test runner — discovers and runs every test_*.py module in
this directory.

Run with: python3 tests/run_tests.py
       or: make test
"""

import os
import sys
import unittest

TEST_DIR = os.path.dirname(os.path.abspath(__file__))


def main() -> int:
    # Make the shared _base module importable from the test files.
    if TEST_DIR not in sys.path:
        sys.path.insert(0, TEST_DIR)
    loader = unittest.TestLoader()
    suite = loader.discover(start_dir=TEST_DIR, pattern="test_*.py")
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    return 0 if result.wasSuccessful() else 1


if __name__ == "__main__":
    sys.exit(main())
