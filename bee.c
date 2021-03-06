#include "actor.h"
#include "obj.h"
#include "bee.h"
#include "heap.h"
#include "ucs.h"
#include "path_debug.h"
#include "character.h"

#define BEE_Q 10
#define BEE_R 1

static actor_t bee = {
  .x = BEE_Q * 8,
  .y = BEE_R * 8,
  .target.q = BEE_Q,
  .target.r = BEE_R,
  .neg.q = 0,
  .neg.r = 0,
};

static value_t target = (value_t)-1;

void bee_step(const value_t source, const value_t * path, u32 * q_out, u32 * r_out)
{
  obj_update(OBJ_BEE_ATTRIBUTE, bee.x, bee.y);

  u32 bee_q = (bee.x >> 3) + (bee.neg.q && (bee.x & 0b111));
  u32 bee_r = (bee.y >> 3) + (bee.neg.r && (bee.y & 0b111));

  value_t w = UCS_QR_VALUE(bee_q, bee_r);
  //path_debug_update(source, w, path);

  if ((bee.x & 0b111) == 0 && (bee.y & 0b111) == 0) {
    target = path[w];
  }

  if (target != (value_t)-1) {
    actor_move_fpath(bee_q, bee_r, target, &bee);
  }

  *q_out = bee_q;
  *r_out = bee_r;
}
