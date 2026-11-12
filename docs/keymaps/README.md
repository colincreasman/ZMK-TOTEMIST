# Experimental keymaps

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
