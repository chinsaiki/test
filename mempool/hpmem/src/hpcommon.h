#ifndef __HPCOMMON_H
#define __HPCOMMON_H 1

/*--------------------------------------------------------------------------------------------------------------------*\
 *      宏定义
\*--------------------------------------------------------------------------------------------------------------------*/
#ifndef unlikely
#define unlikely(x)    (__builtin_expect(!!(x), 0))
#endif

#ifndef likely
#define likely(x)    (__builtin_expect(!!(x), 1))
#endif

#ifndef WRITE_ONCE
#define WRITE_ONCE(var, val) (*((volatile typeof(val) *)(&(var))) = (val))
#endif

#ifndef READ_ONCE
#define READ_ONCE(var) (*((volatile typeof(var) *)(&(var))))
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif 

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#ifndef container_of
#define container_of(ptr, type, member) ({          \
	const typeof(((type *)0)->member)*__mptr = (ptr);    \
		     (type *)((char *)__mptr - offsetof(type, member)); })
#endif



/* Double expansion needed for stringification of macro values. */
#define __xstr(s) __str(s)
#define __str(s) #s

/*--------------------------------------------------------------------------------------------------------------------*\
 *  版本号生成
\*--------------------------------------------------------------------------------------------------------------------*/
#if defined(USE_TCMALLOC)
 #define HPMALLOC_LIB ("tcmalloc-" __xstr(TC_VERSION_MAJOR) "." __xstr(TC_VERSION_MINOR))
 #include <google/tcmalloc.h>
 #include <gperftools/tcmalloc.h>
 #if (TC_VERSION_MAJOR == 1 && TC_VERSION_MINOR >= 6) || (TC_VERSION_MAJOR > 1)
 #else
  #error "Newer version of tcmalloc required"
 #endif

#elif defined(USE_JEMALLOC)
 #define HPMALLOC_LIB ("jemalloc-" __xstr(JEMALLOC_VERSION_MAJOR) "." __xstr(JEMALLOC_VERSION_MINOR) "." __xstr(JEMALLOC_VERSION_BUGFIX))
 #include <jemalloc/jemalloc.h>
 #if (JEMALLOC_VERSION_MAJOR == 2 && JEMALLOC_VERSION_MINOR >= 1) || (JEMALLOC_VERSION_MAJOR > 2)
 #else
  #error "Newer version of jemalloc required"
 #endif
#else
 #define HPMALLOC_LIB ("libc-" __xstr(__GLIBC__) "." __xstr(__GLIBC_MINOR__))
 #include <malloc.h>
 #if (__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1)
 #else
  #error "Newer version of glibc required"
 #endif
#endif


/*--------------------------------------------------------------------------------------------------------------------*\
 *  使用原子操作，规避同步锁性能问题
\*--------------------------------------------------------------------------------------------------------------------*/
/* To test Redis with Helgrind (a Valgrind tool) it is useful to define
 * the following macro, so that __sync macros are used: those can be detected
 * by Helgrind (even if they are less efficient) so that no false positive
 * is reported. */
// #define __ATOMIC_VAR_FORCE_SYNC_MACROS

#if !defined(__ATOMIC_VAR_FORCE_SYNC_MACROS) && defined(__ATOMIC_RELAXED) && !defined(__sun) && (!defined(__clang__) || !defined(__APPLE__) || __apple_build_version__ > 4210057)
/* Implementation using __atomic macros. "atomic-builtin" */

#define atomic_incr(var,count) __atomic_add_fetch(&var,(count),__ATOMIC_RELAXED)
#define atomic_get_incr(var,oldvalue_var,count) do { \
    oldvalue_var = __atomic_fetch_add(&var,(count),__ATOMIC_RELAXED); \
} while(0)
#define atomic_decr(var,count) __atomic_sub_fetch(&var,(count),__ATOMIC_RELAXED)
#define atomic_get(var,dstvar) do { \
    dstvar = __atomic_load_n(&var,__ATOMIC_RELAXED); \
} while(0)
#define atomic_set(var,value) __atomic_store_n(&var,value,__ATOMIC_RELAXED)

#elif defined(HAVE_ATOMIC)
/* Implementation using __sync macros. "sync-builtin" */

#define atomic_incr(var,count) __sync_add_and_fetch(&var,(count))
#define atomic_get_incr(var,oldvalue_var,count) do { \
    oldvalue_var = __sync_fetch_and_add(&var,(count)); \
} while(0)
#define atomic_decr(var,count) __sync_sub_and_fetch(&var,(count))
#define atomic_get(var,dstvar) do { \
    dstvar = __sync_sub_and_fetch(&var,0); \
} while(0)
#define atomic_set(var,value) do { \
    while(!__sync_bool_compare_and_swap(&var,var,value)); \
} while(0)

#endif


#endif /*<__HPCOMMON_H>*/
