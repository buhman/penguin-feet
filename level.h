#include "type.h"

#define LEVEL_PALETTE 3
#define LEVEL_CHARACTER_BASE_BLOCK 0
#define LEVEL_CHARACTER_OFFSET (0)
#define LEVEL_CHARACTER_BLOCK_OFFSET (LEVEL_CHARACTER_OFFSET * (8 / 8 / 2))
#define LEVEL_SCREEN_BASE_BLOCK 31

void level_init(u32 * pathable);
void level_counter_init(const u32 * pathable, u32 * visited, u32 * count);
void level_counter_decrement(const u32 q, const u32 r, u32 * visited, u32 * unvisited_count);
