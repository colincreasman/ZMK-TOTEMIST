#!/usr/bin/env python3
"""Regenerate the Totemist YAML, SVG and PNG using keymap-drawer and librsvg."""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile

import yaml

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument(
    "--include", action="append", type=Path,
    help="module include directory (repeat for zmk-helpers and zmk-unicode)",
)
args = parser.parse_args()
config = yaml.safe_load((root / "docs/keymap-drawer.yaml").read_text())
if args.include:
    config["parse_config"]["zmk_additional_includes"] = [
        str(path.resolve()) for path in args.include
    ]

with tempfile.TemporaryDirectory(prefix="totemist-draw-") as directory:
    directory = Path(directory)
    config_path = directory / "config.yaml"
    config_path.write_text(yaml.safe_dump(config, sort_keys=False, allow_unicode=True))
    parsed = subprocess.run(
        ["keymap", "-c", str(config_path), "parse", "-z", "config/totem.keymap",
         "--virtual-layers", "Combos"],
        cwd=root, check=True, stdout=subprocess.PIPE, text=True,
    )
    data = yaml.safe_load(parsed.stdout)
    data["layout"] = {"qmk_keyboard": "crkbd/rev1", "layout_name": "LAYOUT_split_3x5_3"}
    for combo in data["combos"]:
        combo["l"] = ["Combos"]
    yaml_path = root / "docs/keymap.yaml"
    yaml_path.write_text(yaml.safe_dump(data, sort_keys=False, allow_unicode=True, width=200))
    svg = subprocess.run(
        ["keymap", "-c", str(config_path), "draw", str(yaml_path)],
        cwd=root, check=True, stdout=subprocess.PIPE, text=True,
    ).stdout
    (root / "docs/keymap.svg").write_text(svg)

    # librsvg does not resolve CSS custom properties; use the light palette.
    colors = {}
    for name, value in re.findall(r"(--color-[a-z]+):\s*(#[0-9a-fA-F]+);", svg):
        colors.setdefault(name, value)
    flat = re.sub(r"var\((--color-[a-z]+)\)", lambda match: colors[match[1]], svg)
    flat_path = directory / "flat.svg"
    flat_path.write_text(flat)
    subprocess.run(
        ["rsvg-convert", "-b", colors["--color-bg"], "-z", "1.5",
         str(flat_path), "-o", str(root / "docs/keymap.png")],
        check=True,
    )
print("Updated docs/keymap.yaml, docs/keymap.svg and docs/keymap.png")
