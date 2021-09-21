#include "type.h"
#include "heap.h"

typedef struct {
  u16 q;
  u16 r;
} ucs_coord_t;

typedef struct {
  s8 q;
  s8 r;
} neg_t;

typedef struct {
  u16 x;              // screen coordinates
  u16 y;
  ucs_coord_t target; // ucs coordinates
  neg_t neg;
} actor_t;

void actor_move_rpath(const u32 q, const u32 r, const value_t source, const value_t *path,
                      actor_t * actor, s32 * dq_o, s32 * dr_o);


void actor_move_fpath(const u32 q, const u32 r, const value_t target,
                      actor_t * actor);
