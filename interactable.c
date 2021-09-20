#include "type.h"
#include "assert.h"
#include "interactable.h"

#include "log.h"

// = type << 4 | obj << 0

typedef void (* handler_t)(u32 obj, u32 q, u32 r);

// (nib)-1 is reseved
static_assert(HANDLER_LAST < 16);

static handler_t handlers[HANDLER_LAST] = {
  [HANDLER_LOG] = &log_set_state
};

// [r][q]


void interactable_init(u32 * pathable, u8 * interactable)
{
  for (u32 i = 0; i < (32 * 32 / 4); i++)
    ((u32 *)interactable)[i] = -1;
}

void interactable_call(u32 q, u32 r, u8 * interactable)
{
  u32 a = interactable[r * 32 + q];
  if (a != (u8)-1) {
    u32 handler = (a >> 4 & 0xf);
    u32 obj = (a >> 0 & 0xf);

    (*handlers[handler])(obj, q, r);
  }

  return;
}
