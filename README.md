This repo is the firmware for the Totem and Totemist sold at https://ergomech.store

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

- Keymap changes ([config/totem.keymap](config/totem.keymap)) then only need the **dongle** reflashed. To reach
  its bootloader without opening its case, hold both far outer thumbs (maintenance layer) and hold `T` for 2
  seconds. `Q`/`P` still bootloader each half.
- The dongle has room for all four Bluetooth profiles, so it can also pair to computers wirelessly on its own
  battery.
- Layers are named so the dongle's OLED can show them. The dongle never deep-sleeps: deep sleep is only woken by
  a key press, and it has no keys.
- The `pandakb_dongle` shield holds the OLED wiring, copied from PandaKB's own dongle firmware;
  `totem_dongle` is the board-agnostic dongle itself.
