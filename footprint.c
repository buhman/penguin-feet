/*
       0 1
north  ↑ ⇑
east   → ⇒
south  ↓ ⇓ (north H+V)
west   ← ⇐ (east  H+V)

north
 ┌
  ↑⇑
  ↑⇑

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


static u32 character_offset;

void footprint_init(const u32 palette, const u32 block, const u32 offset)
{
  copy_16((void *)(PRAM_BG + PRAM_PALETTE(palette)),
          (void *)&_binary_character_footprint_palette_start,
          (u32)&_binary_character_footprint_palette_size);

  for (u32 i = 0; i < CHARACTERS_LENGTH; i++) {
    copy_16((void *)(VRAM + CHARACTER_BASE_BLOCK(block) + offset + (i * 8 * 8 / 2)),
            characters[i].start,
            characters[i].size);
  }

  character_offset = offset / (8 * 8 / 2);

  fill_16((void *)(VRAM + SCREEN_BASE_BLOCK(28)),
          SCREEN_TEXT__PALETTE(2) | 16,
          SCREEN_BASE_BLOCK_LENGTH);
}
