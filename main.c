#include "assert.h"
#include "actor.h"
#include "background.h"
#include "base.h"
#include "bee.h"
#include "character.h"
#include "copy16.h"
#include "footprint.h"
#include "graph.h"
#include "heap.h"
#include "interactable.h"
#include "level.h"
#include "log.h"
#include "music.h"
#include "obj.h"
#include "palette.h"
#include "path_debug.h"
#include "register.h"
#include "tile.h"
#include "type.h"
#include "ucs.h"
#include "glyph.h"

static value_t path[UCS_GRAPH_AREA];
static u32 pathable[32];
static u8 interactable[32 * 32];
static u32 printable[32];
static u32 visited[32];
static u32 unvisited_count;

#define DPAD_RIGHT KEYCNT__INPUT_RIGHT
#define DPAD_LEFT  KEYCNT__INPUT_LEFT
#define DPAD_UP    KEYCNT__INPUT_UP
#define DPAD_DOWN  KEYCNT__INPUT_DOWN

#define PENGUIN_Q 3
#define PENGUIN_R 1

static actor_t penguin = {
  .x = PENGUIN_Q * 8,
  .y = PENGUIN_R * 8,
  .target.q = PENGUIN_Q,
  .target.r = PENGUIN_R,
  .neg.q = 0,
  .neg.r = 0,
};

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

typedef enum {
  ZERO,
  TITLE,
  PRE_LEVEL,
  LEVEL,
  //POST_LEVEL,
  LAST_STATE
} state_t;

static state_t state = ZERO;

static state_t transitions[LAST_STATE] = {
  [ZERO] = TITLE,
  [TITLE] = PRE_LEVEL,
  [PRE_LEVEL] = LEVEL,
  [LEVEL] = PRE_LEVEL,
};

static s8 level = -1;

static void next_level(void)
{
  level++;
  level_init(level, &pathable[0], &printable[0]);
  level_reset_penguin(level, &penguin);
  level_counter_init(&printable[0], &visited[0], &unvisited_count);
  footprint_init();
  interactable_init(&pathable[0], &interactable[0]);
  log_init(&pathable[0], &interactable[0]);
}

#define ARRAY_LENGTH(_a) ((sizeof (_a)) / (sizeof ((_a)[0])))

static u8 animals[] = {36, 49, 44, 48, 36, 47, 54};

// use your footprints
static u8 level0_0[] =
  {30, 28, 14, 255, 34, 24, 30, 27, 255, 15, 24, 24, 29, 25, 27, 18, 23, 29, 28};
// fill the path
static u8 level0_1[] =
  {15, 18, 21, 21, 255, 29, 17, 14, 255, 25, 10, 29, 17};

// hide
static u8 level1_0[] =
  {17, 18, 13, 14};

// trust the rainbow
static u8 level1_1[] =
  {29, 27, 30, 28, 29, 255, 29, 17, 14, 255, 27, 10, 18, 23, 11, 24, 32};

// thanks for playing
static u8 thanks[] =
  {29, 17, 10, 23, 20, 28, 255, 15, 24, 27, 255, 25, 21, 10, 34, 18, 23, 16};


typedef struct {
  u8 * m0;
  u8 * m1;
  u8 m0_length;
  u8 m1_length;
} level_message_t;

static level_message_t level_messages[] = {
  [0] = { &level0_0[0], &level0_1[0], ARRAY_LENGTH(level0_0), ARRAY_LENGTH(level0_1) },
  [1] = { &level1_0[0], &level1_1[0], ARRAY_LENGTH(level1_0), ARRAY_LENGTH(level1_1) },
  [LEVEL_LAST] = { &thanks[0], 0, ARRAY_LENGTH(thanks), 0 },
};

static void next_state(void)
{
_next_state:
  state = transitions[state];

  switch (state) {
  case TITLE:
    glyph_draw_line(2, animals, ARRAY_LENGTH(animals));

    *(volatile u16 *)(IO_REG + DISPCNT) =
      ( DISPCNT__BG3
      | DISPCNT__BG_MODE_0
      );
    break;
  case PRE_LEVEL:
    glyph_draw_line(2, level_messages[level + 1].m0,
                       level_messages[level + 1].m0_length);
    glyph_draw_line(6, level_messages[level + 1].m1,
                       level_messages[level + 1].m1_length);
    *(volatile u16 *)(IO_REG + DISPCNT) =
      ( DISPCNT__BG3
      | DISPCNT__BG_MODE_0
      );
    break;
  case LEVEL:
    if (level < (LEVEL_LAST - 1)) {
      next_level();
    } else {
      state = ZERO;
      level = -1;
      goto _next_state;
    }
    *(volatile u16 *)(IO_REG + DISPCNT) =
      ( DISPCNT__BG0
      | DISPCNT__BG1
      | DISPCNT__BG2
      | DISPCNT__OBJ
      | DISPCNT__OBJ_1_DIMENSION
      | DISPCNT__BG_MODE_0
      );
    break;
  default:
    break;
  }
}

static u32 wait_for = 0;

static void wait_for_a()
{
  u32 key_input = ~(*(volatile u16 *)(IO_REG + KEY_INPUT));

  if (key_input & KEYCNT__INPUT_A) {
    if (!(wait_for & KEYCNT__INPUT_A))
      next_state();
    wait_for |= KEYCNT__INPUT_A;
  } else {
    wait_for &= ~KEYCNT__INPUT_A;
  }
}

static void game_step()
{
  log_step(&pathable[0], &interactable[0]);

  /* */

  u32 key_input = ~(*(volatile u16 *)(IO_REG + KEY_INPUT));

  u32 penguin_q = (penguin.x >> 3) + (penguin.neg.q && (penguin.x & 0b111));
  u32 penguin_r = (penguin.y >> 3) + (penguin.neg.r && (penguin.y & 0b111));

  value_t source = UCS_QR_VALUE(penguin_q, penguin_r);
  ucs(&pathable[0], source, &path[0]);
  bee_step(source, &path[0]);

  s8 dx;
  s8 dy;
  {
    u8 xz = (penguin.x & 0b111) == 0;
    u8 yz = (penguin.y & 0b111) == 0;
    dx = yz
       ? ((key_input & KEYCNT__INPUT_LEFT) ? -1 : 0) + ((key_input & KEYCNT__INPUT_RIGHT) ? 1 : 0)
       : 0;
    dy = xz
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

  s32 dq;
  s32 dr;
  actor_move_rpath(penguin_q, penguin_r, source, &path[0], &penguin, &dq, &dr);
  obj_update(OBJ_PENGUIN_ATTRIBUTE, penguin.x, penguin.y);

  {
    if ((dq != 0 || dr != 0) && (penguin.x & 0b111) == 0 && (penguin.y & 0b111) == 0) {
      const s32 dir = dr < 0 ? 0 : // NORTH
                      dq > 0 ? 1 : // EAST
                      dr > 0 ? 2 : // SOUTH
                      3;           // WEST

      const u32 _q = penguin.x >> 3;
      const u32 _r = penguin.y >> 3;
      footprint_place(&printable[0], _q, _r, dir);
      level_counter_decrement(_q, _r, &visited[0], &unvisited_count);

      if (unvisited_count == 0)
        next_state();
    } else if ((dx != 0 || dy != 0) && (!dx != !dy)) {
      const u32 _q = penguin_q + dx;
      const u32 _r = penguin_r + dy;
      if (IN_BOUNDS(_q, _r)) {
        // we tried to move, but hit a wall; there might be an interactable here
        interactable_call(penguin_q + dx, penguin_r + dy, &interactable[0]);
      }
    }
  }
}

void _user_isr(void)
{
  *(volatile u16 *)(IO_REG + IME) = 0;

  u32 ireq = *(volatile u16 *)(IO_REG + IF);
  if ((ireq & IE__TIMER_0) != 0) {
    ireq = IE__TIMER_0;
    music_step();
  } else {
    ireq = IE__V_BLANK;
    switch (state) {
    case TITLE:
    case PRE_LEVEL:
      wait_for_a();
      break;
    case LEVEL:
      game_step();
      break;
    default:
      break;
    }
  }

  /* */
  *(volatile u16 *)(IO_REG + IF) = ireq;
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

  obj_init();
  glyph_init();
  path_debug_init(); // palette 1, screen 30+29
  music_init();

  /* initialize graph_path with -1 */
  ucs((void *)0, (value_t)-1, &path[0]);

  *(volatile u16 *)(IO_REG + BG0CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(LEVEL_CHARACTER_BASE_BLOCK)
    | BG_CNT__SCREEN_BASE_BLOCK(LEVEL_SCREEN_BASE_BLOCK)
    | BG_CNT__PRIORITY(3)
    );

  *(volatile u16 *)(IO_REG + BG1CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(1)
    | BG_CNT__SCREEN_BASE_BLOCK(30)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile u16 *)(IO_REG + BG2CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(FOOTPRINT_CHARACTER_BASE_BLOCK)
    | BG_CNT__SCREEN_BASE_BLOCK(FOOTPRINT_SCREEN_BASE_BLOCK)
    | BG_CNT__PRIORITY(2)
    );

  *(volatile u16 *)(IO_REG + BG3CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(GLYPH_CHARACTER_BASE_BLOCK)
    | BG_CNT__SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)
    | BG_CNT__PRIORITY(0)
    );

  *(volatile u32 *)(IWRAM_USER_ISR) = (u32)(&_user_isr);

  *(volatile u16 *)(IO_REG + DISPSTAT) = DISPSTAT__V_BLANK_INT_ENABLE;

  *(volatile u16 *)(IO_REG + TM0CNT_L) = (u16)-3581;

  *(volatile u16 *)(IO_REG + TM0CNT_H) =
    ( TM_CNT_H__ENABLE
    //| TM_CNT_H__INT_ENABLE
    | TM_CNT_H__PRESCALAR_64
    );

  *(volatile u16 *)(IO_REG + IE) =
    ( IE__V_BLANK
    | IE__TIMER_0
    );
  *(volatile u16 *)(IO_REG + IF) = (u16)-1;

  //

  next_state();

  //

  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  while (1) {
    *(volatile u8 *)(IO_REG + HALTCNT) = 0;
  }
}

void _start(void)
{
  while (1) _main();
}
