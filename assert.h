#pragma once

#ifdef DEBUG
extern void __assert_fail(const char *expr, const char *file, int line, const char *func);
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
#else
#define	assert(x) (void)0
#endif

#define static_assert _Static_assert
