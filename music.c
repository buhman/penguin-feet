#include "type.h"
#include "base.h"
#include "register.h"
#include "character.h"

#include "music/voice.h"
#include "music.h"

typedef struct {
  u32 start;
  u32 size;
  u32 offset;
  u32 step;
} state_t;

static state_t voice[2] = {
  { (u32)&_binary_music_passacaglia_voice_0_dfreq_start,
    (u32)&_binary_music_passacaglia_voice_0_dfreq_size,
    0,
    0 },

  { (u32)&_binary_music_passacaglia_voice_1_dfreq_start,
    (u32)&_binary_music_passacaglia_voice_1_dfreq_size,
    0,
    0 },
};

void music_init(void)
{
  *(volatile u16 *)(IO_REG + SOUNDCNT_X) =
    ( SOUNDCNT_X__ENABLE
    | SOUNDCNT_X__ENABLE_1
    | SOUNDCNT_X__ENABLE_2
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_L) =
    ( SOUNDCNT_L__OUTPUT_1_L
    | SOUNDCNT_L__OUTPUT_1_R
    | SOUNDCNT_L__OUTPUT_2_L
    | SOUNDCNT_L__OUTPUT_2_R
    | SOUNDCNT_L__OUTPUT_LEVEL_L(5)
    | SOUNDCNT_L__OUTPUT_LEVEL_R(5)
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_H) =
    ( SOUNDCNT_H__OUTPUT_RATIO_FULL
    );

  *(volatile u16 *)(IO_REG + SOUND1_CNT_L) =
    ( SOUND1_CNT_L__SWEEP_TIME(0)
    | SOUND1_CNT_L__SWEEP_DECREASE
    | SOUND1_CNT_L__SWEEP_SHIFTS(0)
    );
}

static u8 reg_offset[2][2] = {
  {SOUND1_CNT_H, SOUND1_CNT_X},
  {SOUND2_CNT_L, SOUND2_CNT_H}
};

static u8 _step = 0;

void music_step(void)
{
  _step++;
  if (_step == 15) {
    _step = 0;
  } else return;

  for (int vi = 0; vi < 2; vi++) {
    u32 step = voice[vi].step;
    voice[vi].step = step + 1;
    u32 offset = voice[vi].offset;

    u32 data = ((u16 *)(voice[vi].start))[offset];
    u32 duration = data >> 11;
    if (step + 1 == duration) {
      voice[vi].offset = offset + 1;
      voice[vi].step = 0;
    }
    if (step != 0)
      continue;

    /* */

    u32 frequency = data & 0x7ff;

    *(volatile u16 *)(IO_REG + reg_offset[vi][0]) =
      ( SOUND1_CNT_H__ENVELOPE_VALUE(frequency == 0 ? 0 : 12)
      | SOUND1_CNT_H__ENVELOPE_STEPS(0)
      | SOUND1_CNT_H__DUTY_CYCLE(2)
      );

    *(volatile u16 *)(IO_REG + reg_offset[vi][1]) =
      ( SOUND1_CNT_X__FREQUENCY_DATA(frequency)
      | SOUND1_CNT_X__RESTART
      );
  }
}
