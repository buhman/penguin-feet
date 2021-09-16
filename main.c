#include "base.h"
#include "register.h"

#include "palette.h"
#include "tile.h"

#include "level.h"
#include "penguin.h"
#include "background.h"
#include "graph.h"
#include "path_debug.h"
#include "type.h"

#include "ucs.h"
#include "heap.h"

static value_t path[UCS_GRAPH_AREA];
static u32 pathable[32];

#define DPAD_RIGHT KEYCNT__INPUT_RIGHT
#define DPAD_LEFT  KEYCNT__INPUT_LEFT
#define DPAD_UP    KEYCNT__INPUT_UP
#define DPAD_DOWN  KEYCNT__INPUT_DOWN

typedef struct {
  u16 q;
  u16 r;
} ucs_coord_t;

typedef struct {
  s8 q;
  s8 r;
} neg_t;

typedef struct {
  u16 x;              // screen coordinates
  u16 y;
  ucs_coord_t target; // ucs coordinates
  neg_t neg;
} actor_t;

static actor_t penguin = {
  .x = 5 * 8,
  .y = 0,
  .target.q = 8,
  .target.r = 8,
  .neg.q = 0,
  .neg.r = 0,
};

static u8 screen = 30;

void _user_isr(void)
{
  *(volatile u16 *)(IO_REG + IME) = 0;

  /* */

  *(volatile u16 *)(IO_REG + BG1CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(1)
    | BG_CNT__SCREEN_BASE_BLOCK(screen)
    | BG_CNT__PRIORITY(0)
    );

  /* */

  u32 key_input = ~(*(volatile u16 *)(IO_REG + KEY_INPUT));

  u32 penguin_q = (penguin.x >> 3) + (penguin.neg.q && (penguin.x & 0b111));
  u32 penguin_r = (penguin.y >> 3) + (penguin.neg.r && (penguin.y & 0b111));

  value_t source = UCS_XY_VALUE(penguin_q, penguin_r);
  ucs(&pathable[0], source, &path[0]);
  value_t target = UCS_XY_VALUE(penguin.target.q, penguin.target.r);

  {
    value_t w = target;
    value_t next_w = target;
    while (w != source) {
      next_w = w;
      w = path[w];
    }
    (void)next_w;

    screen = (screen == 30) ? 29 : 30;
    path_debug_update(source, target, screen, &path[0]);

    u32 next_w_q = next_w & 31;
    u32 next_w_r = next_w / 32;

    s32 dq = next_w_q - penguin_q;
    s32 dr = next_w_r - penguin_r;
    penguin.neg.q = (dq < 0);
    penguin.neg.r = (dr < 0);
    penguin.x += dq;
    penguin.y += dr;

    penguin_update(penguin.x, penguin.y);
  }

  /* */
_end:
  *(volatile u16 *)(IO_REG + IF) = IE__V_BLANK;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  return;
}

void _main(void)
{
  //maze_init();
  level_init(&pathable[0]);
  penguin_init();
  //background_init();
  path_debug_init();

  /* initialize graph_path with -1 */
  ucs((void *)0, (value_t)-1, &path[0]);

  *(volatile u16 *)(IO_REG + BG0CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(0)
    | BG_CNT__SCREEN_BASE_BLOCK(31)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile u16 *)(IO_REG + BG1CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(1)
    | BG_CNT__SCREEN_BASE_BLOCK(30)
    | BG_CNT__PRIORITY(0)
    );

  *(volatile u16 *)(IO_REG + DISPCNT) =
    ( DISPCNT__BG0
    | DISPCNT__BG1
    | DISPCNT__OBJ
    | DISPCNT__OBJ_1_DIMENSION
    | DISPCNT__BG_MODE_0
    );

  *(volatile u32 *)(IWRAM_USER_ISR) = (u32)(&_user_isr);

  *(volatile u16 *)(IO_REG + DISPSTAT) = DISPSTAT__V_BLANK_INT_ENABLE;
  *(volatile u16 *)(IO_REG + IE) = IE__V_BLANK;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  while (1) {
    *(volatile u8 *)(IO_REG + HALTCNT) = 0;
  }
}

void _start(void)
{
  while (1) _main();
}
