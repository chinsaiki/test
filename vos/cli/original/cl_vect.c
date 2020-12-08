
/**
 * @file cl_vect.c
 * @brief cli 
 * @details 动态数组库函数
 * @version 1.0
 * @author  wujianming  (wujianming@sylincom.com)
 * @date 2019.01.25
 * @copyright 
 *    Copyright (c) 2018 Sylincom.
 *    All rights reserved.
*/

#ifdef __cplusplus
extern"C"
{
#endif
#include "vos_global.h"
#include "vos_common.h"
#include "cli/cl_vect.h"


/* 功能相当于realloc,重新分配一块内存，将原来内存块中的内容拷贝到新的内存块中。 */
void *cl_realloc( void * ptr, unsigned long old_size, unsigned long new_size, int module_id )
{
    void * new_ptr;
    new_ptr = ( void * ) VOS_Malloc( new_size, module_id );
    if ( new_ptr == NULL )
        return NULL;
    VOS_MemCpy( new_ptr, ptr, ( new_size > old_size ) ? old_size : new_size );
    VOS_Free( ptr );
    ptr = new_ptr;

    return new_ptr;
}

/* 初始化向量，size指向量表的大小--可以容纳多少个向量。 */
/* Initialize cl_vector : allocate memory and return cl_vector. */
cl_vector cl_vector_init( unsigned int size )
{
    cl_vector v = ( cl_vector ) VOS_Malloc( sizeof ( struct _cl_vector ), MODULE_RPU_CLI );
    if ( v == NULL )
        return NULL;
    /* allocate at least one slot */
    if ( size == 0 )
    {
        size = 1;
    }

    v->alloced = size;
    v->max = 0;
    v->index = ( void * ) VOS_Malloc( sizeof ( void * ) * size, MODULE_RPU_CLI );
    if ( v->index == NULL )
    {
        VOS_Free( v );
        return NULL;
    }
    VOS_MemSet( v->index, 0, sizeof ( void * ) * size );

    return v;
}

/* 向量表是空的时候，向量表的释放函数。 */
void cl_vector_only_wrapper_free( cl_vector v )
{
    VOS_Free ( v );
}

/* 向量索引表为空表的时候，释放向量索引表。 */
void cl_vector_only_index_free( void * index )
{
    VOS_Free ( index );
}

/* 释放向量索引表和向量。 */
void cl_vector_free( cl_vector v )
{
    VOS_Free( v->index );
    VOS_Free ( v );
}

/* 将一个向量拷贝到另一个向量中。 */
cl_vector cl_vector_copy( cl_vector v )
{
    unsigned int size;
    cl_vector new = NULL;
    if ( NULL == v )
    {
        VOS_ASSERT( 0 );
        return NULL;
    }
    new = ( cl_vector ) VOS_Malloc( sizeof ( struct _cl_vector ), MODULE_RPU_CLI );
    if ( new == NULL ) 
        return NULL;

    new->max = v->max;
    new->alloced = v->alloced;

    size = sizeof ( void * ) * ( v->alloced );
    new->index = ( void * ) VOS_Malloc( size, MODULE_RPU_CLI );
    if ( new->index == NULL )
    {
        VOS_Free( new );
        return NULL;
    }
    VOS_MemCpy( new->index, v->index, size );

    return new;
}

/* 检查索引表大小，如果索引表小于num，将索引表扩大一倍，如此递归。 */
/* Check assigned index, and if it runs short double index pointer .
return :
0    failure
1    success
*/
int cl_vector_ensure( cl_vector v, unsigned int num )
{
    void * new_ptr = NULL;
    if ( NULL == v )
    {
        VOS_ASSERT( 0 );
        return 0;
    }
    if ( v->alloced > num )
    {
        return 1;
    }

    new_ptr = cl_realloc ( v->index, sizeof ( void * ) * v->alloced, sizeof ( void * ) * ( v->alloced * 2 ), MODULE_RPU_CLI );
    if ( new_ptr == NULL ) 
        return 0;
    v->index = new_ptr;
    VOS_MemSet ( &v->index[ v->alloced ], 0, sizeof ( void * ) * v->alloced );
    v->alloced *= 2;

    if ( v->alloced <= num )
    {
        int ret = 0;
        ret = cl_vector_ensure ( v, num );
        return ret;
    }
    return 1;
}

/* 在一个向量的索引表中查找一个空位。 */
/* This function only returns next empty slot index.  It dose not mean
   the slot's index memory is assigned, please call cl_vector_ensure()
   after calling this function. 
return :
0    failure
1    success
   */
int cl_vector_empty_slot( cl_vector v, unsigned int * num )
{
    unsigned int i;
    if ( NULL == v )
        return 0;

    if ( v->max == 0 )
    {
        *num = 0;
        return 1;
    }

    for ( i = 0; i < v->max; i++ )
    {
        if ( v->index[ i ] == 0 )
        {
            *num = i;
            return 1;
        }
    }

    *num = i;
    return 1;
}

/* 将一个指针存放在向量中。 */
/* Set value to the smallest empty slot. */
int cl_vector_set( cl_vector v, void * val )
{
    unsigned int i;

    if ( NULL == v )
    {
        VOS_ASSERT( 0 );
        return -1;
    }
    ;
    if ( ( 0 == cl_vector_empty_slot( v, &i ) ) ||
            ( 0 == cl_vector_ensure( v, i ) )
       )  /*此次插入操作失败*/
    {
        VOS_ASSERT( 0 );
        return -1;
    }

    // char *p = v->index[ i ];
    v->index[ i ] = val;


    if ( v->max <= i )
    {
        v->max = i + 1;
    }

    return i;
}

/* 将一个指针存放在向量表的指定位置上。 */
/* Set value to specified index slot. */
int cl_vector_set_index( cl_vector v, unsigned int i, void * val )
{
    if ( 0 == cl_vector_ensure( v, i ) )   /*此次插入操作无效*/
    {
        VOS_ASSERT( 0 );
        return -1;
    }

    v->index[ i ] = val;

    if ( v->max <= i )
    {
        v->max = i + 1;
    }

    return i;
}

/* 返回向量表中指定位置的指针。 */
/* Lookup cl_vector, ensure it. */
void *cl_vector_lookup_index( cl_vector v, unsigned int i )
{
    if ( 0 == cl_vector_ensure( v, i ) )   /*此次插入操作无效*/
    {
        VOS_ASSERT( 0 );
        return NULL;
    }
    return v->index[ i ];
}

/* 清空向量表的制定位置。 */
/* Unset value at specified index slot. */
void cl_vector_unset( cl_vector v, unsigned int i )
{
    if ( NULL == v )
    {
        VOS_ASSERT( 0 );
        return ;
    }
    if ( i >= v->alloced )
    {
        return ;
    }

    v->index[ i ] = NULL;

    if ( i + 1 == v->max )
    {
        v->max--;
        while ( i && v->index[ --i ] == NULL && v->max-- )
            ;                /* Is this ugly ? */
    }
}

/* 计算向量表中元素的个数。 */
/* Count the number of not emplty slot. */
unsigned int cl_vector_count( cl_vector v )
{
    unsigned int i;
    unsigned count = 0;

    if ( NULL == v )
    {
        VOS_ASSERT( 0 );
        return 0;
    }

    for ( i = 0; i < v->max; i++ )
    {
        if ( v->index[ i ] != NULL )
        {
            count++;
        }
    }

    return count;
}

int cl_vector_find( cl_vector v, void *val )
{
    unsigned int i;

    if ( NULL == v )
    {
        VOS_ASSERT( 0 );
        return -1;
    }

    for ( i = 0; i < v->max; i++ )
    {
        if ( v->index[ i ] == val )
        {
            return i;
        }
    }

    return -1;
}

/*  */
/* For debug, display  contents of cl_vector */
#if 0
void cl_vector_describe( FILE * fp, cl_vector v )
{
    int i;

    mn_fd_printf( VOS_console_fd, "vecotor max : %d\n", v->max );
    mn_fd_printf( VOS_console_fd, "vecotor alloced : %d\n", v->alloced );

    for ( i = 0; i < ( int ) ( v->max ); i++ )
    {
        if ( v->index[ i ] != NULL )
        {
            mn_fd_printf( VOS_console_fd, "cl_vector [%d]: %p\n", i, cl_vector_slot ( v, i ) );
        }
    }
}
#endif
#ifdef __cplusplus
}
#endif
