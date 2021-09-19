#include "type.h"
#include "assert.h"

#include "log.h"

// = type << 4 | obj << 0

typedef void (* handler_t)(u32 obj);

enum handler {
  HANDLER_LOG,
  HANDLER_LAST
};

// (nib)-1 is reseved
static_assert(HANDLER_LAST < 16);

static handler_t handlers[HANDLER_LAST] = {
  [HANDLER_LOG] = &log_set_state
};

#define REG(h, o) ((h << 4) | (o << 0))

// [r][q]
static u8 interactables[32 * 32];
#define ARRAY_LENGTH(_a) ((sizeof (_a)) / (sizeof ((_a)[0])))

void interactable_init(void)
{
  for (u32 i = 0; i < (ARRAY_LENGTH(interactables) / 4); i++)
    ((u32 *)interactables)[i] = -1;

  u32 ri = 16 * 32;
  interactables[ri + 11] = REG(HANDLER_LOG, 0);
  interactables[ri + 12] = REG(HANDLER_LOG, 0);
  interactables[ri + 13] = REG(HANDLER_LOG, 0);
  interactables[ri + 14] = REG(HANDLER_LOG, 0);
  interactables[ri + 15] = REG(HANDLER_LOG, 0);
  interactables[ri + 16] = REG(HANDLER_LOG, 0);
  interactables[ri + 17] = REG(HANDLER_LOG, 0);
}

void interactable_call(u32 q, u32 r)
{
  u32 a = interactables[r * 32 + q];
  if (a != (u8)-1) {
    u32 handler = (a >> 4 & 0xf);
    u32 obj = (a >> 0 & 0xf);

    (*handlers[handler])(obj);
  }

  return;
}
