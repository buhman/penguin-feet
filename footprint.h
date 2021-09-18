#define FOOTPRINT_PALETTE 2
#define FOOTPRINT_CHARACTER_BASE_BLOCK 0
#define FOOTPRINT_CHARACTER_OFFSET (16)
#define FOOTPRINT_CHARACTER_BLOCK_OFFSET (FOOTPRINT_CHARACTER_OFFSET * (8 * 8 / 2))
#define FOOTPRINT_SCREEN_BASE_BLOCK 28

void footprint_init();
void footprint_place(const u32 q, const u32 r, const u32 dir);
