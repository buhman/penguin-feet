#include <stdio.h>

#include "assert.h"
#include "ucs.h"
#include "heap.h"

static const u32 graph[32] = {
                            /* 0,0 */
  0b00000000000000000000000000000000,
  /* dest: 28,1 */
  0b00000000000000000000000000000000,
  0b00011111111111100000000000000000,
  0b00010000000000000000000001000000,
  0b00010000000000000000000001000000,
  0b00010000000000000000000111111111,
  0b00010111111111111111111000000000,
  0b00000000000000000000000000000000,
  /* 31,7 */           /* src: 1,6 */
};

int main(void)
{
  value_t path[UCS_GRAPH_AREA];

  value_t source = UCS_XY_VALUE(1, 6);
  value_t target = UCS_XY_VALUE(28, 1);

  ucs(&graph[0], source, &path[0]);

  value_t w = target;
  while (w != source) {
    u32 w_x = w & 31;
    u32 w_y = w / 32;
    printf("%d %d\n", w_x, w_y);
    w = path[w];
    assert(w != (u32)-1);
  }

  return 0;
}
