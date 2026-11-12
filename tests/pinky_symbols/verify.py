#!/usr/bin/env python3
"""Exercise the production combos and effective HID modifiers in native ZMK."""

import os
from pathlib import Path
import re
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[2]
zmk = Path(os.environ["ZMK_SRC_DIR"])
source = (repo / "config/totem.keymap").read_text()

# Append diagnostic controls without replacing any production binding.
controls = "&mo 1 &mo 2 &mo 3 &kp LSHFT &kp RSHFT &kp X &kp LCTRL"
for layer in ("layer_zero", "layer_one", "layer_two", "layer_three"):
    pattern = rf"({layer}\s*\{{.*?bindings\s*=\s*<)(.*?)(>;\s*\}};)"
    source, count = re.subn(
        pattern, lambda m: m[1] + m[2] + controls + m[3], source, flags=re.S
    )
    assert count == 1, layer

events = []
expected = []


def event(pressed, position, delay):
    action = "PRESS" if pressed else "RELEASE"
    events.append(f"ZMK_MOCK_{action}({position // 10},{position % 10},{delay})")


def tap(position):
    event(True, position, 40)
    event(False, position, 300)


global_combos = [
    ((20, 21), 0x2D, 0),  # Z+X -> -
    ((28, 29), 0x2E, 0),  # period+slash -> =
    ((10, 11), 0x2D, 2),  # A+S -> _
    ((18, 19), 0x2E, 2),  # L+backslash -> +
    ((22, 23), 0x26, 2),  # C+V -> (
    ((26, 27), 0x27, 2),  # M+comma -> )
    ((12, 13), 0x2F, 0),  # D+F -> [
    ((16, 17), 0x30, 0),  # J+K -> ]
    ((2, 3), 0x2F, 2),  # E+R -> {
    ((6, 7), 0x30, 2),  # U+I -> }
]

# Verify left, right, and both Shifts, as well as an unrelated modifier.
for layer in range(4):
    if layer:
        event(True, 35 + layer, 300)
    for modifier_keys, modifier_bits in (
        ((), 0), ((39,), 2), ((40,), 0x20), ((39, 40), 0x22),
        ((39, 42), 3),
    ):
        for key in modifier_keys:
            event(True, key, 100)
        for positions, keycode, symbol_shift in global_combos:
            # Test either press/release order, including Shift restoration.
            for first, second in (positions, positions[::-1]):
                event(True, first, 10)
                event(True, second, 40)
                event(False, first, 10)
                event(False, second, 300)
                expected.append((keycode, (modifier_bits & ~0x22) | symbol_shift))
                tap(41)
                expected.append((0x1B, modifier_bits))
        for key in reversed(modifier_keys):
            event(False, key, 300)
    if layer:
        event(False, 35 + layer, 300)

# Enter remains comma+period.
event(True, 27, 10)
event(True, 28, 40)
event(False, 27, 10)
event(False, 28, 300)
expected.append((0x28, 0))

# The actual Shift home-row keys and Tab thumb also work as Shift sources.
for shift_key, modifier in ((10, 2), (19, 0x20), (32, 2)):
    event(True, shift_key, 250)
    event(True, 22, 10)
    event(True, 23, 40)
    event(False, 22, 10)
    event(False, 23, 300)
    expected.append((0x26, 2))
    tap(41)
    expected.append((0x1B, modifier))
    event(False, shift_key, 300)

# Fast typing followed by thumb Shift must capitalize A as reliably as T.
for position, keycode in ((10, 0x04), (4, 0x17), (19, 0x31)):
    event(True, 41, 20)
    event(False, 41, 10)
    expected.append((0x1B, 0))
    event(True, 32, 20)
    event(True, position, 30)
    event(False, position, 10)
    event(False, 32, 300)
    expected.append((keycode, 2))

fixture = (
    "#include <dt-bindings/zmk/kscan_mock.h>\n" + source
    + "\n&kscan { rows = <5>; columns = <10>; events = <\n"
    + "\n".join(events) + "\n>; };\n"
)

with tempfile.TemporaryDirectory(prefix="fixed-combos-") as directory:
    directory = Path(directory)
    config = directory / "config"
    config.mkdir()
    (config / "native_posix_64.keymap").write_text(fixture)
    build = directory / "build"
    result = subprocess.run(
        ["west", "build", "-s", str(zmk), "-d", str(build), "-b", "native_posix_64",
         "--", f"-DZMK_CONFIG={config}", f"-DZMK_EXTRA_MODULES={repo}",
         "-DCONFIG_ASSERT=y"],
        text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    )
    if result.returncode:
        raise SystemExit(result.stdout)
    result = subprocess.run(
        [str(build / "zephyr/zmk.exe")], text=True, stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT, check=True,
    )
    actual = []
    pending = None
    for line in result.stdout.splitlines():
        match = re.search(r"hid_listener_keycode_pressed:.*keycode 0x([0-9A-Fa-f]+)", line)
        if match:
            keycode = int(match[1], 16)
            pending = keycode if keycode < 0xE0 else None
        match = re.search(r"hid_implicit_modifiers_press: Modifiers set to 0x([0-9A-Fa-f]+)", line)
        if match and pending is not None:
            actual.append((pending, int(match[1], 16)))
            pending = None
    if actual != expected:
        for index, (want, got) in enumerate(zip(expected, actual)):
            if want != got:
                print(f"First mismatch at {index}: expected {want}, got {got}")
                break
        print(f"Expected {len(expected)} events, got {len(actual)}")
        raise SystemExit(result.stdout)
    print(f"PASS: {len(expected)} symbol/probe events; all layers, both Shift keys, "
          "Ctrl preserved, Shift restored, both combo orders")
