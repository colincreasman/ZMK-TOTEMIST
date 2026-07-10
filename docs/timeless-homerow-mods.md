# Timeless Homerow Mods

This document describes the change that made the totem's homerow mods (`HRML`
and `HRMR`) "timeless", mirroring the approach used in
[urob's zmk-config](https://github.com/urob/zmk-config).

The homerow mods (`HRML` / `HRMR`) and the `&ht` thumb hold-taps were made
timeless. Layers, combos, and the physical layout are untouched. This change was
made in `config/totem.keymap`, which is the active keymap (it overrides the
shield default in `config/boards/shields/totem/totem.keymap`).

## What "timeless" means

Naive homerow mods (HRMs) depend on precise timing: hold longer than
`tapping-term-ms` for a modifier, release faster than it for a tap. That
requires very consistent typing speed and leads to misfires. A "timeless" setup
removes the dependence on precise timing by combining four ZMK features:

1. **`balanced` flavor + a large `tapping-term-ms` (280ms)** — the behavior
   becomes insensitive to exact timing. `balanced` resolves a *hold* as soon as
   another key is both pressed and released within the tapping term, which is
   exactly what you do when using a modifier.
2. **`require-prior-idle-ms` (150ms)** — if the HRM key is pressed shortly after
   another key (i.e. mid-typing), it resolves immediately as a *tap*. This
   eliminates the delay between pressing an alpha and seeing it on screen.
3. **`hold-trigger-key-positions` (cross-hand + thumbs)** — "positional
   hold-tap". A modifier is only produced when the *next* key is on the opposite
   hand (or a thumb). Same-hand rolls therefore resolve as taps, preventing
   false modifiers.
4. **`hold-trigger-on-release`** — delays the positional decision until the next
   key is *released* rather than pressed, so multiple modifiers on the same hand
   can still be combined.

## Implementation

Because each hand needs a different set of cross-hand trigger positions, the
single shared `&ht` behavior was replaced (for the homerow only) with two
dedicated behaviors: `hml` (left hand) and `hmr` (right hand). The `HRML` /
`HRMR` macros now point at these instead of `&ht`, so the per-layer bindings did
not need to change.

The `&ht` behavior — still used by the three thumb hold-taps (`&ht LSHFT TAB`,
`&ht LG(C) ESC`, `&ht RG(V) ESC`) — was given a timeless treatment adapted for
thumb keys (see [Thumb keys](#thumb-keys) below).

### Behavior settings

| Property                     | Old (`ht`)                  | New (`hml` / `hmr`)          | Why |
| ---------------------------- | --------------------------- | ---------------------------- | --- |
| `flavor`                     | `balanced`                  | `balanced`                   | Resolve hold on cross-key press+release |
| `tapping-term-ms`            | 220                         | **280**                      | Large term → insensitive to precise timing |
| `quick-tap-ms`               | 150                         | **175**                      | Matches urob's value |
| `require-prior-idle-ms`      | — (used `global-quick-tap`) | **150**                      | Instant tap when typing fast → removes delay |
| `hold-trigger-key-positions` | —                           | **cross-hand keys + thumbs** | Positional hold-tap → no false mods on same-hand rolls |
| `hold-trigger-on-release`    | —                           | **enabled**                  | Lets same-hand mods still be combined |

### Key-position map

The totem is a 38-key board with an extra outer column on each side of the
bottom row and three thumbs per side. The trigger positions were derived from
the matrix transform in `config/boards/shields/totem/totem.dtsi`:

```
Row 0:        0  1  2  3  4   |  5  6  7  8  9
Row 1:       10 11 12 13 14   | 15 16 17 18 19
Row 2:  20  21 22 23 24 25    | 26 27 28 29 30  31
Thumbs:         32 33 34      | 35 36 37
```

Positions `20` and `31` are the extra outer bottom-row keys (left and right
respectively). Which gives the macros:

```c
#define KEYS_L 0 1 2 3 4 10 11 12 13 14 20 21 22 23 24 25   // left-hand keys
#define KEYS_R 5 6 7 8 9 15 16 17 18 19 26 27 28 29 30 31   // right-hand keys
#define THUMBS 32 33 34 35 36 37                            // thumb keys
```

- `hml` uses `hold-trigger-key-positions = <KEYS_R THUMBS>` — a left-hand HRM
  only produces a modifier when the next key is on the right hand or a thumb.
- `hmr` uses `hold-trigger-key-positions = <KEYS_L THUMBS>` — the mirror image.

## Thumb keys

Three thumb keys use the `&ht` behavior: `&ht LSHFT TAB` (shift / tab),
`&ht LG(C) ESC` (copy / esc) and `&ht RG(V) ESC` (paste / esc). They get the
timeless *timing* — `balanced` flavor with a large `tapping-term-ms` of 280 and
`quick-tap-ms` of 175 — but two of the HRM features are deliberately left off:

- **No cross-hand `hold-trigger-key-positions`.** The shift must capitalize
  letters on *both* hands, and the copy/paste holds are used freely alongside
  either hand, so restricting them to one side would break them.
- **No `require-prior-idle-ms` (and `global-quick-tap` was removed).** These
  resolve the key as a *tap* when pressed shortly after another key. On a thumb
  hold that means a false tap (e.g. `tab`/`esc`) when used immediately after
  typing — the exact false-negative urob's README warns about for shift keys.

This matches urob's own approach of using dedicated shift/thumb holds that omit
`require-prior-idle-ms`.

| Property                     | Old (`ht`) | New (`ht`, thumb) |
| ---------------------------- | ---------- | ----------------- |
| `flavor`                     | `balanced` | `balanced`        |
| `tapping-term-ms`            | 220        | **280**           |
| `quick-tap-ms`               | 150        | **175**           |
| `global-quick-tap`           | enabled    | **removed**       |
| `require-prior-idle-ms`      | —          | — (intentional)   |
| `hold-trigger-key-positions` | —          | — (intentional)   |

## Net effect

Fast same-hand rolls resolve as taps (no accidental modifiers), while genuine
cross-hand chords still produce modifiers — with virtually no typing delay and
no reliance on precise hold/tap timing.

## Reference

- urob's writeup on timeless homerow mods: <https://github.com/urob/zmk-config>
- ZMK hold-tap docs: <https://zmk.dev/docs/keymaps/behaviors/hold-tap>
