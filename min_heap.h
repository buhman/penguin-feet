#include "type.h"

#define HEAP_LENGTH (32 * 32)

#define index_t u16
#define priority_t u16
#define value_t u16

typedef struct {
  priority_t priority;
  value_t value;
} node_t;

typedef struct {
  node_t tree[HEAP_LENGTH];
  index_t index;
} min_heap_t;

void min_heap_insert(min_heap_t * heap, const priority_t priority, const value_t value);
void min_heap_decrease_priority(min_heap_t * heap, const priority_t priority, const value_t value);
void min_heap_extract(min_heap_t * heap, priority_t * key, value_t * value);

#undef index_t
#undef priority_t
#undef value_t
