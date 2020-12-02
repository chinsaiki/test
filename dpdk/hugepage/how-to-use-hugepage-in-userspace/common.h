#include <assert.h>

#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)

#define rte_panic(fmt...) do{printf(fmt);assert(0);}while(0)