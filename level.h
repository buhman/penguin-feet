#include "type.h"

#define LEVEL_LAST 2

void level_init(const u32 level, u32 * pathable, u32 * printable);
void level_counter_init(const u32 * pathable, u32 * visited, u32 * count);
void level_counter_decrement(const u32 q, const u32 r, u32 * visited, u32 * unvisited_count);
void level_reset_penguin(const u32 level, actor_t * penguin);
