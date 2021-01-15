/* put addr to the Cache before use 
 *	从 CRTL 中移植 2021年1月15日17:41:35
 * */
#ifndef prefetch
#define prefetch(x) __builtin_prefetch(x)
#endif
 
#ifndef prefetchw
#define prefetchw(x) __builtin_prefetch(x, 1)
#endif
