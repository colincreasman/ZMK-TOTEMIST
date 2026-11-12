#!/usr/bin/env python3
"""Exercise the actual OS selector and bootloader behavior with host stubs."""

import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
headers = [
    "zephyr/device.h",
    "zephyr/kernel.h",
    "zephyr/init.h",
    "zephyr/logging/log.h",
    "zephyr/settings/settings.h",
    "drivers/behavior.h",
    "zmk/behavior.h",
    "zmk/endpoints.h",
    "zmk/events/position_state_changed.h",
    "zmk/keymap.h",
]

with tempfile.TemporaryDirectory(prefix="totemist-host-tests-") as directory:
    directory = Path(directory)
    for header in headers:
        path = directory / header
        path.parent.mkdir(parents=True, exist_ok=True)
        path.touch()
    for source in ["host_mode", "host_select", "boot_hold"]:
        executable = directory / source
        subprocess.run(
            [
                os.environ.get("CC", "cc"),
                "-std=gnu11",
                "-Wall",
                "-Wextra",
                "-Werror",
                "-Wno-unused-function",
                "-Wno-unused-const-variable",
                "-Wno-unused-variable",
                "-Wno-unused-parameter",
                "-I" + str(directory),
                str(root / "tests" / f"{source}.c"),
                "-o",
                str(executable),
            ],
            check=True,
        )
        subprocess.run([str(executable)], check=True)
