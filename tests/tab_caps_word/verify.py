#!/usr/bin/env python3
"""Run with ZMK_SRC_DIR set to a prepared ZMK app workspace."""

import os
from pathlib import Path
import re
import subprocess
import sys

test_dir = Path(__file__).resolve().parent
repo = test_dir.parents[1]
fixture = (test_dir / "native_posix_64.keymap").read_text()
keymaps = [repo / "config/totem.keymap"]
for keymap in keymaps:
    source = keymap.read_text()
    for label, name in (("ht", "hold_tap"), ("a_caps_dance", "a_caps_dance"),
                        ("backslash_caps_dance", "backslash_caps_dance"),
                        ("a_tap", "a_tap"), ("backslash_tap", "backslash_tap"),
                        ("shift_a", "shift_a"), ("shift_backslash", "shift_backslash")):
        pattern = rf"{label}:\s*{name}\s*\{{[^{{}}]*\}};"
        actual = re.search(pattern, source)
        expected = re.search(pattern, fixture)
        if not actual or not expected or actual.group() != expected.group():
            sys.exit(f"{name} in {keymap} differs from the native test")
    if any(binding not in source for binding in (
        "&ht LSHFT TAB", "&shift_a LSHFT 0", "&shift_backslash RSHFT 0"
    )):
        sys.exit(f"Tab/Shift thumb binding missing from {keymap}")

zmk = os.environ.get("ZMK_SRC_DIR")
if not zmk:
    sys.exit("Set ZMK_SRC_DIR to the ZMK app directory")
subprocess.run([str(Path(zmk) / "run-test.sh"), str(test_dir)], check=True)
