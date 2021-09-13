#include "base.h"
#include "register.h"
#include "character.h"

#include "maze_image.h"
#include "copy16.h"

#define V_FLIP (1 << 6)
#define H_FLIP (1 << 5)
#define MAZE_COLUMNS 30
#define MAZE_ROWS 20

enum maze_tile {
  CORNER_NE = 1,
  CROSS_NESW = 2,
  END_W = 3,
  END_S = 4,
  LINE_NS = 5,
  LINE_WE = 6,
  TEE_NES = 7,
  TEE_WNE = 8,
};


#define REP0(X)
#define REP1(X) X ,
#define REP2(X) REP1(X) X ,
#define REP3(X) REP2(X) X ,
#define REP4(X) REP3(X) X ,
#define REP5(X) REP4(X) X ,
#define REP6(X) REP5(X) X ,
#define REP7(X) REP6(X) X ,
#define REP8(X) REP7(X) X ,
#define REP9(X) REP8(X) X ,
#define REP10(X) REP9(X) X

#define REP(TENS, ONES, X) \
  REP##TENS(REP10(X)) \
  REP##ONES(X)

/*
CORNER_NE │ └
          └──

CROSS_NESW ─┘ └─
           ─┐ ┌─

END_W  ──┐
       ──┘

END_S  ┌─┐
       │ │

LINE_NS │ │
        │ │

LINE_WE ────
        ────

TEE_NES │ └─
        │ ┌─

TEE_WNE  ─┘ └─
         ─────

*/

static const unsigned char maze_cells[MAZE_ROWS][MAZE_COLUMNS] = {
  { // 0
    CORNER_NE | V_FLIP,
    REP(1, 3, LINE_WE)
    REP(0, 2, TEE_WNE | V_FLIP)
    REP(1, 3, LINE_WE)
    CORNER_NE | V_FLIP | H_FLIP,
  },
  { // 1
    LINE_NS,
    REP(1, 3, 0)
    REP(0, 2, LINE_NS)
    REP(1, 3, 0)
    LINE_NS,
  },
  { // 2
    LINE_NS,
    REP(1, 3, 0)
    REP(0, 2, LINE_NS)
    REP(1, 3, 0)
    LINE_NS,
  },
  { // 3
    LINE_NS,
    REP(0, 2, 0)
    CORNER_NE | V_FLIP,
    LINE_WE,
    CORNER_NE | V_FLIP | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE | V_FLIP,
    REP(0, 2, LINE_WE)
    CORNER_NE | V_FLIP | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE,
    CORNER_NE | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE | V_FLIP,
    REP(0, 2, LINE_WE)
    CORNER_NE | V_FLIP | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE | V_FLIP,
    LINE_WE,
    CORNER_NE | V_FLIP | H_FLIP,
    REP(0, 2, 0)
    LINE_NS,
  },
  { // 4
    LINE_NS,
    REP(0, 2, 0)
    CORNER_NE ,
    LINE_WE,
    CORNER_NE | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE ,
    REP(0, 2, LINE_WE)
    CORNER_NE | H_FLIP,
    REP(0, 6, 0)
    CORNER_NE ,
    REP(0, 2, LINE_WE)
    CORNER_NE | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE ,
    LINE_WE,
    CORNER_NE | H_FLIP,
    REP(0, 2, 0)
    LINE_NS,
  },
  { // 5
    LINE_NS,
    REP(0, 9, 0) // 28?
    REP(1, 0, 0)
    REP(0, 9, 0)
    LINE_NS,
  },
  { // 6
    LINE_NS,
    REP(1, 3, 0)
    CORNER_NE | V_FLIP,
    CORNER_NE | V_FLIP | H_FLIP,
    REP(1, 3, 0)
    LINE_NS,
  },
  { // 7
    LINE_NS,
    REP(0, 2, 0)
    END_W | H_FLIP,
    END_W,
    REP(0, 2, 0)
    END_S,
    REP(0, 2, 0)
    END_W | H_FLIP,
    REP(0, 3, LINE_WE)
    REP(0, 2, CROSS_NESW)
    REP(0, 3, LINE_WE)
    END_W,
    REP(0, 2, 0)
    END_S,
    REP(0, 2, 0)
    END_W | H_FLIP,
    END_W,
    REP(0, 2, 0)
    LINE_NS,
  },
  { // 8
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
    REP(0, 6, 0)
    REP(0, 2, LINE_NS)
    REP(0, 6, 0)
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
  },
  { // 9
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
    REP(0, 6, 0)
    REP(0, 2, LINE_NS)
    REP(0, 6, 0)
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
  },
  { // 10
    CORNER_NE,
    REP(0, 3, LINE_WE)
    CORNER_NE | H_FLIP | V_FLIP,
    REP(0, 2, 0)
    TEE_NES,
    REP(0, 3, LINE_WE)
    END_W,
    REP(0, 2, 0)
    REP(0, 2, LINE_NS)
    REP(0, 2, 0)
    END_W | H_FLIP,
    REP(0, 3, LINE_WE)
    TEE_NES | H_FLIP,
    REP(0, 2, 0)
    CORNER_NE | V_FLIP,
    REP(0, 3, LINE_WE)
    CORNER_NE | H_FLIP,
  },
  { // 11
    REP(0, 4, 0)
    LINE_NS,
    REP(0, 2, 0)
    LINE_NS,
    REP(0, 6, 0)
    REP(0, 2, LINE_NS)
    REP(0, 6, 0)
    LINE_NS,
    REP(0, 2, 0)
    LINE_NS,
    REP(0, 4, 0)
  },
  { // 12
    REP(0, 4, 0)
    LINE_NS,
    REP(0, 2, 0)
    LINE_NS,
    REP(0, 6, 0)
    REP(0, 2, LINE_NS)
    REP(0, 6, 0)
    LINE_NS,
    REP(0, 2, 0)
    LINE_NS,
    REP(0, 4, 0)
  },
  { // 13
    CORNER_NE | V_FLIP,
    REP(0, 3, LINE_WE)
    CORNER_NE | H_FLIP,
    REP(0, 2, 0)
    LINE_NS,
    REP(0, 2, 0)
    END_W | H_FLIP,
    REP(0, 3, LINE_WE)
    REP(0, 2, TEE_WNE)
    REP(0, 3, LINE_WE)
    END_W,
    REP(0, 2, 0)
    LINE_NS,
    REP(0, 2, 0)
    CORNER_NE,
    REP(0, 3, LINE_WE)
    CORNER_NE | H_FLIP | V_FLIP,
  },
  { // 14
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
    REP(1, 4, 0)
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
  },
  { // 15
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
    REP(1, 4, 0)
    LINE_NS,
    REP(0, 6, 0)
    LINE_NS,
  },
  { // 16
    LINE_NS,
    REP(0, 2, 0)
    END_W | H_FLIP,
    REP(0, 3, LINE_WE)
    CORNER_NE | H_FLIP,
    REP(0, 2, 0)
    END_W | H_FLIP,
    REP(0, 8, LINE_WE)
    END_W,
    REP(0, 2, 0)
    CORNER_NE,
    REP(0, 3, LINE_WE)
    END_W,
    REP(0, 2, 0)
    LINE_NS,
  },
  { // 17
    LINE_NS,
    REP(0, 9, 0)
    REP(1, 0, 0)
    REP(0, 9, 0)
    LINE_NS,
  },
  { // 18
    LINE_NS,
    REP(0, 9, 0)
    REP(1, 0, 0)
    REP(0, 9, 0)
    LINE_NS,
  },
  { // 19
    CORNER_NE,
    REP(0, 9, LINE_WE)
    REP(1, 0, LINE_WE)
    REP(0, 9, LINE_WE)
    CORNER_NE | H_FLIP,
  }
};

/*
   7  6  5  4  3  2  1  0
   P  V  H  [       type]
*/

static const character_t characters[] = {
  [CORNER_NE] = {
    (void *)&_binary_image_maze_tile_CORNER_NE_character_start,
    (unsigned int)&_binary_image_maze_tile_CORNER_NE_character_size,
  },
  [CROSS_NESW] = {
    (void *)&_binary_image_maze_tile_CROSS_NESW_character_start,
    (unsigned int)&_binary_image_maze_tile_CROSS_NESW_character_size,
  },
  [END_S] = {
    (void *)&_binary_image_maze_tile_END_S_character_start,
    (unsigned int)&_binary_image_maze_tile_END_S_character_size,
  },
  [END_W] = {
    (void *)&_binary_image_maze_tile_END_W_character_start,
    (unsigned int)&_binary_image_maze_tile_END_W_character_size,
  },
  [LINE_NS] = {
    (void *)&_binary_image_maze_tile_LINE_NS_character_start,
    (unsigned int)&_binary_image_maze_tile_LINE_NS_character_size,
  },
  [LINE_WE] = {
    (void *)&_binary_image_maze_tile_LINE_WE_character_start,
    (unsigned int)&_binary_image_maze_tile_LINE_WE_character_size,
  },
  [TEE_NES] = {
    (void *)&_binary_image_maze_tile_TEE_NES_character_start,
    (unsigned int)&_binary_image_maze_tile_TEE_NES_character_size,
  },
  [TEE_WNE] = {
    (void *)&_binary_image_maze_tile_TEE_WNE_character_start,
    (unsigned int)&_binary_image_maze_tile_TEE_WNE_character_size,
  },
};
#define CHARACTERS_LENGTH ((sizeof (characters)) / (sizeof (character_t)))


void maze_init(void)
{
  copy_16((void *)(PRAM_BG + PRAM_PALETTE(0)),
          (void *)&_binary_image_maze_tile_palette_start,
          (unsigned int)&_binary_image_maze_tile_palette_size);

  for (int i = 1; i < CHARACTERS_LENGTH; i++) {
    copy_16((void *)(VRAM + CHARACTER_BASE_BLOCK(0) + (i * 32)),
            characters[i].start,
            characters[i].size);
  }

  for (int y = 0; y < MAZE_ROWS; y++) {
    for (int x = 0; x < MAZE_COLUMNS; x++) {
      *(unsigned short *)(VRAM + SCREEN_BASE_BLOCK(31) + (x * 2) + (y * 32 * 2))
        = ((maze_cells[y][x] & 0b11100000) << 5)
        | (maze_cells[y][x] & 0b11111)
        ;
    }
  }
}
