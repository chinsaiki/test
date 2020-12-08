/**
 * @file cl_buf.c
 * @brief cli buf
 * @details cli 缓存
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

#include "cli/cl_vty.h"
#include "cli/cl_buf.h"
#include "cli/cl_set.h"


#define    VOS_EWOULDBLOCK    70        /* Operation would block */

//extern int cl_writev( int fd, struct cl_lib_iovec * iovec, int index );

/* Make buffer data. */
struct buffer_data *buffer_data_new ( unsigned long size )
{
    struct buffer_data *d;

    d = ( struct buffer_data * ) VOS_Malloc ( sizeof ( struct buffer_data ), MODULE_RPU_CLI );
    if ( d == NULL )
    {
        return NULL;
    }
    VOS_MemZero ( ( char * ) d, sizeof ( struct buffer_data ) );
    d->data = ( unsigned char * ) VOS_Malloc ( size, MODULE_RPU_CLI );
    if ( d->data == NULL )
    {
        VOS_Free( d );
        return NULL;
    }
    VOS_MemZero( d->data, size );
    return d;
}

void buffer_data_free ( struct buffer_data * d )
{
    if ( d->data )
    {
        VOS_Free( d->data );
    }
    VOS_Free( d );
}

/* Make new buffer. */
struct buffer *buffer_new ( int type, unsigned long size )
{
    struct buffer *b;

    b = ( struct buffer * ) VOS_Malloc ( sizeof ( struct buffer ), MODULE_RPU_CLI );
    if ( b == NULL )
    {
        return NULL;
    }
    VOS_MemZero ( ( char * ) b, sizeof ( struct buffer ) );

    b->type = type;
    b->size = size;

    return b;
}

/* Free buffer. */
void buffer_free ( struct buffer * b )
{
    struct buffer_data *d;
    struct buffer_data *next;

    d = b->head;
    while ( d )
    {
        next = d->next;
        buffer_data_free ( d );
        d = next;
    }

    d = b->unused_head;
    while ( d )
    {
        next = d->next;
        buffer_data_free ( d );
        d = next;
    }

    VOS_Free ( b );
}

/* Make string clone. */
char *buffer_getstr ( struct buffer * b )
{
    return VOS_StrDup ( ( char * ) b->head->data, MODULE_RPU_CLI );
}

/* Return 1 if buffer is empty. */
int buffer_empty ( struct buffer * b )
{
    if ( b == NULL || b->tail == NULL || b->tail->cp == b->tail->sp )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* Clear and free all allocated data. */
void buffer_reset ( struct buffer * b )
{
    struct buffer_data *data;
    struct buffer_data *next;

    for ( data = b->head; data; data = next )
    {
        next = data->next;
        buffer_data_free ( data );
    }
    b->head = b->tail = NULL;
    b->alloc = 0;
    b->length = 0;
}

/* Add buffer_data to the end of buffer. */
int buffer_add ( struct buffer * b )
{
    struct buffer_data *d;

    d = buffer_data_new ( b->size );
    if ( d == NULL )
    {
        return 0 ;
    }

    if ( b->tail == NULL )
    {
        d->prev = NULL;
        d->next = NULL;
        b->head = d;
        b->tail = d;
    }
    else
    {
        d->prev = b->tail;
        d->next = NULL;

        b->tail->next = d;
        b->tail = d;
    }

    b->alloc++;

    return 1 ;
}


/* Write data to buffer. */
int buffer_write_vty_out ( struct buffer * b, unsigned char * ptr, unsigned long size )
{
/* B--remmed by liwei056@2014-6-26 for D20796 */
#if 0
    int nDivide1 = 0;
    int nDivide2 = 0;

    nDivide1 = b->length / BAC_BATCH_MSG_LEN ;
    nDivide2 = ( b->length + size ) / BAC_BATCH_MSG_LEN ;

    if ( nDivide2 > nDivide1 )/*填充64k空间的剩余部分，据说是打包需要*/
    {
        long size_space = 0;
        unsigned char * spaceptr = NULL ;

        size_space = nDivide2 * BAC_BATCH_MSG_LEN - b->length ;
        if ( size_space < 0 )
        {
            ASSERT( 0 );
            return 0;
        }
        spaceptr = VOS_Malloc( size_space , MODULE_RPU_CLI );
        if ( spaceptr == NULL )
        {
            buffer_reset( b ) ;
            return 0;
        }

        VOS_MemZero ( spaceptr , size_space );
        {
            LONG i = 0;
            spaceptr[size_space-1] = '\n';
            for(i = 0; i < size_space-1; i++)
            {
                spaceptr[i] = ' ';
            }
        }

        if ( buffer_write( b, spaceptr , size_space ) == 0 )
        {
            VOS_Free( spaceptr );
            return 0;
        }

        VOS_Free( spaceptr );
    }
#endif
/* E--remmed by liwei056@2014-6-26 for D20796 */

    if ( buffer_write( b, ptr , size ) == 0 )
    {
        return 0 ;
    }

    return 1;

}



/* Write data to buffer. If no memery , free all memory of  b */
int buffer_write ( struct buffer * b, unsigned char * ptr, unsigned long size )
{
    struct buffer_data *data;

    data = b->tail;
    b->length += size;

    /* We use even last one byte of data buffer. */
    while ( size )
    {
        /* If there is no data buffer add it. */
        if ( data == NULL || data->cp == b->size )
        {
            if ( buffer_add ( b ) == 0 )
            {
                buffer_reset( b ) ;
                VOS_ASSERT( 0 );
                return 0 ;
            }
            data = b->tail;
        }

        /* Last data. */
        /* 如果缓冲区空间够用，将所有的新的数据全部拷贝到缓冲区尾部。 */
        if ( size <= ( b->size - data->cp ) )
        {
            VOS_MemCpy ( ( data->data + data->cp ), ptr, size );

            data->cp += size;
            size = 0;
        }
        /* 否则，将部分数据拷贝到缓冲区尾部，之后再分配新的缓冲区。 */
        else
        {
            VOS_MemCpy ( ( data->data + data->cp ), ptr, ( b->size - data->cp ) );

            size -= ( b->size - data->cp );
            ptr += ( b->size - data->cp );

            data->cp = b->size;
        }
    }

    return 1;
}


/* Insert character into the buffer. */
int buffer_putc ( struct buffer * b, unsigned char c )
{
    if ( buffer_write ( b, &c, 1 ) == 0 )
    {
        return 0 ;
    }

    return 1;
}

/* Insert word (2 octets) into ther buffer. */
int buffer_putw ( struct buffer * b, unsigned short c )
{
    if ( buffer_write ( b, ( unsigned char * ) &c, 2 ) == 0 )
    {
        return 0 ;
    }
    return 1;
}

/* Put string to the buffer. */
int buffer_putstr ( struct buffer * b, unsigned char * c )
{
    unsigned long size;

    size = VOS_StrLen ( ( char * ) c );

    if ( buffer_write ( b, c, size ) == 0 )
    {
        return 0 ;
    }

    return 1;
}

#if 0 
/* Flush specified size to the fd. */
void buffer_flush ( struct buffer * b, int fd, unsigned long size )
{
    int iov_index;
    struct cl_lib_iovec *iovec;

    struct buffer_data *data; /* data指向当前还没全部输出到fd或者还没输出到fd的缓冲区。 */
    struct buffer_data *out;
    struct buffer_data *next;

    /* iovec按照最大的可能分配. */
    iovec = ( struct cl_lib_iovec * ) VOS_Malloc ( sizeof ( struct cl_lib_iovec ) * b->alloc, MODULE_RPU_CLI );
    if ( iovec == NULL )    /*add by liuyanjun 2002/08/15 */
    {
        ASSERT( 0 );
        return ;
    }               /* end */
    iov_index = 0;

    for ( data = b->head; data; data = data->next )
    {
        iovec[ iov_index ].iov_base = ( char * ) ( data->data + data->sp );

        /* 如果输出的数据的大小或等于当前缓冲区中的数据的     */
        /* 长度，那么在当前缓冲区中取出size大小的数据来即可。 */
        /* data是最后一个需要取数据的缓冲区。                 */
        if ( size <= ( data->cp - data->sp ) )
        {
            iovec[ iov_index++ ].iov_len = ( int ) size;
            data->sp += size;
            /* 如果当前缓冲区所有的数据已经取完，取下个缓冲区的数据。 */
            if ( data->sp == data->cp )
            {
                data = data->next;
            }
            break;
        }
        /* 否则，取出当前缓冲区中的所有数据。 */
        else
        {
            iovec[ iov_index++ ].iov_len = ( int ) ( data->cp - data->sp );
            size -= data->cp - data->sp;
            data->sp = data->cp;
        }
    }

    /* Write buffer to the fd. */
    if ( fd == _CL_FLASH_FD_ )
    {
        /*
        do nothing
        */
    }
    else
    {
        cl_writev ( fd, iovec, iov_index );
    }

    /* Free printed buffer data. */
    /* 释放从链表头到data的所有的缓冲区。 */
    for ( out = b->head; out && out != data; out = next )
    {
        next = out->next;
        if ( next )
        {
            next->prev = NULL;
        }
        else
        {
            b->tail = next;
        }
        b->head = next;

        buffer_data_free ( out );
        b->alloc--;
    }

    VOS_Free ( iovec );
}
#endif /* if 0 */

int buffer_flush_vty ( struct buffer * b, int fd, unsigned int size,
                       int erase_flag, int no_more_flag )
{
    int nbytes = 0;

#if 0 
    /*    int iov_index;

    struct cl_lib_iovec *iov;
    struct cl_lib_iovec small_iov[3];
    */
#endif
#if (OS_LINUX)

    char more[] = "\x1b\x5b\x37\x6d --Press any key to continue Ctrl+c to stop-- \x1b\x5b\x6d";
#else

    char more[] = " --Press any key to continue Ctrl+c to stop--";
#endif

    char erase[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                     ' ', ' ', ' ', ' ', ' ', ' ',
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                     0x08, 0x08, 0x08, 0x08, 0x08, 0x08
                   };
    struct buffer_data *data;
    struct buffer_data *out;
    struct buffer_data *next;

    /* For erase and more data add two to b's buffer_data count. */
#if 0

    if ( b->alloc == 1 )
    {
        iov = small_iov;
    }
    else
    {
        iov = ( struct cl_lib_iovec * ) VOS_Malloc( sizeof ( struct cl_lib_iovec ) * ( b->alloc + 2 ), MODULE_RPU_CLI );
        if ( iov == NULL )      /*add by liuyanjun 2002/08/15 */
        {
            ASSERT( 0 );
            return -1;
        }               /* end */

    }
    iov_index = 0;
#endif

    data = b->head;


    /* Previously print out is performed. */
    if ( erase_flag )
    {
#if 0
        iov[ iov_index ].iov_base = erase;
        iov[ iov_index ].iov_len = sizeof erase;
        iov_index++;
#endif

        cl_write( fd, erase, sizeof erase );
    }

    /* Output data. */
    for ( data = b->head; data; data = data->next )
    {
        /* iov[iov_index].iov_base = (char *) (data->data + data->sp); */

        /* 如果输出的数据的大小或等于当前缓冲区中的数据的     */
        /* 长度，那么在当前缓冲区中取出size大小的数据来即可。 */
        /* data是最后一个需要取数据的缓冲区。                 */
        if ( size <= ( data->cp - data->sp ) )
        {
            /* iov[iov_index++].iov_len = (int)size; */

            cl_write( fd, ( char * ) ( data->data + data->sp ), size );

            data->sp += size;
            /* 如果当前缓冲区所有的数据已经取完，取下个缓冲区的数据。 */
            if ( data->sp == data->cp )
            {
                data = data->next;
            }
            break;
        }
        /* 否则，取出当前缓冲区中的所有数据。 */
        else
        {
            /* iov[iov_index++].iov_len = (int)(data->cp - data->sp); */

            cl_write( fd, ( char * ) ( data->data + data->sp ), ( int ) ( data->cp - data->sp ) );

            size -= ( data->cp - data->sp );
            data->sp = data->cp;
        }
    }
    /* In case of `more' display need. */
    if ( !buffer_empty ( b ) && !no_more_flag )
    {
#if 0
        iov[ iov_index ].iov_base = ;
        iov[ iov_index ].iov_len = sizeof more;
        iov_index++;
#endif

        cl_write( fd, ( char * ) more, sizeof more );
    }

    /* We use write or writev */
    if ( fd == _CL_FLASH_FD_ )
    {
        /*
        do nothing .
        */
    }
    else
    {
        /* nbytes = cl_writev (fd, iov, iov_index); */
    }

    /* Error treatment. */
    if ( nbytes < 0 )
    {
        if ( errno == EINTR )
        {}
        else
        {}
        ;
        if ( errno == VOS_EWOULDBLOCK )
        {}
        else
        {}
        ;
    }

    /* Free printed buffer data. */
    /* 释放从链表头到data的所有的缓冲区。 */
    for ( out = b->head; out && out != data; out = next )
    {
        next = out->next;
        if ( next )
        {
            next->prev = NULL;
        }
        else
        {
            b->tail = next;
        }
        b->head = next;

        b->alloc--;
        b->length -= out->cp;
        buffer_data_free ( out );


    }

#if 0
    if ( iov != small_iov )
    {
        VOS_Free( iov );
    }
#endif

    return nbytes;
}


/* Calculate size of outputs then flush buffer to the file
   descriptor. */
int buffer_flush_window ( struct buffer * b, int fd, int width, int height,
                          int erase, int no_more )
{
    unsigned long cp;
    unsigned long size;
    int lineno;
    int ret;
    struct buffer_data *data;

    if ( height >= 2 )
    {
        height--;
    }

    /* We have to calculate how many bytes should be written. */
    lineno = 0;
    size = 0;
    if ( width == 0 )
        lineno++;

    for ( data = b->head; data; data = data->next )
    {
        cp = data->sp;

        while ( cp < data->cp )
        {
            if ( data->data[ cp ] == '\n' )
            {
                lineno++;
                if ( lineno == height )
                {
                    cp++;
                    size++;
                    goto flush;
                }
            }
            cp++;
            size++;
        }
    }

    /* Write data to the file descriptor. */
flush:

    ret = buffer_flush_vty ( b, fd, size, erase, no_more );

    return ret;
}

/* Flush all buffer to the fd. */
int buffer_flush_all ( struct buffer * b, int fd )
{
    int ret = 0;
    struct buffer_data *d;

#if 0

    int iov_index;
    struct cl_lib_iovec *iovec;
#endif

    if ( buffer_empty ( b ) )
    {
        return 0;
    }

#if 0
    iovec = ( struct cl_lib_iovec * ) VOS_Malloc ( sizeof ( struct cl_lib_iovec ) * b->alloc, MODULE_RPU_CLI );
    if ( iovec == NULL )    /*add by liuyanjun 2002/08/15 */
    {
        ASSERT( 0 );
        return 0;
    }                   /* end */

    iov_index = 0;
#endif

    for ( d = b->head; d; d = d->next )
    {
#if 0
        iovec[ iov_index ].iov_base = ( char * ) ( d->data + d->sp );
        iovec[ iov_index ].iov_len = ( int ) ( d->cp - d->sp );
        iov_index++;
#endif

        cl_write( fd, ( char * ) ( d->data + d->sp ), ( int ) ( d->cp - d->sp ) );
    }
    if ( fd == _CL_FLASH_FD_ )
    {
        /*
        do nothing .
        */
    }
    else
    {
        /* ret = cl_writev (fd, iovec, iov_index); */
    }

    /*    VOS_Free (iovec); */

    buffer_reset ( b );

    return ret;
}

void buffer_dump ( struct buffer * b )
{
}

#ifdef __cplusplus
}
#endif


