#include "type.h"
#include "base.h"
#include "register.h"
#include "copy16.h"

#include "character/log.h"
#include "log.h"

static const s16 sin[] = {
  0,
  49,
  97,
  142,
  181,
  212,
  236,
  251,
  256,
  251,
  236,
  212,
  181,
  142,
  97,
  49,
  0,
  -49,
  -97,
  -142,
  -181,
  -212,
  -236,
  -251,
  -256,
  -251,
  -236,
  -212,
  -181,
  -142,
  -97,
  -49
};

static u8 step = 0;
static u8 state = 0;

#define _sin32(s) (sin[(s) & 31])
#define _cos32(s) (sin[(8 - s) & 31])

static s8 affine7_offsets[][2] = {
  [0] = { 0,  0},
  [1] = {-9,  0},
  [2] = {-9, -9},
  [3] = { 0, -9},
};

#define ORIGIN_Y 72
#define ORIGIN_X 56

void log_init(void)
{
  copy_16((void *)(PRAM_OBJ + PRAM_PALETTE(1)),
          (void *)&_binary_character_log_palette_start,
          (u32)&_binary_character_log_palette_size);

  copy_16((void *)(VRAM_OBJ + 32 + 128),
          (void *)&_binary_character_log_character_start,
          (u32)&_binary_character_log_character_size);

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(16, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__DOUBLE_SIZE
    | OBJ_A0__ROTATION_SCALING
    | OBJ_A0__Y_COORDINATE(ORIGIN_Y - 9)
    );

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(16, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_64_64)
    | OBJ_A1__ROTATION_SCALING(0)
    | OBJ_A1__X_COORDINATE(ORIGIN_X)
    );

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(16, 2)) =
    ( OBJ_A2__COLOR_PALETTE(1)
    | OBJ_A2__PRIORITY(0)
    | OBJ_A2__CHARACTER(5)
    );

  // identity matrix
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PA)) = 256;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PC)) = 0;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PB)) = 0;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PD)) = 256;
}

void log_rotate_step(void)
{
  u32 step8 = (step >> 5) & 0x7;

  u32 next_state = (state + 1) & 3;
  s32 offset_x = affine7_offsets[state][0];
  s32 offset_y = affine7_offsets[state][1];
  s32 lerp_x = ((affine7_offsets[next_state][0] - offset_x) * step8) / 8;
  s32 lerp_y = ((affine7_offsets[next_state][1] - offset_y) * step8) / 8;

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(16, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
    | OBJ_A0__DOUBLE_SIZE
    | OBJ_A0__ROTATION_SCALING
    | OBJ_A0__Y_COORDINATE(ORIGIN_Y + offset_y + lerp_y)
    );

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(16, 1)) =
    ( OBJ_A1__SIZE(OBJ__SQUARE_64_64)
    | OBJ_A1__ROTATION_SCALING(0)
    | OBJ_A1__X_COORDINATE(ORIGIN_X + offset_x + lerp_x)
    );

  /* [ PA PC ]
     [ PB PD ]
     DX  cos(t) DY -sin(t)
     DMX sin(t) DMY cos(t)
   */
  u32 theta = state * 8 + step8;
  s32 s = _sin32(theta);
  s32 c = _cos32(theta);

  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PA)) = c;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PC)) = -s;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PB)) = s;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PD)) = c;

  //

  if (step == 255)
    state = next_state;
  step++;
}
