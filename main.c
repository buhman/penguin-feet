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
#include "footprint.h"
#include "copy16.h"

#include "ucs.h"
#include "heap.h"

static value_t path[UCS_GRAPH_AREA];
static u32 pathable[32];
static u32 visited[32];
static u32 unvisited_count;

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
  .x = 4 * 8,
  .y = 9 * 8,
  .target.q = 4,
  .target.r = 9,
  .neg.q = 0,
  .neg.r = 0,
};

static u8 screen = 30;

#define IN_BOUNDS(_q, _r) ((_q) >= 0 && (_q) <= 31 && (_r) >= 0 && (_r) <= 31)

#define IN_GRAPH_PATH(_a, _q1, _r1, _q2, _r2, _q3, _r3) \
  {                     \
    value_t q1 = (_q1); \
    value_t r1 = (_r1); \
    value_t q2 = (_q2); \
    value_t r2 = (_r2); \
    value_t q3 = (_q3); \
    value_t r3 = (_r3); \
                                 \
    if (q1 == _a.target.q && r1 == _a.target.r); \
    else if (q2 == _a.target.q && r2 == _a.target.r); \
    else if (q3 == _a.target.q && r3 == _a.target.r); \
    else if (IN_BOUNDS(q1, r1) && (path[UCS_QR_VALUE(q1, r1)]) != (value_t)-1) { \
      _a.target.q = q1;                                \
      _a.target.r = r1;                                \
    } else if (IN_BOUNDS(q2, r2) && (path[UCS_QR_VALUE(q2, r2)]) != (value_t)-1) { \
      _a.target.q = q2;                                       \
      _a.target.r = r2;                                       \
    } else if (IN_BOUNDS(q3, r3) && (path[UCS_QR_VALUE(q3, r3)]) != (value_t)-1) { \
      _a.target.q = q3;                                       \
      _a.target.r = r3;                                       \
    }                                                         \
  }

static void next_level(void)
{
  level_init(&pathable[0]);
  level_counter_init(&pathable[0], &visited[0], &unvisited_count);
  footprint_init();
}

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

  value_t source = UCS_QR_VALUE(penguin_q, penguin_r);
  ucs(&pathable[0], source, &path[0]);

  {
    u8 xz = (penguin.x & 0b111) == 0;
    u8 yz = (penguin.y & 0b111) == 0;
    s8 dx = yz
          ? ((key_input & KEYCNT__INPUT_LEFT) ? -1 : 0) + ((key_input & KEYCNT__INPUT_RIGHT) ? 1 : 0)
          : 0;
    s8 dy = xz
          ? ((key_input & KEYCNT__INPUT_UP) ? -1 : 0) + ((key_input & KEYCNT__INPUT_DOWN) ? 1 : 0)
          : 0;

    if (dx != 0 && dy != 0) {
      /*  ___
          _OX
          _XX  */
      IN_GRAPH_PATH(penguin, penguin_q + dx, penguin_r + dy,
                             penguin_q + dx, penguin_r +  0,
                             penguin_q +  0, penguin_r + dy
                             );

    } else if (dx != 0) {
      /*  __X
          _OX
          __X  */
      IN_GRAPH_PATH(penguin, penguin_q + dx, penguin_r +  0,
                             penguin_q + dx, penguin_r +  (yz),
                             penguin_q + dx, penguin_r + -(yz)
                             );

    } else if (dy != 0) {
      /*  ___
          _O_
          XXX  */
      IN_GRAPH_PATH(penguin, penguin_q +  0, penguin_r + dy,
                             penguin_q +  (xz), penguin_r + dy,
                             penguin_q + -(xz), penguin_r + dy
                             );
    }
  }

  value_t target = UCS_QR_VALUE(penguin.target.q, penguin.target.r);

  s32 dq;
  s32 dr;
  {
    value_t w = target;
    value_t next_w = target;
    while (w != (value_t)-1 && w != source) {
      next_w = w;
      w = path[w];
    }
    (void)next_w;

    screen = (screen == 30) ? 29 : 30;
    path_debug_update(source, target, screen, &path[0]);

    u32 next_w_q = next_w & 31;
    u32 next_w_r = next_w / 32;

    dq = next_w_q - penguin_q;
    dr = next_w_r - penguin_r;
    penguin.neg.q = (dq < 0);
    penguin.neg.r = (dr < 0);
    penguin.x += dq;
    penguin.y += dr;

    penguin_update(penguin.x, penguin.y);
  }

  {
    if ((dq != 0 || dr != 0) && (penguin.x & 0b111) == 0 && (penguin.y & 0b111) == 0) {
      const s32 dir = dr < 0 ? 0 : // NORTH
                      dq > 0 ? 1 : // EAST
                      dr > 0 ? 2 : // SOUTH
                      3;           // WEST

      const u32 _q = penguin.x >> 3;
      const u32 _r = penguin.y >> 3;
      footprint_place(_q, _r, dir);
      level_counter_decrement(_q, _r, &visited[0], &unvisited_count);

      if (unvisited_count == 0)
        next_level();
    }
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

  // transparency character
  fill_16((void *)(VRAM + CHARACTER_BASE_BLOCK(0)),
          0,
          (8 * 8 / 2));

  next_level();

  penguin_init();
  //background_init();
  path_debug_init(); // palette 1, screen 30+29

  /* initialize graph_path with -1 */
  ucs((void *)0, (value_t)-1, &path[0]);

  *(volatile u16 *)(IO_REG + BG0CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(LEVEL_CHARACTER_BASE_BLOCK)
    | BG_CNT__SCREEN_BASE_BLOCK(LEVEL_SCREEN_BASE_BLOCK)
    | BG_CNT__PRIORITY(2)
    );

  *(volatile u16 *)(IO_REG + BG1CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(1)
    | BG_CNT__SCREEN_BASE_BLOCK(30)
    | BG_CNT__PRIORITY(0)
    );

  *(volatile u16 *)(IO_REG + BG2CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(FOOTPRINT_CHARACTER_BASE_BLOCK)
    | BG_CNT__SCREEN_BASE_BLOCK(FOOTPRINT_SCREEN_BASE_BLOCK)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile u16 *)(IO_REG + DISPCNT) =
    ( DISPCNT__BG0
    | DISPCNT__BG1
    | DISPCNT__BG2
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
