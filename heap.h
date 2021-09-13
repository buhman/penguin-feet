typedef u16 index_t;
typedef u16 priority_t;
typedef u16 value_t;

#define PARENT(_i) (index_t)(((_i) - 1U) >> 1)
#define CHILD_L(_i) (index_t)(((_i) << 1) + 1U)
#define CHILD_R(_i) (index_t)(((_i) << 1) + 2U)
