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

  fill_32((void *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)),
          0,
          32 * 32 * 2
          );
}

void glyph_clear_line(const u32 r)
{
  fill_32((void *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK) + (32 * 2 * r)),
          0,
          32 * 2 * 2
          );
}

void glyph_draw_line(const u32 r, const u8 * l, const u32 length)
{
  s32 q = (29 / 2) - (length / 2);
  if (q < 0)
    q = 0;

  glyph_clear_line(r);

  for (int i = 0; i < length; i++) {
    u32 character = l[i];
    if (character == 255)
      goto next;
    else
      character *= 2;

    ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)))[32 * r + q] =
      ( SCREEN_TEXT__PALETTE(GLYPH_PALETTE)
        | (GLYPH_CHARACTER_OFFSET + character)
        );

    ((volatile u16 *)(VRAM + SCREEN_BASE_BLOCK(GLYPH_SCREEN_BASE_BLOCK)))[32 * (r + 1) + q] =
      ( SCREEN_TEXT__PALETTE(GLYPH_PALETTE)
        | (GLYPH_CHARACTER_OFFSET + character + 1)
        );
  next:
    q++;
  }
}
