#include "type.h"
#include "base.h"
#include "register.h"

#include "copy16.h"
#include "background.h"
#include "character.h"
#include "character/GBA_BG.h"

void background_init(void)
{
  copy_32((void *)(PRAM_BG + PRAM_PALETTE(BACKGROUND_PALETTE) + 2),
          (void *)&_binary_character_GBA_BG_palette_start,
          (u32)&_binary_character_GBA_BG_palette_size);

  copy_32((void *)( VRAM
                  + CHARACTER_BASE_BLOCK(BACKGROUND_CHARACTER_BASE_BLOCK)
                  + BACKGROUND_CHARACTER_BLOCK_OFFSET),
          (void *)&_binary_character_GBA_BG_character_start,
          (u32)&_binary_character_GBA_BG_character_size);

  for (int i = 0; i < (32 * 32); i++) {
    *(u16 *)(VRAM + SCREEN_BASE_BLOCK(BACKGROUND_SCREEN_BASE_BLOCK) + i * 2)
      = SCREEN_TEXT__PALETTE(BACKGROUND_PALETTE)
      | (BACKGROUND_CHARACTER_OFFSET + i)
      ;
  }
}
