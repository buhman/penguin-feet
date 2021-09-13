#include "base.h"
#include "register.h"

#include "copy16.h"
#include "background.h"
#include "background_image.h"

void background_init(void)
{
  copy_16((void *)(PRAM_BG + PRAM_PALETTE(1)),
          (void *)&_binary_image_background_palette_start,
          (unsigned int)&_binary_image_background_palette_size);

  copy_16((void *)(VRAM + CHARACTER_BASE_BLOCK(1) + 32),
          (void *)&_binary_image_background_character_start,
          (unsigned int)&_binary_image_background_character_size);

  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 30; x++) {
      *(unsigned short *)(VRAM + SCREEN_BASE_BLOCK(30) + (x * 2) + (y * 32 * 2))
        = SCREEN_TEXT__PALETTE(1)
        | 1
        | (x & 1 ? SCREEN_TEXT__H_FLIP : 0)
        | (y & 1 ? SCREEN_TEXT__V_FLIP : 0)
        ;
    }
  }
}
