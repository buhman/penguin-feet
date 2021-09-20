#include "base.h"
#include "register.h"
#include "type.h"

static unsigned int state = 0;

#define RNCT_MASK (0b1111111111110000)

void _user_isr(void)
{
  *(volatile u16 *)(IO_REG + IME) = 0;

  *(volatile u16 *)(IO_REG + SOUND4_CNT_H) =
    ( SOUND4_CNT_H__RESTART
    // | SOUND4_CNT_H__SOUND_LENGTH
    | SOUND4_CNT_H__COUNTER_SHIFT_FREQ(1) // 15
    //| SOUND4_CNT_H__COUNTER_7_STEP
    | SOUND4_CNT_H__COUNTER_PRESCALAR(4) // 7
    );

  *(volatile u16 *)(IO_REG + IF) = IE__TIMER_0;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;
}

void _main(void)
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
    | SOUNDCNT_L__OUTPUT_LEVEL_L(7)
    | SOUNDCNT_L__OUTPUT_LEVEL_R(7)
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_H) =
    ( SOUNDCNT_H__OUTPUT_RATIO_FULL
    );

  *(volatile u16 *)(IO_REG + SOUND1_CNT_L) =
    ( SOUND1_CNT_L__SWEEP_TIME(0)
    | SOUND1_CNT_L__SWEEP_DECREASE
    | SOUND1_CNT_L__SWEEP_SHIFTS(0)
    );

  *(volatile u16 *)(IO_REG + SOUND1_CNT_H) =
    ( SOUND1_CNT_H__ENVELOPE_VALUE(12)
    | SOUND1_CNT_H__ENVELOPE_STEPS(0)
    | SOUND1_CNT_H__DUTY_CYCLE(2)
    );

  *(volatile u16 *)(IO_REG + SOUND1_CNT_X) =
    ( SOUND1_CNT_X__FREQUENCY_DATA(1750)
    //| SOUND1_CNT_X__RESTART
    );

  /* */

  *(volatile u32 *)(IO_REG + WAVE_RAM0_L) = 0xfedcba98;
  *(volatile u32 *)(IO_REG + WAVE_RAM1_L) = 0x76543210;
  *(volatile u32 *)(IO_REG + WAVE_RAM2_L) = 0xfedcba98;
  *(volatile u32 *)(IO_REG + WAVE_RAM3_L) = 0x76543210;
  /*
  *(volatile u32 *)(IO_REG + WAVE_RAM0_L) = 0xffffffff;
  *(volatile u32 *)(IO_REG + WAVE_RAM1_L) = 0x00000000;
  *(volatile u32 *)(IO_REG + WAVE_RAM2_L) = 0xffffffff;
  *(volatile u32 *)(IO_REG + WAVE_RAM3_L) = 0x00000000;
  */

  *(volatile u16 *)(IO_REG + SOUND3_CNT_L) =
    ( SOUND3_CNT_L__SOUND_OUTPUT
    | SOUND3_CNT_L__WAVE_BANK(1)
    | SOUND3_CNT_L__WAVE_32_STEP
    );

  *(volatile u16 *)(IO_REG + SOUND3_CNT_H) =
    ( SOUND3_CNT_H__OUTPUT_LEVEL(1)
    | SOUND3_CNT_H__SOUND_LENGTH(0)
    );

  *(volatile u16 *)(IO_REG + SOUND3_CNT_X) =
    ( SOUND3_CNT_X__FREQUNCY_DATA(1750)
    //| SOUND3_CNT_X__RESTART
    );

  /* */

  *(volatile u16 *)(IO_REG + SOUND4_CNT_L) =
    ( SOUND4_CNT_L__ENVELOPE_VALUE(15)
    | SOUND4_CNT_L__ENVELOPE_STEPS(1)
    | SOUND4_CNT_L__SOUND_LENGTH(30)
    );

  *(volatile u16 *)(IO_REG + SOUND4_CNT_H) =
    ( 0//SOUND4_CNT_H__RESTART
    | SOUND4_CNT_H__SOUND_LENGTH
    | SOUND4_CNT_H__COUNTER_SHIFT_FREQ(1) // 15
    //| SOUND4_CNT_H__COUNTER_7_STEP
    | SOUND4_CNT_H__COUNTER_PRESCALAR(4) // 7
    );

  /* */

  *(volatile u32 *)(IWRAM_USER_ISR) = (u32)(&_user_isr);

  *(volatile u16 *)(IO_REG + TM0CNT_L) = (u16)44300;

  *(volatile u16 *)(IO_REG + TM0CNT_H) =
    ( TM_CNT_H__ENABLE
    | TM_CNT_H__INT_ENABLE
    | TM_CNT_H__PRESCALAR_256
    );
  *(volatile u16 *)(IO_REG + IE) = IE__TIMER_0;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  while (1) {
    *(volatile u8 *)(IO_REG + HALTCNT) = 0;
  }
}

void _start(void)
{
  while (1) _main();
}
