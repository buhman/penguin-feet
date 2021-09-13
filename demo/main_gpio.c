#include "base.h"
#include "register.h"
#include "type.h"

static unsigned int state = 0;

#define RNCT_MASK (0b1111111111110000)

void _user_isr(void)
{
  *(volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(31) + ((32 * 19) + 29) * 2) ^= 1;

  *(volatile u16 *)(IO_REG + IME) = 0;

  u32 latch;
  u32 clock;

  if (state == 0 || state == 1)
    latch = 1;
  else
    latch = 0;

  if (state > 1)
    clock = (~state) & 1;
  else
    clock = 1;

  if (state > 1 && clock == 0) {
    u32 bit = (state >> 1) - 1U;
    u16 data = *(volatile u16 *)(IO_REG + RCNT);
    data = (data >> RCNT__SD_DATA) & 1;

    *(volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(31) + bit * 2) = data;
  }

  state++;
  if (state >= 34)
    state = 0;

  *(volatile u16 *)(IO_REG + RCNT) = (u16)
    ( RCNT__GPIO
    | RCNT__SO_OUTPUT_ENABLE
    | RCNT__SC_OUTPUT_ENABLE
    | (clock << RCNT__SC_DATA)
    | (latch << RCNT__SO_DATA)
    );

  *(volatile u16 *)(IO_REG + IF) = IE__TIMER_0;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;
  return;
}

void _main(void)
{
  *(volatile u16 *)(IO_REG + IME) = 0;

  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 2) = 0x7fff;

  for (int i = 1; i < (8 * 8 / 2); i++)
    *(volatile u8 *)(VRAM + CHARACTER_BASE_BLOCK(0) + 32 + i) = 0x11;

  *(volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(31) + ((32 * 19) + 29) * 2) = 1;

  *(volatile u16 *)(IO_REG + BG0CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(0)
    | BG_CNT__SCREEN_BASE_BLOCK(31)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile u16 *)(IO_REG + DISPCNT) =
    ( DISPCNT__BG0
    | DISPCNT__BG_MODE_0
    );

  /* */


  *(volatile u32 *)(IWRAM_USER_ISR) = (u32)(&_user_isr);

  *(volatile u16 *)(IO_REG + TM0CNT_L) = (u16)-400;

  *(volatile u16 *)(IO_REG + TM0CNT_H) =
    ( TM_CNT_H__ENABLE
    | TM_CNT_H__INT_ENABLE
    | TM_CNT_H__PRESCALAR_64
    );
  *(volatile u16 *)(IO_REG + IE) = IE__TIMER_0;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  while (1) {
    *(volatile u8 *)(IO_REG + 0x300 + 1) = 0;
  }
}

void _start(void)
{
  while (1) _main();
}
