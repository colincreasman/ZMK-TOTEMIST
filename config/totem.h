/*                        TOTEM KEY POSITIONS

  Maps urob's logical 34-key layout onto the TOTEM's physical matrix. The
  TOTEM adds two bottom-row outer pinky keys (positions 20 & 31) and a third
  thumb per hand relative to urob's base; those extras are wired up in the
  ZMK_BASE_LAYER macro (see totem.keymap).

  ╭────────────────────────────╮ ╭────────────────────────────╮
  │  0   1   2   3   4         │ │         5   6   7   8   9   │
  │ 10  11  12  13  14         │ │        15  16  17  18  19   │
  │ 20  21  22  23  24  25     │ │    26  27  28  29  30  31   │
  ╰─────────╮ 32  33  34       │ │    35  36  37 ╭─────────────╯
            ╰──────────────────╯ ╰───────────────╯

  ╭─────────────────────╮ ╭─────────────────────╮
  │ LT4 LT3 LT2 LT1 LT0 │ │ RT0 RT1 RT2 RT3 RT4 │
  │ LM4 LM3 LM2 LM1 LM0 │ │ RM0 RM1 RM2 RM3 RM4 │
  │ LB4 LB3 LB2 LB1 LB0 │ │ RB0 RB1 RB2 RB3 RB4 │  (+ LX0 / RX0 outer)
  ╰───────╮ LH2 LH1 LH0 │ │ RH0 RH1 RH2 ╭───────╯
          ╰─────────────╯ ╰─────────────╯                        */

#pragma once

#define LT0  4  // left-top row
#define LT1  3
#define LT2  2
#define LT3  1
#define LT4  0

#define RT0  5  // right-top row
#define RT1  6
#define RT2  7
#define RT3  8
#define RT4  9

#define LM0 14  // left-middle row
#define LM1 13
#define LM2 12
#define LM3 11
#define LM4 10

#define RM0 15  // right-middle row
#define RM1 16
#define RM2 17
#define RM3 18
#define RM4 19

#define LB0 25  // left-bottom row
#define LB1 24
#define LB2 23
#define LB3 22
#define LB4 21

#define RB0 26  // right-bottom row
#define RB1 27
#define RB2 28
#define RB3 29
#define RB4 30

#define LH0 34  // left thumb keys
#define LH1 33
#define LH2 32

#define RH0 35  // right thumb keys
#define RH1 36
#define RH2 37

#define LX0 20  // left  bottom-outer pinky (TOTEM extra)
#define RX0 31  // right bottom-outer pinky (TOTEM extra)

#define KEYS_L LT0 LT1 LT2 LT3 LT4 LM0 LM1 LM2 LM3 LM4 LB0 LB1 LB2 LB3 LB4 LX0
#define KEYS_R RT0 RT1 RT2 RT3 RT4 RM0 RM1 RM2 RM3 RM4 RB0 RB1 RB2 RB3 RB4 RX0
#define THUMBS_L LH0 LH1 LH2
#define THUMBS_R RH0 RH1 RH2
#define THUMBS THUMBS_L THUMBS_R
