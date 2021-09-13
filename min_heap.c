#include "assert.h"
#include "type.h"
#include "min_heap.h"
#include "heap.h"

static_assert(HEAP_LENGTH <= (1 << (8 * (sizeof (index_t)))));
static_assert(HEAP_LENGTH <= (1 << (8 * (sizeof (value_t)))));
static_assert((sizeof (node_t)) == (sizeof (u32)));

#define _HEAP_UP \
  while (ix != 0) { \
    index_t parent_ix = PARENT(ix); \
    if (heap->tree[ix].key > heap->tree[parent_ix].key) \
      break; \
    \
    node_t tmp = heap->tree[ix]; \
    heap->tree[ix] = heap->tree[parent_ix]; \
    heap->tree[parent_ix] = tmp; \
    ix = parent_ix; \
  }

void min_heap_insert(min_heap_t * heap, index_t key, value_t value)
{
  assert(heap->index < HEAP_LENGTH);

  index_t ix = heap->index;
  heap->tree[heap->index++] = (node_t){key, value};

  _HEAP_UP
}

void min_heap_decrease_key(min_heap_t * heap, index_t key, value_t value)
{
  index_t ix;

  for (ix = 0; ix < heap->index; ix++) {
    if (heap->tree[ix].value == value)
      break;
  }

  assert(ix != heap->index);
  assert(key < heap->tree[ix].key);
  heap->tree[ix].key = key;

  _HEAP_UP
}

value_t min_heap_extract(min_heap_t * heap)
{
  if (heap->index == 0)
    return heap->tree[heap->index].value;

  index_t ix = 0;
  value_t old_root = heap->tree[ix].value;
  heap->tree[ix] = heap->tree[--heap->index];

  while (1) {
    index_t child_l = CHILD_L(ix);
    if (child_l >= heap->index)
      break;
    index_t swap_ix = child_l;
    index_t child_r = CHILD_R(ix);
    if ((child_r < heap->index) && (heap->tree[child_r].key < heap->tree[child_l].key))
      swap_ix = child_r;

    if (heap->tree[swap_ix].key > heap->tree[ix].key)
      break;

    node_t tmp = heap->tree[ix];
    heap->tree[ix] = heap->tree[swap_ix];
    heap->tree[swap_ix] = tmp;
    ix = swap_ix;
  }
  return (value_t)old_root;
}
