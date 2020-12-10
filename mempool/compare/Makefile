#########################################################################
#
#	对比内存申请的效率（耗费时间）
#
#########################################################################
#
#	荣涛		2020年11月17日
#	荣涛		2020年11月27日	添加 tcmalloc jemalloc的多线程对比
#########################################################################

#线程数
TEST_NTHREAD=1


#参加对比的库函数
TEST_GLIBC=y
TEST_OBSTACK=y
TEST_JEMALLOC=n
TEST_TCMALLOC=n
TEST_REDIS=n
TEST_NGINX=y
TEST_VMEM=n

DEFINE:= -DNR_THREAD=${TEST_NTHREAD}
LIBS:= -lpthread -lrt

SOURCES=compare.c
SOURCES+=main.c


#默写多线程情况下不支持的接口，这里不执行
ifneq ($(TEST_NTHREAD),1)
	TEST_OBSTACK=n
	TEST_NGINX=n
endif

ifeq ($(TEST_TCMALLOC),y)
	#如果使用tcmalloc，可执行程序的malloc接口默认使用tc_malloc
	TEST_GLIBC=n 
endif

ifeq ($(TEST_GLIBC),y)
	SOURCES+=malloc.c
	DEFINE+=-DPTMALLOC
endif


ifeq ($(TEST_OBSTACK),y)
	SOURCES+=obstack.c
	DEFINE+=-DOBSTACK
endif
#SOURCES+=../mempool/nmx_palloc.c \
#		 ../mempool/nmx_alloc.c \
#		 nmx.c

ifeq ($(TEST_JEMALLOC),y)
	SOURCES+=jemalloc.c
	DEFINE+=-DJEMALLOC
	LIBS+=-ljemalloc
endif

ifeq ($(TEST_TCMALLOC),y)
	SOURCES+=tcmalloc.c
	DEFINE+=-DTCMALLOC
	LIBS+=-ltcmalloc
endif
ifeq ($(TEST_NGINX),y)
	SOURCES+=../ncx_mempool/ncx_slab.c \
				ncx.c
	DEFINE+=-DNGINX -DLOG_LEVEL=8
endif			
ifeq ($(TEST_REDIS),y)
	SOURCES+=../../redis/zmalloc/redis-6.0.5/zmalloc.c \
				redis-6.0.5-zmalloc.c
	DEFINE+=-DREDIS
endif


ifeq ($(TEST_VMEM),y)
	SOURCES+=vmem.c
	DEFINE+=-DLIBVMEM
	LIBS+=-lvmem
endif


INCLUDE=-I./ 
INCLUDE+=-I../mempool
INCLUDE+=-I../ncx_mempool
INCLUDE+=-I../../redis/zmalloc/redis-6.0.5/




all:
#	gcc ${SOURCES} ${INCLUDE} -o main.out
	gcc ${SOURCES} ${INCLUDE} -o main.out ${LIBS} ${DEFINE}
