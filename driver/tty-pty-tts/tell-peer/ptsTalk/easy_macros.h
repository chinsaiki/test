/**********************************************************************************************************************
** 
** Copyright (c) 2008-2018 ICT/CAS.
** All rights reserved.
**
** File name: easy_macros.h
** Description: 定义了一些公共的宏
**
** Current Version: 
** $Revision$ 
** Author: Rong Tao
** Date: 2019.10
**
***********************************************************************************************************************/
#ifndef __TOOLS_ATTRIBUTE_H
#define __TOOLS_ATTRIBUTE_H 1

/* Dependencies ------------------------------------------------------------------------------------------------------*/
//#include "vos_ctype.h"
//#include "vos_common.h"

/* Constants ---------------------------------------------------------------------------------------------------------*/
/* Types -------------------------------------------------------------------------------------------------------------*/
/* Macros ------------------------------------------------------------------------------------------------------------*/
#ifndef and
#define and &&
#endif
#ifndef or
#define or  ||
#endif


#ifdef __GNUC__
#define _packed             __attribute__((packed))
#define _noreturn           __attribute__((noreturn))
#define _unused             __attribute__((unused))
#define _used               __attribute__((used))
#define _const              __attribute__((const))
#define _api                __attribute__((visibility("default")))
#define _hidden             __attribute__((visibility("hidden")))
#define _aligned(x)         __attribute__((aligned(x)))
#define _format(func, a, b) __attribute__((__format__ (func, a, b)))
#define _bitwise            __attribute__((bitwise))
#ifndef _inline
#define _inline inline      __attribute__((always_inline))
#endif
#define _nonnull            __nonnull
#define _alias(symbol)      __attribute__((__alias__(#symbol)))
#define _aligned_largest    __attribute__((__aligned__))
#define likely(exp)         __builtin_expect(!!(exp), 1)
#define unlikely(exp)       __builtin_expect(!!(exp), 0)
#define prefetch(x)         __builtin_prefetch(x)       /* put addr to the Cache before use */
#define prefetchw(x)        __builtin_prefetch(x,1)

#else//__GNUC__
#define _packed             
#define _noreturn           
#define _unused   
#define _used
#define _const              
#define _api                
#define _hidden             
#define _aligned(x)         
#define _format(func, a, b) 
#define _bitwise 
#define _inline
#define _nonnull            
#define _alias(symbol)
#define _aligned_largest
#define likely(exp)         (exp)
#define unlikely(exp)       (exp)
#define prefetch(x)         (x)       /* put addr to the Cache before use */
#define prefetchw(x)        (x)

#endif//__GNUC__



#ifndef alignof
#define alignof(s) __alignof__(s)
#endif

#ifndef weak_alias
#define weak_alias(name, aliasname) extern typeof (name) aliasname __attribute__ ((weak, alias(#name)))
#endif
#ifndef strong_alias
#define strong_alias(name, aliasname)  extern __typeof (name) aliasname __attribute__ ((alias (#name)));
#endif
#ifndef weak_hidden_alias
#define weak_hidden_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name), __visibility__ ("hidden")));
#endif
#define weak_function __attribute__ ((weak))
#define weak_const_function __attribute__ ((weak, __const__))


#ifndef _toString
#define _toString(val) #val
#endif

#ifndef offsetof
#if 1
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)/* see also: __builtin_offsetof(TYPE, MEMBER) */
#else
#define offsetof(TYPE, MEMBER) __builtin_offsetof(type, member)
#endif
#endif//offsetof

#ifndef containof
#define containof(ptr, type, member) ((type *)((char *)(ptr) - (size_t)&((type *)0)->member))
#endif 



/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 */
#ifndef container_of
#define container_of(ptr, type, member) ((type *) ((uint8_t *)(ptr) - offsetof(type, member)))
#endif


#ifndef indexofarray 
#define indexofarray(type, startAddr, endAddr) (ABS(((unsigned long)endAddr)-((unsigned long)startAddr))/sizeof(type))
#endif


/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#ifndef typecheck
#define typecheck(type,x) ({type __dummy; typeof(x) __dummy2; (void)(&__dummy == &__dummy2);1;})
#endif

/*
 * Check at compile time that 'function' is a certain type, or is a pointer
 * to that type (needs to use typedef for the function type.)
 */
#ifndef typecheck_fn
#define typecheck_fn(type,function) ({	typeof(type) __tmp = function;(void)__tmp;})
#endif



/**
 *  x=8: 0000 0000 0000 1000; location of suffix "1" . 
 *  1000 -> 4 
 */
#ifndef ffs
#define ffs(integer) __builtin_ffs(integer)
#endif

/**
 *  x=8: 0000 0000 0000 1000; number of "1" 
 *  8 -> 1
 */
#ifndef popcount
#define popcount(integer) __builtin_popcount(integer)
#endif

/**
 *  x=8: 0000 0000 0000 1000;  number of low "0" 
 *  8 -> 3
 */
#ifndef ctz
#define ctz(x) __builtin_ctz(x)
#endif


/**
 *  x=8: 0000 0000 0000 1000;  number of high "0" 
 *  8 -> 28
 */
#ifndef clz
#define clz(x) __builtin_clz(x)
#endif

/**
 *  x=8: 0000 0000 0000 1000;  parity of "1" 
 *  8 -> 0
 *  12 -> 0
 *  13 -> 1
 *  1 -> 1
 */
#ifndef parity
#define parity(x) __builtin_parity(x)
#endif

#ifndef __OPTIMIZE__

#ifndef frame_address
#define frame_address(level_integer) __builtin_frame_address(level_integer)
#define frame_address0               __builtin_frame_address(0)
#define frame_address1               __builtin_frame_address(1)
#define frame_address2               __builtin_frame_address(2)
#define frame_address3               __builtin_frame_address(3)
/*...*/
#endif

#ifndef return_address
#define return_address(level_integer)   __builtin_return_address(level_integer)
#define return_address0                 __builtin_return_address(0)
#define return_address1                 __builtin_return_address(1)
#define return_address2                 __builtin_return_address(2)
#define return_address3                 __builtin_return_address(3)
/*...*/
#endif

#else //__OPTIMIZE__

#ifndef frame_address
#define frame_address(level_integer) (NULL)
#define frame_address0               (NULL)
#define frame_address1               (NULL)
#define frame_address2               (NULL)
#define frame_address3               (NULL)
/*...*/
#endif

#ifndef return_address
#define return_address(level_integer)   (NULL)
#define return_address0                 (NULL)
#define return_address1                 (NULL)
#define return_address2                 (NULL)
#define return_address3                 (NULL)
/*...*/
#endif
#endif //__OPTIMIZE__

/**
 *  To use , you must compile like "gcc XXX.c -O2" (优化选项: -O,-O0,-O1,-O2,-O3)
 */
#ifndef object_size
#define object_size(p_obj, type)    __builtin_object_size(p_obj, type)
#define object_size0(p_obj)         __builtin_object_size(p_obj, 0)
#define object_size1(p_obj)         __builtin_object_size(p_obj, 1)
#define object_size2(p_obj)         __builtin_object_size(p_obj, 2)
#define object_size3(p_obj)         __builtin_object_size(p_obj, 3)
#endif



/* is constant during compiled:  yes -> 1, no -> 0 */
#ifndef constant_p
#define constant_p(v) __builtin_constant_p(v)
#endif

/* compare t1 with t2 is same as each other: yes -> 1, no -> 0 */
#ifndef types_cmp_p
#define types_cmp_p(t1, t2) __builtin_types_compatible_p(t1, t2)
#endif

#define _fetch_and_add(ptr, v)  __sync_fetch_and_add(ptr, v)
#define _fetch_and_sub(ptr, v)  __sync_fetch_and_sub(ptr, v)
#define _fetch_and_or(ptr, v)  __sync_fetch_and_or(ptr, v)
#define _fetch_and_and(ptr, v)  __sync_fetch_and_and(ptr, v)
#define _fetch_and_xor(ptr, v)  __sync_fetch_and_xor(ptr, v)
#define _fetch_and_nand(ptr, v)  __sync_fetch_and_nand(ptr, v)


#define _add_and_fetch(ptr, v)  __sync_add_and_fetch(ptr, v)
#define _sub_and_fetch(ptr, v)  __sync_sub_and_fetch(ptr, v)
#define _or_and_fetch(ptr, v)  __sync_or_and_fetch(ptr, v)
#define _and_and_fetch(ptr, v)  __sync_and_and_fetch(ptr, v)
#define _xor_and_fetch(ptr, v)  __sync_xor_and_fetch(ptr, v)
#define _nand_and_fetch(ptr, v)  __sync_nand_and_fetch(ptr, v)

#define _synchronize(...)  __sync_synchronize(__VA_ARGS__)
#define _bool_compare_and_swap(ptr, oldval, newold) __sync_bool_compare_and_swap(ptr, oldval, newold)
#define _val_compare_and_swap(ptr, oldval, newold) __sync_val_compare_and_swap(ptr, oldval, newold)
#define CAS _val_compare_and_swap

#ifndef VCAS
#undef VCAS
#define VCAS(ptr, oldval, newold) __sync_val_compare_and_swap(ptr, oldval, newold)
#endif

#ifndef CAS
#undef CAS
#define CAS(loc, old_value, new_value) __sync_bool_compare_and_swap((void **)loc, old_value, new_value)
#endif

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))


#ifndef _swapbyte16
#define _swapbyte16(s16) (((s16>>8)&0xff) | ((s16&0xff)<<8))
#endif
#ifndef _swapbyte32
#define _swapbyte32(i32) (((i32>>24)&0xff) | ((i32&0xff)<<24) |\
                          ((i32>>8)&0xff00) | ((i32&0xff00)<<8))
#endif
#ifndef _swapbyte64
#define _swapbyte64(l64) (((l64>>56)&0xff)       | ((l64&0xff)<<56) |\
		                  ((l64>>40)&0xff00)     | ((l64&0xff00)<<40) |\
		                  ((l64>>24)&0xff0000)   | ((l64&0xff0000)<<24) |\
		                  ((l64>> 8)&0xff000000) | ((l64&0xff000000)<<8))
#endif


#define __called_once_and_exit() \
    do{ \
        static int __call_times =  0; \
        __call_times++; \
        if(__call_times>1) { \
            exit(0);\
        } \
    }while(0)
    
#define __called_once_and_warn() \
    do{ \
        static int __call_times =  0; \
        __call_times++; \
        if(__call_times>1) { \
        } \
    }while(0)
    
#define __called_once_and_goto(jmp_loop) \
    do{ \
        static int __call_times =  0; \
        __call_times++; \
        if(__call_times>1) { \
            goto jmp_loop;\
        } \
    }while(0)


#define __chk_null_and_return_err(p, ret) \
    do{ \
        if(unlikely(p == NULL)){\
            return ret;\
        }\
    }while(0)
            
#define __chk_null_and_jmp(p, jmppoint) \
    do{ \
        if(unlikely(p == NULL)){\
            goto jmppoint;\
        }\
    }while(0)

                    
#define __chk_exp_and_jmp(p, jmppoint) \
    do{ \
        if(unlikely((p))){\
            goto jmppoint;\
        }\
    }while(0)

                       
#define __chk_exp_and_exe(p, exe) \
    do{ \
        if(unlikely((p))){\
            exe;\
        }\
    }while(0)

#define ONCE(flag) \
    static int _used __just_exe_once##flag = 0;\
    if(__just_exe_once##flag++ == 0)


#ifdef	__cplusplus
 #ifndef __BEGIN_DECLS
  #define __BEGIN_DECLS	extern "C" {
 #endif
 #ifndef __END_DECLS
  #define __END_DECLS	}
 #endif
#else
 #ifndef __BEGIN_DECLS
  #define __BEGIN_DECLS
 #endif
 #ifndef __END_DECLS
  #define __END_DECLS
 #endif
#endif


/* Globals -----------------------------------------------------------------------------------------------------------*/
/* Functions ---------------------------------------------------------------------------------------------------------*/



#endif /*<__TOOLS_ATTRIBUTE_H>*/



