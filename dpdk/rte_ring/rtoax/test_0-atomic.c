/***********************************************************************************\
 *  文件名   ：test_0.c
 *
 *  描述：原子操作 测试代码
 *  作者：荣涛
 *  日期：2021年1月22日
 *  
\***********************************************************************************/

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>

#include "atomic.h"

#define log1(fmt...)  do{printf("\033[33m[%d]", getpid());printf(fmt);printf("\033[m");}while(0)
#define log2(fmt...)  do{printf("\033[32m[%d]", getpid());printf(fmt);printf("\033[m");}while(0)


int main(int argc,char *argv[])
{
    int i;
    
    atomic64_t a1;

    atomic64_init(&a1);
    log1("a1 = %ld\n", atomic64_read(&a1));
    atomic64_inc(&a1);
    log1("a1 = %ld\n", atomic64_read(&a1));

    atomic64_exchange(&a1, 12);
    log1("a1 = %ld\n", atomic64_read(&a1));

    atomic64_cmpset(&a1, 12, 19);
    log1("a1 = %ld\n", atomic64_read(&a1));

    atomic64_set(&a1, 112);
    log1("a1 = %ld\n", atomic64_read(&a1));

    atomic64_add(&a1, 112);
    log1("a1 = %ld\n", atomic64_read(&a1));

    atomic64_sub(&a1, 12);
    log1("a1 = %ld\n", atomic64_read(&a1));
    
    atomic64_dec(&a1);
    atomic64_dec(&a1);
    log1("a1 = %ld\n", atomic64_read(&a1));

    log1("a1 = %ld\n", atomic64_add_return(&a1, 10));
    log1("a1 = %ld\n", atomic64_sub_return(&a1, 10));
    

    atomic64_sub(&a1, atomic64_read(&a1)+1);
    log1("a1 = %ld\n", atomic64_read(&a1));
    log1("inc_and_test = %ld\n", atomic64_inc_and_test(&a1));
    log1("dec_and_test = %ld\n", atomic64_dec_and_test(&a1));

    
}


