# Experimental keymaps

The shipped [config/totem.keymap](../../config/totem.keymap) now mirrors the
Toucan2 keymap tagged `stable-2026-11-12-hrm-caps-word`, minus the Toucan's
outer pinky columns, trackpad layer and OS-mode keys (those only drive
trackpad gestures). Compared with v2 below, `,+.` no longer gives Enter, L1
puts `1`–`3`, `4`–`6` (on home-row mods) and `7`–`0` on the left hand with
arrows on I/J/K/L, and shifted A/backslash taps go through the Caps Word
double-tap like on the Toucan. The maintenance layer puts Bluetooth clear
on K.

These are Totemist adaptations of the two Toucan keymap experiments. Both keep
the Escape combos and maintenance layer. V2 uses Toucan commit `4d3364d`'s
balanced home-row mods, with a shorter hold threshold and prior-idle gate for D.

The previously faulty right pinky switch has been repaired. V2 uses
backslash/Shift on the right home-row pinky, slash below it, and semicolon/GUI
on the outer right thumb (Shift for colon). L1 provides semicolon on N and
quote on M.

- **v1:** Enter is on L1+A. See [SVG](experiment-v1.svg), [PNG](experiment-v1.png),
  or [keymap source](experiment-v1.keymap).
- **v2:** On every layer, `C+V` and `M+,` give `(` and `)`;
  `D+F` and `J+K` give `[` and `]`; `E+R` and `U+I` give `{` and `}`.
  `A+S`, `L+\`, `Z+X`, and `.+/` give `_`, `+`, `-`, and `=`.
  These symbol combos ignore held Shift, so their output never changes with
  Shift. `,+.` still gives Enter.
  Key names refer to physical L0 positions. Combos have a 25ms timeout.
  See [SVG](experiment-v2.svg) or [PNG](experiment-v2.png).

Both use L1 for `1`–`0` along Q–P and HJKL arrows. The left middle
thumb taps Enter and holds L1. The right inner thumb remains Space/L1; the
right middle thumb remains Backspace. The far outer thumbs still activate the
maintenance layer together within 100ms. In v2, those thumbs tap quote and
semicolon and hold GUI independently; on L1 the left outer thumb taps grave
instead. The left inner thumb taps Tab and holds
Shift. Double-tap the base-layer A/Shift or backslash/Shift key for Caps Word;
consecutive taps within 200ms activate it instead of typing two characters.
Shifted taps bypass that wait so quick Shift+A and Shift+backslash chords
emit while Shift is still held.
