#include "type.h"

void interactable_init(u32 * pathable, u8 * interactable);
void interactable_call(u32 q, u32 r, u8 * interactable);

#define INTERACTABLE_REG(h, o) ((h << 4) | (o << 0))

enum handler {
  HANDLER_LOG,
  HANDLER_LAST
};
