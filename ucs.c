#include "assert.h"
#include "type.h"
#include "min_heap.h"
#include "heap.h"
#include "ucs.h"

struct offset {
  s8 dq;
  s8 dr;
};

#define NEIGHBORS_LENGTH 4
static const struct offset neighbors[NEIGHBORS_LENGTH] = {
  {-1,  0},
  { 1,  0},
  { 0, -1},
  { 0,  1},
};

#define GRAPH_AREA UCS_GRAPH_AREA
#define QR_VALUE UCS_QR_VALUE
#define IN_BITMAP(bitmap, q, r) ((bitmap[r] >> q) & 1)

#ifdef PRIORITY_DEMO
void ucs(const u32 * graph, const value_t source, value_t * path, priority_t * min_priority)
#else
void ucs(const u32 * graph, const value_t source, value_t * path)
#endif
{
  /* /2: GRAPH_AREA is in u16 units; this writes u32 */
  for (u32 i = 0; i < (GRAPH_AREA / 2); i++) {
    ((u32 *)path)[i] = (u32)-1;
    #ifdef PRIORITY_DEMO
    ((u32 *)min_priority)[i] = (u32)-1;
    #endif
  }

  if (source == (value_t)-1
      || IN_BITMAP(graph, (u32)(source & 31), (u32)(source / 32)))
    return;

  min_heap_t heap;
  heap.index = 0;
  min_heap_insert(&heap, 0, source);

  u32 visited[32];
  for (u32 i = 0; i < 32; i++)
    visited[i] = 0;

  while (heap.index != 0) {
    priority_t u_priority;
    value_t u_value;
    min_heap_extract(&heap, &u_priority, &u_value);
    u32 u_q = u_value & 31;
    u32 u_r = u_value / 32;

    for (u32 i = 0; i < NEIGHBORS_LENGTH; i++) {
      u32 v_q = u_q + neighbors[i].dq;
      u32 v_r = u_r + neighbors[i].dr;

      if (v_q < 0 || v_q > 31 || v_r < 0 || v_r > 31)
        continue;
      if (IN_BITMAP(graph, v_q, v_r))
        continue;

      priority_t v_priority = u_priority + 1;
      value_t v_value = QR_VALUE(v_q, v_r);

      if (!IN_BITMAP(visited, v_q, v_r)) {
        min_heap_insert(&heap, v_priority, v_value);
        visited[v_r] |= (1 << v_q);

        path[v_value] = u_value;
        #ifdef PRIORITY_DEMO
        min_priority[v_value] = v_priority;
        #endif
      }

      #ifdef PRIORITY_DEMO
      if (v_priority < min_priority[v_value]) {
        //min_heap_decrease_priority(&heap, v_priority, v_value);
        //path[v_value] = u_value;
        min_priority[v_value] = v_priority;
      }
      #endif
    }
  }
}
