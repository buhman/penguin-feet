#include "base.h"
#include "register.h"
#include "copy16.h"
#include "type.h"
#include "color.h"
#include "level/level.h"
#include "assert.h"
#include "character.h"
#include "actor.h"
#include "level.h"

/*
  bitfield

   3  2  1  0
  -- -- -- --
   P

 */

typedef struct {
  void * start;
  unsigned int size;
  u8 penguin_q;
  u8 penguin_r;
} level_t;


#define CHARACTERS 4

static const level_t levels[LEVEL_LAST] = {
  /*
  [0] = {
    (void *)&_binary_level_0_level_start,
    (u32)&_binary_level_0_level_size,
  },
  */
  [0] = {
    (void *)&_binary_level_2_level_start,
    (u32)&_binary_level_2_level_size,
    0,
    8,
  },
  [1] = {
    (void *)&_binary_level_1_level_start,
    (u32)&_binary_level_1_level_size,
    2,
    1,
  },
};

void level_reset_penguin(const u32 level, actor_t * penguin)
{
  penguin->x = levels[level].penguin_q * 8;
  penguin->y = levels[level].penguin_r * 8;
  penguin->target.q = levels[level].penguin_q;
  penguin->target.r = levels[level].penguin_r;
  penguin->neg.q = 0;
  penguin->neg.r = 0;
}

void level_init(const u32 level, u32 * pathable, u32 * printable)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 2) = RGB15(31, 31, 31); // white
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 4) = RGB15(31, 30, 20); // yellow
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 6) = RGB15(23, 31, 23); // green
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 8) = RGB15( 0, 22, 24);  // teal
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(LEVEL_PALETTE) + 10) = RGB15( 4, 4, 4); // black

  for (u32 i = 1; i < CHARACTERS + 1; i++) {
    u32 value = i;
    value = (value << 28) | (value << 24) | (value << 20) | (value << 16)
          | (value << 12) | (value <<  8) | (value <<  4) | (value <<  0);
    fill_32((void *)( VRAM
                    + CHARACTER_BASE_BLOCK(LEVEL_CHARACTER_BASE_BLOCK)
                    + LEVEL_CHARACTER_BLOCK_OFFSET
                    + (i * (8 * 8 / 2))),
            value,
            (8 * 8 / 2));
  }

  fill_32((void *)pathable, 0, 32 * 4);
  fill_32((void *)printable, 0, 32 * 4);

  for (u32 r = 0; r < (32 * 4); r++) {
    u32 v = ((u32 *)(levels[level].start))[r];

    u32 y = (r / 4);
    u32 y32 = y * 32;

    for (u32 q = 0; q < 8; q++) {
      u32 x = q + (8 * (r % 4));
      u32 nib = (v >> (q * 4));

      ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(LEVEL_SCREEN_BASE_BLOCK)))[y32 + x] =
        ( SCREEN_TEXT__PALETTE(LEVEL_PALETTE)
        | (LEVEL_CHARACTER_OFFSET + ((nib & 0xf) == 0b1001 ? 5 : ((nib & 0x3) + 1)))
        );

      pathable[y] |= ((nib >> 3) & 1) << x;
      printable[y] |= ((nib >> 2) & 1) << x;
    }
  }
}

void level_counter_init(const u32 * printable, u32 * visited, u32 * unvisited_count)
{
  u32 unvisited = 0;
  for (u32 i = 0; i < 32; i++) {
    u32 row = ~(printable[i]);
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
