/**
 *	从 crtl 移植 荣涛 2021年1月15日17:43:54
 */
#define _add_and_fetch(ptr, v)  __sync_add_and_fetch(ptr, v)
#define _sub_and_fetch(ptr, v)  __sync_sub_and_fetch(ptr, v)
#define _or_and_fetch(ptr, v)  __sync_or_and_fetch(ptr, v)
#define _and_and_fetch(ptr, v)  __sync_and_and_fetch(ptr, v)
#define _xor_and_fetch(ptr, v)  __sync_xor_and_fetch(ptr, v)
#define _nand_and_fetch(ptr, v)  __sync_nand_and_fetch(ptr, v)
