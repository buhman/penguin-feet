#include "base.h"
#include "register.h"
#include "copy16.h"
#include "type.h"
#include "color.h"
#include "level/0.level.h"

void level_init(void)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 2) = RGB15(23, 23, 23);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(0) + 4) = RGB15(8, 8, 8);

  for (u32 i = 0; i < 2; i++) {
    u16 value = (i + 1);
    value = (value << 12) | (value << 8) | (value << 4) | (value << 0);
    fill_16((void *)(VRAM + CHARACTER_BASE_BLOCK(0) + (i * 8 * 8 / 2)),
            value,
            (8 * 8 / 2));
  }

  for (u32 y = 0; y < 32; y++) {
    u32 v = ((u32 *)(&_binary_level_0_level_start))[y];
    for (u32 x = 0; x < 32; x++) {
      ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(31)))[y * 32 + x] =
        ( SCREEN_TEXT__PALETTE(0)
        | ((v >> x) & 1)
        );
    }
  }
}
