#include "base.h"
#include "character.h"
#include "color.h"
#include "glyph/glyph.h"
#include "register.h"
#include "type.h"
#include "glyph.h"
#include "copy16.h"

void glyph_init(void)
{
  *(volatile u16 *)(PRAM_BG + PRAM_PALETTE(GLYPH_PALETTE) + 4) = RGB15(31, 31, 31);

  u32 * vram = (u32 *)( VRAM
                      + CHARACTER_BASE_BLOCK(GLYPH_CHARACTER_BASE_BLOCK)
                      + GLYPH_CHARACTER_BLOCK_OFFSET);
  u32 offset = 0;
  for (u32 i = 0; i < (u32)(&_binary_glyph_bizcat_glyph_size); i++) {
    u32 row_in = ((u8 *)&_binary_glyph_bizcat_glyph_start)[i];
    u32 row_out = 0;
    for (u32 bit = 0; bit < 8; bit++)
      row_out |= (2 * ((row_in >> bit) & 1)) << (bit * 4);

    vram[offset++] = row_out;
  }

  fill_16((void *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)),
          0,
          32 * 32 * 2
          );

  ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)))[32] =
    ( SCREEN_TEXT__PALETTE(GLYPH_PALETTE)
    | (GLYPH_CHARACTER_OFFSET + 10 * 2)
    );

  ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)))[32 + 32] =
    ( SCREEN_TEXT__PALETTE(GLYPH_PALETTE)
    | (GLYPH_CHARACTER_OFFSET + 10 * 2 + 1)
    );
}
