#include "assert.h"
#include "base.h"
#include "register.h"
#include "color.h"
#include "copy16.h"
#include "character.h"

#include "ucs.h"
#include "heap.h"

void path_debug_init(void)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 2) = RGB15(0, 23, 0);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 4) = RGB15(23, 0, 23);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 6) = RGB15(23, 0, 0);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 8) = RGB15(23, 23, 0);

  for (u32 i = 0; i < 5; i++) {
    u16 value = i;
    value = (value << 12) | (value << 8) | (value << 4) | (value << 0);
    fill_16((void *)(VRAM + CHARACTER_BASE_BLOCK(1) + (i * 8 * 8 / 2)),
            value,
            (8 * 8 / 2));
  }
}

void path_debug_update(const value_t source, const value_t target,
                       const value_t * path)
{
  value_t w = target;
  while (w != (value_t)-1) {
    u32 w_x = w & 31;
    u32 w_y = w / 32;

    u32 color = w == target ? 1 :
                w == source ? 3 :
                2;

    ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(PATH_DEBUG_SCREEN_BASE_BLOCK)))[w_y * 32 + w_x] =
      ( SCREEN_TEXT__PALETTE(1)
      | color
      );

    if (w == source)
      break;

    w = path[w];
  }
}
