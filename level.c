#include "base.h"
#include "register.h"
#include "copy16.h"
#include "type.h"
#include "color.h"
#include "level/0.level.h"

/*
  bitfield

   3  2  1  0
  -- -- -- --
   P

 */

u32 level_graph[1][32] __attribute__ ((section (".bss"))) = {0};

void level_init(void)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 2) = RGB15(31, 31, 31);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 4) = RGB15(23, 31, 23);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 6) = RGB15(4, 4, 4);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 8) = RGB15(15, 15, 15);

  for (u32 i = 0; i < 2; i++) {
    u16 value = (i + 1);
    value = (value << 12) | (value << 8) | (value << 4) | (value << 0);
    fill_16((void *)(VRAM + CHARACTER_BASE_BLOCK(0) + (i * 8 * 8 / 2)),
            value,
            (8 * 8 / 2));
  }

  for (u32 r = 0; r < (32 * 4); r++) {
    u32 v = ((u32 *)(&_binary_level_0_level_start))[r];

    u32 y = (r / 4);
    u32 y32 = y * 32;

    for (u32 q = 0; q < 8; q++) {
      u32 x = q + (8 * (r % 4));
      u32 nib = (v >> (q * 4));

      ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(31)))[y32 + x] =
        ( SCREEN_TEXT__PALETTE(0)
        | (nib & 0x7)
        );

      level_graph[0][y] |= ((nib >> 3) & 1) << x;
    }
  }

}
