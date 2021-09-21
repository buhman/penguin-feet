#include "base.h"
#include "register.h"
#include "type.h"

#include "copy16.h"
#include "obj.h"
#include "character/penguin.h"
#include "character/bee.h"

struct ss {
  struct {
    void * start;
    u32 size;
  } palette;
  struct {
    void * start;
    u32 size;
  } character;
};

const static struct ss obj[] = {
  { { (void *)&_binary_character_penguin_palette_start
    , (u32)&_binary_character_penguin_palette_size },
    { (void *)&_binary_character_penguin_character_start
    , (u32)&_binary_character_penguin_character_size }
  },
  { { (void *)&_binary_character_bee_palette_start
    , (u32)&_binary_character_bee_palette_size },
    { (void *)&_binary_character_bee_character_start
    , (u32)&_binary_character_bee_character_size }
  }
};

#define ARRAY_LENGTH(_a) ((sizeof (_a)) / (sizeof ((_a)[0])))

void obj_init(void)
{
  u32 vram_offset = 32;
  u32 palette = 0;
  u32 attribute = 0;

  for (u32 i = 0; i < ARRAY_LENGTH(obj); i++) {
    copy_16((void *)(PRAM_OBJ + PRAM_PALETTE(palette)),
            obj[i].palette.start,
            obj[i].palette.size);

    copy_16((void *)(VRAM_OBJ + vram_offset),
            obj[i].character.start,
            obj[i].character.size);

    *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(attribute, 0)) =
      ( OBJ_A0__SHAPE_SQUARE
      | OBJ_A0__MODE_NORMAL
      | OBJ_A0__Y_COORDINATE(240) // offscreen
      );

    *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(attribute, 1)) =
      ( OBJ_A1__SIZE(OBJ__SQUARE_16_16)
      | OBJ_A1__X_COORDINATE(240) // offscreen
      );

    *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(attribute, 2)) =
      ( OBJ_A2__COLOR_PALETTE(palette)
      | OBJ_A2__PRIORITY(0)
      | OBJ_A2__CHARACTER(vram_offset / 32)
      );

    vram_offset += obj[i].character.size;
    attribute++;
    palette++;
  };
}

void obj_update(u32 attribute, u32 x, u32 y)
{
  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(attribute, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__Y_COORDINATE(y)
    );

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(attribute, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_16_16)
    | OBJ_A1__X_COORDINATE(x)
    );
}
