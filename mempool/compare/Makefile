## 对比内存申请的效率（耗费时间）
#	荣涛		2020年11月17日

SOURCES=compare.c
SOURCES+=main.c
SOURCES+=malloc.c
SOURCES+=obstack.c
#SOURCES+=../mempool/nmx_palloc.c \
#		 ../mempool/nmx_alloc.c \
#		 nmx.c

SOURCES+=../ncx_mempool/ncx_slab.c \
			ncx.c

SOURCES+=../../redis/zmalloc/redis-6.0.5/zmalloc.c \
			redis-6.0.5-zmalloc.c

INCLUDE=-I./ 
INCLUDE+=-I../mempool
INCLUDE+=-I../ncx_mempool
INCLUDE+=-I../../redis/zmalloc/redis-6.0.5/

#ncx_mempool  DLOG_LEVEL=8(ERROR)
DEFINES=-DLOG_LEVEL=8

JEMALLOC_LIBS=-ljemalloc


all:
	gcc ${SOURCES} ${INCLUDE} -o main.out
	gcc ${SOURCES} ${INCLUDE} -o main_jemalloc.out ${JEMALLOC_LIBS}