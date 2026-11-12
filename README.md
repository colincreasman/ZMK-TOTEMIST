This repo is the firmware for the Totem and Totemist sold at https://ergomech.store


# `urob-qwerty` branch: urob's keymap in QWERTY

This branch runs [urob's ZMK keymap](https://github.com/urob/zmk-config) (upstream commit `1331b1a`) on the
36-key Totemist. Everything is kept as upstream: the timeless, bilateral home row mods, layers (Nav, Fn, Num,
Sys, Mouse), combos, leader key, smart-num, magic shift, comma/dot morphs and all timings. The only change is
the base layer's alphas, which are QWERTY instead of his Colemak-DH-like layout.

![Keymap](docs/keymap.png)

- Files: [config/base.keymap](config/base.keymap), [config/combos.dtsi](config/combos.dtsi),
  [config/leader.dtsi](config/leader.dtsi) and [config/mouse.dtsi](config/mouse.dtsi) are copied from urob with
  only the three alpha rows edited. [config/totem.keymap](config/totem.keymap) is the Totemist wrapper; like
  urob's 36-key Corneish Zen it puts `Gui` on the outer left thumb and `Smart Mouse` on the outer right thumb.
- Modules: urob's `zmk-helpers`, `zmk-auto-layer`, `zmk-adaptive-key`, `zmk-leader-key`, `zmk-tri-state` and
  `zmk-unicode` are pinned to their `v0.3` tags to match this repo's ZMK `v0.3.0` (urob builds on ZMK `main`).
- QWERTY details: `'` sits on the right pinky home key (Miryoku's QWERTY placement), `,` `.` keep urob's
  morphs (`;` and `:` on shift), and the bottom-right pinky key is urob's `?`/`!` key.
- **Known issue:** the bottom-right pinky key (position 29) is dead on this hardware, so `?` and `!` have no
  working key; `/` is still on its combo.
- Shortcuts are urob's Linux ones (e.g. Copy is `Ctrl+Ins`, desktops are `Ctrl+Gui+Arrow`); the unicode
  leader sequences need the macOS "Unicode Hex Input" source or Linux.
- Bootloader: hold `Enter` (Fn) and the right-thumb `Smart`/Num key for the Sys layer, then `G`/`H` reboot that
  half into its bootloader. This branch has no Totemist maintenance layer, so in dongle mode the dongle's
  bootloader is only reachable by its reset button.
- Sleep is not enabled (urob uses `CONFIG_ZMK_SLEEP`), because [config/totem.conf](config/totem.conf) also
  configures the dongle, which must never sleep.
- Redraw the diagram with [keymap-drawer](https://github.com/caksoylar/keymap-drawer) and
  [docs/keymap-drawer.yaml](docs/keymap-drawer.yaml) (urob's drawing config): see the comment at its top.
- Background reading on this layout: [Miryoku](https://github.com/manna-harbour/miryoku),
  [bilateral home row mods](https://sunaku.github.io/home-row-mods.html) and
  [urob's layout discussion](https://github.com/urob/zmk-config/discussions/103).

# Dongle mode (PandaKB USB dongle)

The keyboard can also run through [PandaKB's ZMK dongle](https://pandakb.com/shop/keyboard-kit/pandakb-zmk-split-keyboard-dongle/)
(a nice!nano v2 with a 1.3" OLED). The dongle becomes the split **central** and both halves become its
**peripherals**: plug it into a computer and the keyboard just works as a USB keyboard, with no Bluetooth pairing
on that computer. ZMK fixes each part's role at build time, so in dongle mode the halves cannot connect to a
computer without the dongle, and switching modes means reflashing. Both sets of firmware are built.

| Artifact | Flash to | Mode |
|---|---|---|
| `totem_dongle` | dongle | dongle |
| `totem_left_dongle_mode` | left half | dongle |
| `totem_right-seeeduino_xiao_ble-zmk` | right half | **both** (the right half is a peripheral either way) |
| `totem_left-seeeduino_xiao_ble-zmk` | left half | standalone |
| `settings_reset_dongle` | dongle | reset (nice!nano board) |
| `settings_reset-seeeduino_xiao_ble-zmk` | either half | reset (XIAO board) |

**One-time setup:** turn off other ZMK keyboards nearby, flash the matching settings reset to all three devices
(standalone-mode bonds must be cleared first), then flash the dongle-mode firmware, plug in the dongle, and power
on both halves. To go back, reset the halves and flash the standalone left firmware.

- Keymap changes then only need the **dongle** reflashed. On this branch the dongle's bootloader needs its
  reset button (see above).
- The dongle has room for all four Bluetooth profiles, so it can also pair to computers wirelessly on its own
  battery.
- Layers are named so the dongle's OLED can show them. The dongle never deep-sleeps: deep sleep is only woken by
  a key press, and it has no keys.
- The `pandakb_dongle` shield holds the OLED wiring, copied from PandaKB's own dongle firmware;
  `totem_dongle` is the board-agnostic dongle itself.
