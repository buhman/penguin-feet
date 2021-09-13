#include "type.h"

#define value_t u16

void ucs(const u32 * graph, const value_t source, value_t * path);

#undef value_t

#define UCS_GRAPH_ROWS 32
#define UCS_GRAPH_COLS 32
#define UCS_GRAPH_AREA (UCS_GRAPH_ROWS * UCS_GRAPH_COLS)
#define UCS_XY_VALUE(x, y) ((y * 32) + x)
