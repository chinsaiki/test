/**
 *  bitset.h
 *  
 *  提供简单 bitmap 操作
 *  
 *  荣涛  2021年1月13日
 */

#ifndef __CRYPTO_BITS_TYPES_BITS_SET_H
#define __CRYPTO_BITS_TYPES_BITS_SET_H 1


/**
 *  Crypto
 */
#define __BITS_SETSIZE  256
#define __NBITS     (8 * (int) sizeof (__bits_mask))
#define __BITS_ELT(d)   ((d) / __NBITS)
#define __BITS_MASK(d)  ((__bits_mask)(1UL << ((d) % __NBITS)))

typedef long int __bits_mask;
typedef struct {
    __bits_mask __bits[__BITS_SETSIZE/__NBITS];
#define __BITS(set) ((set)->__bits)
#define __BITS_SET_INITIALIZER    {0}
}__bits_set;


#define __BITS_ZERO(s) \
    do {    \
        unsigned int __i;   \
        __bits_set *__arr = (s);  \
        for (__i = 0; __i < sizeof (__bits_set) / sizeof (__bits_mask); ++__i)    \
            __BITS (__arr)[__i] = 0;  \
    } while (0)
#define __BITS_SET(d, s) \
    ((void) (__BITS (s)[__BITS_ELT(d)] |= __BITS_MASK(d)))
#define __BITS_CLR(d, s) \
    ((void) (__BITS (s)[__BITS_ELT(d)] &= ~ __BITS_MASK(d)))
#define __BITS_ISSET(d, s) \
    ((__BITS (s)[__BITS_ELT (d)] & __BITS_MASK (d)) != 0)



typedef __bits_set bits_set;
#define BITS_SET_INITIALIZER __BITS_SET_INITIALIZER


#define BITS_SETSIZE                    __BITS_SETSIZE
#define BITS_SET(bit, p_bits_set)       __BITS_SET(bit, p_bits_set)
#define BITS_CLR(bit, p_bits_set)       __BITS_CLR(bit, p_bits_set)
#define BITS_ISSET(bit, p_bits_set)     __BITS_ISSET(bit, p_bits_set)
#define BITS_ZERO(p_bits_set)           __BITS_ZERO(p_bits_set)



#endif /*<__CRYPTO_BITS_TYPES_BITS_SET_H>*/

