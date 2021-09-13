#include "base.h"
#include "register.h"

#include "palette.h"
#include "tile.h"

void _main(void)
{
  palette$basic((void *)(PRAM_BG + PRAM_PALETTE(0)));
  palette$basic((void *)(PRAM_OBJ + PRAM_PALETTE(0)));

  tile$basic((void *)(VRAM_BG + CHARACTER_BASE_BLOCK(0)));
  tile$basic((void *)(VRAM_OBJ));

  *((unsigned short *)(VRAM + SCREEN_BASE_BLOCK(31)) + 0) = 4;
  *((unsigned short *)(VRAM + SCREEN_BASE_BLOCK(31)) + 2) = 5;

  *(volatile unsigned short *)(IO_REG + BG0CNT) =
    ( BG_CNT__COLOR_16_16
    | BG_CNT__SCREEN_SIZE(0)
    | BG_CNT__CHARACTER_BASE_BLOCK(0)
    | BG_CNT__SCREEN_BASE_BLOCK(31)
    | BG_CNT__PRIORITY(1)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__Y_COORDINATE(24)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_16_16)
    | OBJ_A1__X_COORDINATE(24)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 2)) =
    ( OBJ_A2__COLOR_PALETTE(0)
    | OBJ_A2__PRIORITY(0)
    | OBJ_A2__CHARACTER(1)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(1, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__Y_COORDINATE(60)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(1, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_16_16)
    | OBJ_A1__X_COORDINATE(60)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(1, 2)) =
    ( OBJ_A2__COLOR_PALETTE(0)
    | OBJ_A2__PRIORITY(0)
    | OBJ_A2__CHARACTER(4)
    );

  *(volatile unsigned short *)(IO_REG + DISPCNT) =
    ( DISPCNT__BG0
    | DISPCNT__OBJ
    | DISPCNT__OBJ_1_DIMENSION
    | DISPCNT__BG_MODE_0
    );

  while (1) {
  }
}

void _start(void)
{
  while (1) _main();
}
