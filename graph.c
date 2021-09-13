#include "graph.h"

/*
16,16    -> 2,2
224,144  -> 28,18

32x32 ; 1 bit per column ; 1 world per row
*/

#define P(n) | (1 << n)
static const unsigned int _pathable[32] = {
  0,
  0,
  0 P(2) P(3) P(4) P(5) P(6) P(7) P(8) P(9) P(10) P(11) P(12) P(13), // 2
  0 P(2) P(7) P(13), // 3
  0 P(2) P(7) P(13), // 4
  0 P(2) P(7) P(13), // 5
  0 P(2) P(3) P(4) P(5) P(6) P(7) P(8) P(9) P(10) P(11) P(12) P(13), // 6
};

int graph_pathable(unsigned short sx, unsigned short sy)
{
  /*
    sx and sy are in screen coordinates
    gx and gy are in graph coordinates
   */

  unsigned short gx, gy, gx_ceil, gy_ceil;

  gx = (sx >> 3) & 0x1f;
  gy = (sy >> 3) & 0x1f;

  gx_ceil = gx + ((sx & 0b111) != 0);
  gy_ceil = gy + ((sy & 0b111) != 0);

  return (
       ((_pathable[gy] >> gx) & 1)
    && ((_pathable[gy_ceil] >> gx_ceil) & 1)
    && ((_pathable[gy] >> gx_ceil) & 1)
    && ((_pathable[gy_ceil] >> gx) & 1)
  );
}
