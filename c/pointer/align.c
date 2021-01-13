#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

typedef struct {
	int a;	
}MemBlock;


typedef struct MemPool {
	uint16_t count;
#if 0
	MemBlock *head;
#else
	MemBlock head[];
#endif
}MemPool;

int main()
{
	MemPool *pPool = malloc(sizeof(MemPool)+sizeof(MemBlock)*10);
	
	MemBlock *pHd = pPool->head;
	//pPool->head->a = 10;
	printf("%p\n", pPool);
	printf("%p\n", pPool->head);
	printf("%p\n", &pPool->head);
	printf("%p\n", pHd);

	return 0;
}
