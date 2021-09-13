#include "base.h"
#include "register.h"

#include "palette.h"
#include "tile.h"

#include "level.h"
#include "penguin.h"
#include "background.h"
#include "graph.h"

#define DPAD_RIGHT KEYCNT__INPUT_RIGHT
#define DPAD_LEFT  KEYCNT__INPUT_LEFT
#define DPAD_UP    KEYCNT__INPUT_UP
#define DPAD_DOWN  KEYCNT__INPUT_DOWN

enum heading {
  DIR_NONE = 0,
  DIR_RIGHT = 1,
  DIR_LEFT = 2,
  DIR_UP = 4,
  DIR_DOWN = 8,
};

struct penguin {
  unsigned short x;
  unsigned short y;
  unsigned char heading;
};

static struct penguin penguin = {
  .x = 16,
  .y = 16,
  .heading = DIR_NONE,
};

void _user_isr(void)
{
  *(volatile unsigned short *)(IO_REG + IME) = 0;

  /* */

  /*
  key_input = ~(*(volatile unsigned short *)(IO_REG + KEY_INPUT));

  unsigned int next_heading = 0;

  if      ((key_input & DPAD_RIGHT) && !(key_input & DPAD_LEFT)) next_heading |= DIR_RIGHT;
  else if  (key_input & DPAD_LEFT)                               next_heading |= DIR_LEFT;

  if      ((key_input & DPAD_UP) && !(key_input & DPAD_DOWN))    next_heading |= DIR_UP;
  else if  (key_input & DPAD_DOWN)                               next_heading |= DIR_DOWN;

  if (next_heading == DIR_NONE) {
    if (((penguin.x & 0b111) == 0) && ((penguin.y & 0b111) == 0))
      penguin.heading = DIR_NONE;
    else
      next_heading = penguin.heading;
  }

  unsigned int next_x = penguin.x;
  unsigned int next_y = penguin.y;
  if (next_heading & DIR_RIGHT) next_x += 1;
  if (next_heading & DIR_LEFT)  next_x -= 1;
  if (next_heading & DIR_UP)    next_y -= 1;
  if (next_heading & DIR_DOWN)  next_y += 1;

  if (next_heading != DIR_NONE) {
    if (graph_pathable(next_x, next_y)) {
      penguin.x = next_x;
      penguin.y = next_y;
      penguin.heading |= next_heading;
    } else if (graph_pathable(next_x, penguin.y)) {
      penguin.x = next_x;
      penguin.heading |= next_heading & 0b0011;
    } else if (graph_pathable(penguin.x, next_y)) {
      penguin.y = next_y;
      penguin.heading |= next_heading & 0b1100;
    }
    penguin_update(penguin.x, penguin.y);
  }
  */

  *(volatile unsigned short *)(IO_REG + IF) = IE__V_BLANK;
  *(volatile unsigned short *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  return;
}

void _main(void)
{
  //maze_init();
  level_init();
  penguin_init();
  background_init();

  *(volatile unsigned short *)(IO_REG + BG0CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(0)
    | BG_CNT__SCREEN_BASE_BLOCK(31)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile unsigned short *)(IO_REG + BG1CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(1)
    | BG_CNT__SCREEN_BASE_BLOCK(30)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile unsigned short *)(IO_REG + DISPCNT) =
    ( DISPCNT__BG0
    | DISPCNT__BG1
    | DISPCNT__OBJ
    | DISPCNT__OBJ_1_DIMENSION
    | DISPCNT__BG_MODE_0
    );

  *(volatile unsigned int *)(IWRAM_USER_ISR) = (unsigned int)(&_user_isr);

  *(volatile unsigned short *)(IO_REG + DISPSTAT) = DISPSTAT__V_BLANK_INT_ENABLE;
  *(volatile unsigned short *)(IO_REG + IE) = IE__V_BLANK;
  *(volatile unsigned short *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  while (1) {}
}

void _start(void)
{
  while (1) _main();
}
