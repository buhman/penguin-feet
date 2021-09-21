#include "type.h"
#include "actor.h"
#include "heap.h"
#include "ucs.h"

void actor_move_rpath(const u32 q, const u32 r, const value_t source, const value_t *path,
                      actor_t * actor, s32 * dq_o, s32 * dr_o)
{
  // assumes (path) source == UCS_QR_VALUE(q, r)

  value_t target = UCS_QR_VALUE(actor->target.q, actor->target.r);
  value_t w = target;
  value_t next_w = target;
  while (w != (value_t)-1 && w != source) {
    next_w = w;
    w = path[w];
  }

  u32 next_w_q = next_w & 31;
  u32 next_w_r = next_w / 32;

  s32 dq = next_w_q - q;
  s32 dr = next_w_r - r;
  actor->neg.q = (dq < 0);
  actor->neg.r = (dr < 0);
  actor->x += dq;
  actor->y += dr;
  *dq_o = dq;
  *dr_o = dr;
}

void actor_move_fpath(const u32 q, const u32 r, const value_t target,
                      actor_t * actor)
{
  u32 next_w_q = target & 31;
  u32 next_w_r = target / 32;

  s32 dq = next_w_q - q;
  s32 dr = next_w_r - r;

  actor->neg.q = (dq < 0);
  actor->neg.r = (dr < 0);
  actor->x += dq;
  actor->y += dr;
}
