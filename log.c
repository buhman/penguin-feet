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

#define _sin32(s) (sin[(s) & 31])
#define _cos32(s) (sin[(8 - s) & 31])

static s8 affine7_offsets[][2] = {
  [0] = { 0,  0},
  [1] = {-9,  0},
  [2] = {-9, -9},
  [3] = { 0, -9},
};

typedef struct log {
  u8 step;
  u8 state; // next_state << 4 | state << 0
  u8 origin_q;
  u8 origin_r;
} log_t;

static u8 log_length = 1;

static log_t logs[16] = {
  [0] = {
    .origin_q = 11,
    .origin_r = 16,
    .step = 0,
    .state = 0,
  }
};

//define ORIGIN_Y 72
//define ORIGIN_X 56
#define ORIGIN_Q (11)
#define ORIGIN_R (16) // minimum 5
#define ORIGIN_X ((ORIGIN_Q - 0) * 8)
#define ORIGIN_Y ((ORIGIN_R - 3) * 8)

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
    | OBJ_A0__ROTATION_SCALING
    | OBJ_A0__Y_COORDINATE(ORIGIN_Y)
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

void log_set_state(u32 obj)
{
  u32 this_state = logs[obj].state & 0xf;
  logs[obj].state = (((this_state + 1) & 0x3) << 4) | this_state;
}

static u32 _step = 0;

void log_step_obj(u32 this_state, u32 next_state, u32 obj)
{
  if (++_step == 1)
    _step = 0;
  else return;

  // I honestly don't care that this state_dir expression is slow and ugly; this
  // could also be a lookup table
  u32 state_dir = (next_state == 0 && this_state == 3) ?  1
                : (next_state == 3 && this_state == 0) ? -1
                : next_state - this_state;

  s32 step = (logs[obj].step + 1) & 0x7;
  if (step == 0)
    this_state = next_state;

  s32 offset_x = affine7_offsets[this_state][0];
  s32 offset_y = affine7_offsets[this_state][1];
  s32 lerp_x = ((affine7_offsets[next_state][0] - offset_x) * step) / 8;
  s32 lerp_y = ((affine7_offsets[next_state][1] - offset_y) * step) / 8;

  *(volatile u16 *)(OAM + OAM_OBJ_ATTRIBUTE(16, 0)) =
    ( OBJ_A0__SHAPE_SQUARE
    | OBJ_A0__MODE_NORMAL
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
  u32 theta = this_state * 8 + (step * state_dir);
  s32 s = _sin32(theta);
  s32 c = _cos32(theta);

  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PA)) = c;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PC)) = -s;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PB)) = s;
  *(volatile s16 *)(OAM + OAM_OBJ_ROTATION_SCALING(0, OAM_OBJ__PD)) = c;

  //

  logs[obj].state = (next_state << 4) | this_state;
  logs[obj].step = step;
}

void log_step(void)
{
  for (u32 obj = 0; obj < log_length; obj++) {
    u32 this_state = logs[obj].state & 0xf;
    u32 next_state = (logs[obj].state >> 4) & 0xf;

    if (this_state != next_state)
      log_step_obj(this_state, next_state, obj);
  }
}
