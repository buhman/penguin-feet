/* 3.1 Overall Memory Map */
#define GPRAM     0xe000000
#define GPROM     0x8000000
#define OAM_END   0x70003ff
#define OAM       0x7000000
#define VRAM_END  0x6017fff
#define VRAM_OBJ 0x6010000
#define VRAM     0x6000000
#define VRAM_BG  (VRAM)
/* 7.2 Color Palette RAM */
#define PRAM_OBJ_END 0x50003ff
#define PRAM_OBJ     0x5000200
#define PRAM_BG_END  0x50001ff
#define PRAM_BG      0x5000000
#define IO_REG    0x4000000
#define IWRAM_END    0x3007fff
#define IWRAM_SP_IRQ 0x3007fa0
#define IWRAM_SP_USR 0x3007f00
#define IWRAM        0x3000000
#define EWRAM_END 0x203ffff
#define EWRAM     0x2000000
#define SROM_END  0x0003fff
#define SROM      0x0000000

/* interrupt */
#define IWRAM_USER_ISR 0x3007ffc

/* palette */
#define PRAM_PALETTE(n) (n * 16 * 2)

/* base blocks */
#define CHARACTER_BASE_BLOCK_LENGTH 0x4000
#define CHARACTER_BASE_BLOCK_END(n) ((0x4000 * n) + 0x4000)
#define CHARACTER_BASE_BLOCK(n) (0x4000 * n)

#define SCREEN_BASE_BLOCK_LENGTH 0x800
#define SCREEN_BASE_BLOCK_END(n) ((0x800 * n) + 0x800))
#define SCREEN_BASE_BLOCK(n) (0x800 * n)

/* OAM */

#define OAM_OBJ_ATTRIBUTE(n, a) (n * 8 + a * 2)
#define OAM_OBJ__DX  (0) // PA
#define OAM_OBJ__DMX (1) // PB
#define OAM_OBJ__DY  (2) // PC
#define OAM_OBJ__DMY (3) // PD
#define OAM_OBJ_ROTATION_SCALING(n, p) ((n * 8 * 4) + (p * 8) + 6)
