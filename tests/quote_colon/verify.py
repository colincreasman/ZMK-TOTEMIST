#!/usr/bin/env python3
"""Run with ZMK_SRC_DIR set to a prepared ZMK app workspace."""

import os
from pathlib import Path
import re
import subprocess
import sys

test_dir = Path(__file__).resolve().parent
repo = test_dir.parents[1]
source = (repo / "config/totem.keymap").read_text()
fixture = (test_dir / "native_posix_64.keymap").read_text()

for pattern in (r"&lt\s*\{[^{}]*\};", r"ht:\s*hold_tap\s*\{[^{}]*\};"):
    actual = re.search(pattern, source)
    expected = re.search(pattern, fixture)
    if not actual or not expected or actual.group() != expected.group():
        sys.exit(f"Thumb behavior differs from the native test: {pattern}")

for binding in ("&lt L1 RET", "&ht RGUI SEMI", "&ht LGUI SQT", "&ht LGUI GRAVE"):
    if binding not in source:
        sys.exit(f"Thumb binding missing: {binding}")

zmk = os.environ.get("ZMK_SRC_DIR")
if not zmk:
    sys.exit("Set ZMK_SRC_DIR to the ZMK app directory")
subprocess.run([str(Path(zmk) / "run-test.sh"), str(test_dir)], check=True)
