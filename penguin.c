#include "base.h"
#include "register.h"

#include "copy16.h"
#include "penguin.h"
#include "character/penguin.h"

void penguin_init(void)
{
  copy_16((void *)(PRAM_OBJ + PRAM_PALETTE(0)),
          (void *)&_binary_character_penguin_palette_start,
          (unsigned int)&_binary_character_penguin_palette_size);

  copy_16((void *)(VRAM_OBJ + 32),
          (void *)&_binary_character_penguin_character_start,
          (unsigned int)&_binary_character_penguin_character_size);

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__Y_COORDINATE(240) // offscreen
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_16_16)
    | OBJ_A1__X_COORDINATE(240) // offscreen
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 2)) =
    ( OBJ_A2__COLOR_PALETTE(0)
    | OBJ_A2__PRIORITY(0)
    | OBJ_A2__CHARACTER(1)
    );
}

void penguin_update(unsigned int x, unsigned int y)
{
  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__Y_COORDINATE(y)
    );

  *(volatile unsigned short *)(OAM + OAM_OBJ_ATTRIBUTE(0, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_16_16)
    | OBJ_A1__X_COORDINATE(x)
    );
}
