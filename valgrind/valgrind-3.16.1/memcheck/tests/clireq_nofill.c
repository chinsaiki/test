#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "valgrind/valgrind.h"
#include "valgrind/memcheck.h"

//[root@localhost tests]# valgrind --quiet ./a.out 
//==128553== Use of uninitialised value of size 8
//==128553==    at 0x4E7E2D1: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x40098B: main (clireq_nofill.c:21)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E7E2D8: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x40098B: main (clireq_nofill.c:21)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E825FF: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x40098B: main (clireq_nofill.c:21)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80D2B: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x40098B: main (clireq_nofill.c:21)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80DAE: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x40098B: main (clireq_nofill.c:21)
//==128553== 
//*s=0x5 after MEMPOOL_ALLOC
//==128553== Invalid read of size 1
//==128553==    at 0x4009FF: main (clireq_nofill.c:23)
//==128553==  Address 0x5204040 is 0 bytes inside a recently re-allocated block of size 40 alloc'd
//==128553==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==128553==    by 0x4008D6: main (clireq_nofill.c:16)
//==128553== 
//*s=0x5 after MEMPOOL_FREE
//==128553== Use of uninitialised value of size 8
//==128553==    at 0x4E7E2D1: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400A9F: main (clireq_nofill.c:25)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E7E2D8: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400A9F: main (clireq_nofill.c:25)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E825FF: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400A9F: main (clireq_nofill.c:25)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80D2B: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400A9F: main (clireq_nofill.c:25)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80DAE: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400A9F: main (clireq_nofill.c:25)
//==128553== 
//*s=0x5 after second MEMPOOL_ALLOC
//==128553== Use of uninitialised value of size 8
//==128553==    at 0x4E7E2D1: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400BD8: main (clireq_nofill.c:34)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E7E2D8: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400BD8: main (clireq_nofill.c:34)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E825FF: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400BD8: main (clireq_nofill.c:34)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80D2B: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400BD8: main (clireq_nofill.c:34)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80DAE: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400BD8: main (clireq_nofill.c:34)
//==128553== 
//*s=0x5 after MALLOCLIKE_BLOCK
//==128553== Invalid read of size 1
//==128553==    at 0x400C4B: main (clireq_nofill.c:36)
//==128553==  Address 0x52040b0 is 0 bytes inside a recently re-allocated block of size 40 alloc'd
//==128553==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==128553==    by 0x400B24: main (clireq_nofill.c:29)
//==128553== 
//*s=0x5 after FREELIKE_BLOCK
//==128553== Use of uninitialised value of size 8
//==128553==    at 0x4E7E2D1: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400CDE: main (clireq_nofill.c:38)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E7E2D8: _itoa_word (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E825B0: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400CDE: main (clireq_nofill.c:38)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E825FF: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400CDE: main (clireq_nofill.c:38)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80D2B: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400CDE: main (clireq_nofill.c:38)
//==128553== 
//==128553== Conditional jump or move depends on uninitialised value(s)
//==128553==    at 0x4E80DAE: vfprintf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x4E894A8: printf (in /usr/lib64/libc-2.17.so)
//==128553==    by 0x400CDE: main (clireq_nofill.c:38)
//==128553== 
//*s=0x5 after second MALLOCLIKE_BLOCK


struct super { int x; };
static struct super superblock = { 12345 };

/* run with `valgrind -q --malloc-fill=0xaf --free-fill=0xdb` */
int main(int argc, char **argv)
{
    unsigned char *s;
    VALGRIND_CREATE_MEMPOOL(&superblock, /*rzB=*/0, /*is_zeroed=*/0);
    s = malloc(40);
    assert(s);
//    assert(*s == 0xaf);
    *s = 0x05;
    VALGRIND_MEMPOOL_ALLOC(&superblock, s, 40);
    printf("*s=%#hhx after MEMPOOL_ALLOC\n", *s);
    VALGRIND_MEMPOOL_FREE(&superblock, s);
    printf("*s=%#hhx after MEMPOOL_FREE\n", *s);
    VALGRIND_MEMPOOL_ALLOC(&superblock, s, 40);
    printf("*s=%#hhx after second MEMPOOL_ALLOC\n", *s);
    free(s);
    VALGRIND_DESTROY_MEMPOOL(&superblock);

    s = malloc(40);
    assert(s);
//    assert(*s == 0xaf);
    *s = 0x05;
    VALGRIND_MALLOCLIKE_BLOCK(s, 40, 0/*rzB*/, 0/*is_zeroed*/);
    printf("*s=%#hhx after MALLOCLIKE_BLOCK\n", *s);
    VALGRIND_FREELIKE_BLOCK(s, 0/*rzB*/);
    printf("*s=%#hhx after FREELIKE_BLOCK\n", *s);
    VALGRIND_MALLOCLIKE_BLOCK(s, 40, 0/*rzB*/, 0/*is_zeroed*/);
    printf("*s=%#hhx after second MALLOCLIKE_BLOCK\n", *s);

    return 0;
}


