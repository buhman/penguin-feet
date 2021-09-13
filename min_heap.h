#include "type.h"

#define HEAP_LENGTH (32 * 32)

#define index_t u16
#define value_t u16

typedef struct {
  index_t key;
  value_t value;
} node_t;

typedef struct {
  node_t tree[HEAP_LENGTH];
  index_t index;
} min_heap_t;

void min_heap_insert(min_heap_t * heap, index_t key, value_t value);
void min_heap_decrease_key(min_heap_t * heap, index_t key, value_t value);
value_t min_heap_extract(min_heap_t * heap);

#undef index_t
#undef value_t
