#include "type.h"
#include "heap.h"

void path_debug_init(void);
void path_debug_update(const value_t source, const value_t target,
                       const value_t * path);
