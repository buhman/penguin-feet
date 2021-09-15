#include "assert.h"
#include "base.h"
#include "register.h"
#include "color.h"
#include "copy16.h"

#include "ucs.h"
#include "heap.h"

static_assert(UCS_GRAPH_ROWS == 32 && UCS_GRAPH_COLS == 32);
extern u32 level_graph[1][32];
static value_t graph_path[UCS_GRAPH_AREA];

void path_debug_init(void)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 2) = RGB15(0, 23, 0);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 4) = RGB15(23, 0, 23);
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(1) + 6) = RGB15(23, 0, 0);

  for (u32 i = 0; i < 4; i++) {
    u16 value = i;
    value = (value << 12) | (value << 8) | (value << 4) | (value << 0);
    fill_16((void *)(VRAM + CHARACTER_BASE_BLOCK(1) + (i * 8 * 8 / 2)),
            value,
            (8 * 8 / 2));
  }
}

void path_debug_update(u32 x, u32 y, u32 screen)
{
  fill_16((void *)(VRAM + SCREEN_BASE_BLOCK(screen)),
          0,
          SCREEN_BASE_BLOCK_LENGTH / 2);

  value_t path[UCS_GRAPH_AREA];
  value_t source = UCS_XY_VALUE(x, y);
  value_t target = UCS_XY_VALUE(8, 8);

  u32 * graph = &level_graph[0][0];

  ucs(graph, source, &path[0]);

  value_t w = target;
  while (w != (value_t)-1) {
    u32 w_x = w & 31;
    u32 w_y = w / 32;

    u32 color = w == target ? 1 :
                w == source ? 3 :
                2;

    ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(screen)))[w_y * 32 + w_x] =
      ( SCREEN_TEXT__PALETTE(1)
      | color
      );

    if (w == source)
      break;

    w = path[w];
  }
}
