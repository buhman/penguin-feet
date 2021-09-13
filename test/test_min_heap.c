#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "min_heap.h"
#include "assert.h"
#include "heap.h"

#define ARRAY_LENGTH(_a) ((sizeof (_a)) / (sizeof ((_a)[0])))

void print_tree(int fd, min_heap_t * heap, index_t index, u32 prefix)
{
  if (heap->index != 0)
    dprintf(fd, "\"%d_%d\" [label=\"k=%d v=%d\"]\n", prefix, index, heap->tree[index].key, heap->tree[index].value);

  index_t child_l = CHILD_L(index);
  if (child_l < heap->index) {
    dprintf(fd, "\"%d_%d\" -> \"%d_%d\"\n", prefix, index, prefix, child_l);
    print_tree(fd, heap, child_l, prefix);
  }
  index_t child_r = CHILD_R(index);
  if (child_r < heap->index) {
    dprintf(fd, "\"%d_%d\" -> \"%d_%d\"\n", prefix, index, prefix, child_r);
    print_tree(fd, heap, child_r, prefix);
  }
}

void test_min_heap_decrease_key(int fd)
{
  const value_t values[] = {25, 100, 2, 36, 19, 1, 7, 4, 17};

  min_heap_t heap;
  heap.index = 0;
  for (index_t i = 0; i < ARRAY_LENGTH(values); i++) {
    min_heap_insert(&heap, values[i], values[i]);
  }

  u32 prefix = 0;
  dprintf(fd, "digraph min_heap {\n");

  dprintf(fd, "subgraph cluster_%d {\n", prefix);
  print_tree(fd, &heap, 0, prefix++);
  dprintf(fd, "}\n");

  min_heap_decrease_key(&heap, 2, 100);
  dprintf(fd, "subgraph cluster_%d {\n", prefix);
  print_tree(fd, &heap, 0, prefix++);
  dprintf(fd, "}\n");

  dprintf(fd, "}\n");
}

void test_min_heap_extract(int fd)
{
  const value_t values[] = {25, 100, 2, 36, 19, 1, 7, 3, 17};

  min_heap_t heap;
  heap.index = 0;
  for (index_t i = 0; i < ARRAY_LENGTH(values); i++) {
    min_heap_insert(&heap, values[i], values[i]);
  }

  dprintf(fd, "digraph min_heap {\n");
  u32 prefix = 0;

  dprintf(fd, "subgraph cluster_%d {\n", prefix);
  print_tree(fd, &heap, 0, prefix++);
  dprintf(fd, "}\n");

  while (heap.index != 0) {
    dprintf(fd, "subgraph cluster_%d {\n", prefix);
    value_t v = min_heap_extract(&heap);
    dprintf(fd, "\"%d_%d\" [label=\"v=%d\" shape=box]", prefix, -1, v);
    print_tree(fd, &heap, 0, prefix++);
    dprintf(fd, "}\n");
  }

  dprintf(fd, "}\n");
}

int main(void)
{
  int fd;
  fd = open("min_heap_extract.dot", O_WRONLY|O_CREAT);
  assert(!(fd < 0));
  test_min_heap_extract(fd);
  close(fd);
  fd = open("min_heap_decrease_key.dot", O_WRONLY|O_CREAT);
  assert(!(fd < 0));
  test_min_heap_decrease_key(fd);
  close(fd);

  return 0;
}
