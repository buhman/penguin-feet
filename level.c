#include "base.h"
#include "register.h"
#include "copy16.h"
#include "type.h"
#include "color.h"
#include "level/0.level.h"
#include "assert.h"

#include "level.h"

/*
  bitfield

   3  2  1  0
  -- -- -- --
   P

 */


#define CHARACTERS 4

void level_init(u32 * pathable)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 2) = RGB15(31, 31, 31);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 4) = RGB15(23, 31, 23);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 6) = RGB15(4, 4, 4);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 8) = RGB15(15, 15, 15);

  for (u32 i = 1; i < CHARACTERS + 1; i++) {
    u16 value = i;
    value = (value << 12) | (value << 8) | (value << 4) | (value << 0);
    fill_16((void *)( VRAM
                    + CHARACTER_BASE_BLOCK(LEVEL_CHARACTER_BASE_BLOCK)
                    + LEVEL_CHARACTER_BLOCK_OFFSET
                    + (i * (8 * 8 / 2))),
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

      ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(LEVEL_SCREEN_BASE_BLOCK)))[y32 + x] =
        ( SCREEN_TEXT__PALETTE(LEVEL_PALETTE)
        | ((nib + 1) & 0x7)
        );

      pathable[y] |= ((nib >> 3) & 1) << x;
    }
  }

}

void level_counter_init(const u32 * pathable, u32 * visited, u32 * unvisited_count)
{
  u32 unvisited = 0;
  for (u32 i = 0; i < 32; i++) {
    u32 row = pathable[i];
    visited[i] = row;
    for (u32 j = 0; j < 32; j++) {
      if (((row >> j) & 1) == 0) unvisited++;
    }
  }
  *unvisited_count = unvisited;
}

void level_counter_decrement(const u32 q, const u32 r, u32 * visited, u32 * unvisited_count)
{
  u32 unvisited = *unvisited_count;
  for (u32 ri = r; ri < (r + 2); ri++) {
    u32 row = visited[ri];
    for (u32 qi = q; qi < (q + 2); qi++) {
      if (((row >> qi) & 1) == 0)
        unvisited--;
      visited[ri] |= (1 << qi);
    }
  }
  *unvisited_count = unvisited;
}
