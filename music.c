#include "type.h"
#include "base.h"
#include "register.h"
#include "character.h"

#include "music/music.h"
#include "music.h"

typedef struct {
  u32 start;
  u32 size;
  u32 offset;
  u16 step;
  u8 sound;
  u8 _padding;
} state_t;

enum {
  SOUND1,
  SOUND2,
  SOUND3,
  SOUND4
};

static u8 voices = 3;
static state_t voice[3] = {
  {
    .start = (u32)&_binary_music_gba_sketch_02v2__part_P1_voice_1_dfreq_start,
    .size = (u32)&_binary_music_gba_sketch_02v2__part_P1_voice_1_dfreq_size,
    .offset = 0,
    .step = 0,
    .sound = SOUND1
  },
  {
    .start = (u32)&_binary_music_gba_sketch_02v2__part_P1_voice_5_dfreq_start,
    .size = (u32)&_binary_music_gba_sketch_02v2__part_P1_voice_5_dfreq_size,
    .offset = 0,
    .step = 0,
    .sound = SOUND3
  },
  {
    .start = (u32)&_binary_music_gba_sketch_02v2__part_P2_voice_1_dfreq_start,
    .size = (u32)&_binary_music_gba_sketch_02v2__part_P2_voice_1_dfreq_size,
    .offset = 0,
    .step = 0,
    .sound = SOUND4
  },
};

void music_init(void)
{
  *(volatile u16 *)(IO_REG + SOUNDCNT_X) =
    ( SOUNDCNT_X__ENABLE
    | SOUNDCNT_X__ENABLE_1
    | SOUNDCNT_X__ENABLE_2
    | SOUNDCNT_X__ENABLE_3
    | SOUNDCNT_X__ENABLE_4
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_L) =
    ( SOUNDCNT_L__OUTPUT_1_L
    | SOUNDCNT_L__OUTPUT_1_R
    | SOUNDCNT_L__OUTPUT_2_L
    | SOUNDCNT_L__OUTPUT_2_R
    | SOUNDCNT_L__OUTPUT_3_L
    | SOUNDCNT_L__OUTPUT_3_R
    | SOUNDCNT_L__OUTPUT_4_L
    | SOUNDCNT_L__OUTPUT_4_R
    | SOUNDCNT_L__OUTPUT_LEVEL_L(5)
    | SOUNDCNT_L__OUTPUT_LEVEL_R(5)
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_H) =
    ( SOUNDCNT_H__OUTPUT_1234_RATIO_FULL
    );

  *(volatile u16 *)(IO_REG + SOUND1_CNT_L) =
    ( SOUND1_CNT_L__SWEEP_TIME(0)
    | SOUND1_CNT_L__SWEEP_DECREASE
    | SOUND1_CNT_L__SWEEP_SHIFTS(0)
    );

  *(volatile u32 *)(IO_REG + WAVE_RAM0_L) = 0xfedcba98;
  *(volatile u32 *)(IO_REG + WAVE_RAM1_L) = 0x76543210;
  *(volatile u32 *)(IO_REG + WAVE_RAM2_L) = 0xfedcba98;
  *(volatile u32 *)(IO_REG + WAVE_RAM3_L) = 0x76543210;

  *(volatile u16 *)(IO_REG + SOUND3_CNT_L) =
    ( SOUND3_CNT_L__SOUND_OUTPUT
    | SOUND3_CNT_L__WAVE_BANK(1)
    | SOUND3_CNT_L__WAVE_32_STEP
    );

  // not setting parameters on 4 prior to restart appears to make the first
  // restart sound "weird"
  *(volatile u16 *)(IO_REG + SOUND4_CNT_H) =
    ( SOUND4_CNT_H__COUNTER_SHIFT_FREQ(1)
    | SOUND4_CNT_H__COUNTER_PRESCALAR(4)
    );
}

static u8 _step = 0;

void music_step(void)
{
  _step++;
  /*
  if (_step == 1) {
    _step = 0;
  } else return;
  */

  for (int vi = 0; vi < voices; vi++) {
    u32 step = voice[vi].step;
    voice[vi].step = step + 1;
    u32 offset = voice[vi].offset;

    u32 duration = *((u8 *)(voice[vi].start + offset));
    if (step + 1 == duration) {
      u32 next_offset = offset + 4;
      if (next_offset >= voice[vi].size) {
        voice[vi].offset = 0;
      } else {
        voice[vi].offset = next_offset;
      }
      voice[vi].step = 0;
    }
    if (step != 0)
      continue;

    /* */
    u32 frequency = *((u16 *)(voice[vi].start + offset + 2));

    u32 sound = voice[vi].sound;
    if (sound == SOUND4) {
      *(volatile u16 *)(IO_REG + SOUND4_CNT_L) =
        ( SOUND4_CNT_L__ENVELOPE_VALUE(frequency == 0 ? 0 : 10)
        | SOUND4_CNT_L__ENVELOPE_STEPS(1)
        | SOUND4_CNT_L__SOUND_LENGTH(30)
        );

      *(volatile u16 *)(IO_REG + SOUND4_CNT_H) =
        ( SOUND4_CNT_H__RESTART
        | SOUND4_CNT_H__COUNTER_SHIFT_FREQ(1)
        | SOUND4_CNT_H__COUNTER_PRESCALAR(4)
        );
    } else if (sound == SOUND3) {
      *(volatile u16 *)(IO_REG + SOUND3_CNT_H) =
        ( SOUND3_CNT_H__OUTPUT_LEVEL(frequency == 0 ? 0 : 1)
        | SOUND3_CNT_H__SOUND_LENGTH(0)
        );

      *(volatile u16 *)(IO_REG + SOUND3_CNT_X) =
        ( SOUND3_CNT_X__FREQUNCY_DATA(frequency)
        | SOUND3_CNT_X__RESTART
        );
    } else { // SOUND1 or SOUND2
      u32 reg1, reg2;
      if (sound == SOUND1) {
        reg1 = SOUND1_CNT_H;
        reg2 = SOUND1_CNT_X;
      } else {
        reg1 = SOUND2_CNT_L;
        reg2 = SOUND2_CNT_H;
      }

      *(volatile u16 *)(IO_REG + reg1) =
        ( SOUND1_CNT_H__ENVELOPE_VALUE(frequency == 0 ? 0 : 9)
        | SOUND1_CNT_H__ENVELOPE_STEPS(0)
        | SOUND1_CNT_H__DUTY_CYCLE(3)
        );

      *(volatile u16 *)(IO_REG + reg2) =
        ( SOUND1_CNT_X__FREQUENCY_DATA(frequency)
        | SOUND1_CNT_X__RESTART
        );
    }
  }
}
