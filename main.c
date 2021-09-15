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

#define DPAD_RIGHT KEYCNT__INPUT_RIGHT
#define DPAD_LEFT  KEYCNT__INPUT_LEFT
#define DPAD_UP    KEYCNT__INPUT_UP
#define DPAD_DOWN  KEYCNT__INPUT_DOWN

enum heading {
  DIR_NONE = 0,
  DIR_RIGHT = 1,
  DIR_LEFT = 2,
  DIR_UP = 3,
  DIR_DOWN = 4,
};

struct actor {
  u16 x;
  u16 y;
  u8 heading;
  u8 next_heading;
};

static struct actor penguin = {
  .x = 0,
  .y = 0,
  .heading = DIR_NONE,
  .next_heading = DIR_NONE,
};

static u8 screen = 30;

void _user_isr(void)
{
  *(volatile u16 *)(IO_REG + IME) = 0;

  /* */

  u32 key_input = ~(*(volatile u16 *)(IO_REG + KEY_INPUT));

  u32 dpad = (key_input >> 4) & 0b1111;

  switch (dpad) {
  case (KEYCNT__INPUT_RIGHT >> 4):
    penguin.next_heading = DIR_RIGHT;
    break;
  case (KEYCNT__INPUT_LEFT >> 4):
    penguin.next_heading = DIR_LEFT;
    break;
  case (KEYCNT__INPUT_UP >> 4):
    penguin.next_heading = DIR_UP;
    break;
  case (KEYCNT__INPUT_DOWN >> 4):
    penguin.next_heading = DIR_DOWN;
    break;
  default:
    penguin.next_heading = penguin.heading;
    break;
  }

  /* */

  u32 next_x = penguin.x;
  u32 next_y = penguin.y;
  switch (penguin.next_heading) {
  case DIR_RIGHT:
    next_x += 1;
    break;
  case DIR_LEFT:
    next_x -= 1;
    break;
  case DIR_UP:
    next_y -= 1;
    break;
  case DIR_DOWN:
    next_y += 1;
    break;
  }

  /*
    if (next_x == penguin.x && next_y == penguin.y)
    goto _end;
  */

  penguin.x = next_x;
  penguin.y = next_y;
  penguin_update(penguin.x, penguin.y);

  screen = (screen == 30) ? 29 : 30;

  path_debug_update(penguin.x >> 3, penguin.y >> 3, screen);

  *(volatile u16 *)(IO_REG + BG1CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(1)
    | BG_CNT__SCREEN_BASE_BLOCK(screen)
    | BG_CNT__PRIORITY(0)
    );

  /* */
_end:
  *(volatile u16 *)(IO_REG + IF) = IE__V_BLANK;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  return;
}

void _main(void)
{
  //maze_init();
  level_init();
  penguin_init();
  //background_init();
  path_debug_init();


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
