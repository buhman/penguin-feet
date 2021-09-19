/*
       0 1
north  ↑ ⇑
east   → ⇒
south  ↓ ⇓ (north H+V)
west   ← ⇐ (east  H+V)

north
 ┌
  ↑⇑  00 01
  ↑⇑  10 11

east
 ┌
  →→
  ⇒⇒

south
 ┌
  ⇓↓
  ⇓↓

west
 ┌
  ⇐⇐
  ←←

*/

#include "base.h"
#include "register.h"

#include "type.h"
#include "copy16.h"
#include "footprint.h"
#include "character.h"
#include "character/footprint.h"
#include "footprint.h"


enum footprint {
  NORTH_0,
  NORTH_1,
  EAST_0,
  EAST_1,
  FOOTPRINT_LAST
};

static const character_t characters[FOOTPRINT_LAST] = {
  [NORTH_0] = {
    (void *)&_binary_character_footprint_north_0_character_start,
    (u32)&_binary_character_footprint_north_0_character_size,
  },
  [NORTH_1] = {
    (void *)&_binary_character_footprint_north_1_character_start,
    (u32)&_binary_character_footprint_north_1_character_size,
  },
  [EAST_0] = {
    (void *)&_binary_character_footprint_east_0_character_start,
    (u32)&_binary_character_footprint_east_0_character_size,
  },
  [EAST_1] = {
    (void *)&_binary_character_footprint_east_1_character_start,
    (u32)&_binary_character_footprint_east_1_character_size,
  },
};
#define CHARACTERS_LENGTH ((sizeof (characters)) / (sizeof (character_t)))

void footprint_init()
{
  copy_16((void *)(PRAM_BG + PRAM_PALETTE(FOOTPRINT_PALETTE)),
          (void *)&_binary_character_footprint_palette_start,
          (u32)&_binary_character_footprint_palette_size);

  for (u32 i = 0; i < CHARACTERS_LENGTH; i++) {
    copy_16((void *)( VRAM
                    + CHARACTER_BASE_BLOCK(FOOTPRINT_CHARACTER_BASE_BLOCK)
                    + FOOTPRINT_CHARACTER_BLOCK_OFFSET
                    + (i * (8 * 8 / 2))
                    ),
            characters[i].start,
            characters[i].size);
  }

  fill_16((void *)(VRAM + SCREEN_BASE_BLOCK(FOOTPRINT_SCREEN_BASE_BLOCK)),
          0,
          SCREEN_BASE_BLOCK_LENGTH);
}


#define P SCREEN_TEXT__PALETTE(FOOTPRINT_PALETTE)
#define V_NORTH_0 (FOOTPRINT_CHARACTER_OFFSET + NORTH_0)
#define V_NORTH_1 (FOOTPRINT_CHARACTER_OFFSET + NORTH_1)
#define V_EAST_0 (FOOTPRINT_CHARACTER_OFFSET + EAST_0)
#define V_EAST_1 (FOOTPRINT_CHARACTER_OFFSET + EAST_1)
#define V_SOUTH_0 ( (FOOTPRINT_CHARACTER_OFFSET + NORTH_0) \
                  | SCREEN_TEXT__H_FLIP | SCREEN_TEXT__V_FLIP )
#define V_SOUTH_1 ( (FOOTPRINT_CHARACTER_OFFSET + NORTH_1) \
                  | SCREEN_TEXT__H_FLIP | SCREEN_TEXT__V_FLIP )
#define V_WEST_0 ( (FOOTPRINT_CHARACTER_OFFSET + EAST_0) \
                 | SCREEN_TEXT__H_FLIP | SCREEN_TEXT__V_FLIP )
#define V_WEST_1 ( (FOOTPRINT_CHARACTER_OFFSET + EAST_1) \
                 | SCREEN_TEXT__H_FLIP | SCREEN_TEXT__V_FLIP )

#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3

static u16 value[4][4] = {
  [DIR_NORTH] = {(P | V_NORTH_0), // 00
                 (P | V_NORTH_1), // 01
                 (P | V_NORTH_0), // 10
                 (P | V_NORTH_1)  // 11
                },
  [DIR_EAST]  = {(P | V_EAST_0), // 00
                 (P | V_EAST_0), // 01
                 (P | V_EAST_1), // 10
                 (P | V_EAST_1)  // 11
                },
  [DIR_SOUTH] = {(P | V_SOUTH_1), // 00
                 (P | V_SOUTH_0), // 01
                 (P | V_SOUTH_1), // 10
                 (P | V_SOUTH_0)  // 11
                },
  [DIR_WEST]  = {(P | V_WEST_1), // 00
                 (P | V_WEST_1), // 01
                 (P | V_WEST_0), // 10
                 (P | V_WEST_0)  // 11
                }
};

#define SCREEN_BASE (VRAM + SCREEN_BASE_BLOCK(FOOTPRINT_SCREEN_BASE_BLOCK))

void footprint_place(const u32 * printable, const u32 q, const u32 r, const u32 dir)
{
  u32 qa = q * 2;
  u32 ra = r * 2;
  if ((printable[r + 0] >> (q + 0)) & 1)
    *(volatile u16 *)(SCREEN_BASE + ((ra + 0) * 32 + qa + 0)) = value[dir][0];
  if ((printable[r + 0] >> (q + 1)) & 1)
    *(volatile u16 *)(SCREEN_BASE + ((ra + 0) * 32 + qa + 2)) = value[dir][1];
  if ((printable[r + 1] >> (q + 0)) & 1)
    *(volatile u16 *)(SCREEN_BASE + ((ra + 2) * 32 + qa + 0)) = value[dir][2];
  if ((printable[r + 1] >> (q + 1)) & 1)
    *(volatile u16 *)(SCREEN_BASE + ((ra + 2) * 32 + qa + 2)) = value[dir][3];
}
