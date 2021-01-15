/**
 *	从 crtl 移植 2021年1月15日17:43:07 荣涛
 */

#define _fetch_and_add(ptr, v)  __sync_fetch_and_add(ptr, v)
#define _fetch_and_sub(ptr, v)  __sync_fetch_and_sub(ptr, v)
#define _fetch_and_or(ptr, v)  __sync_fetch_and_or(ptr, v)
#define _fetch_and_and(ptr, v)  __sync_fetch_and_and(ptr, v)
#define _fetch_and_xor(ptr, v)  __sync_fetch_and_xor(ptr, v)
#define _fetch_and_nand(ptr, v)  __sync_fetch_and_nand(ptr, v)
