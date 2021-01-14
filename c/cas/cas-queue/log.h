#pragma once

#include <stdio.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/types.h>

#define gettid() syscall(__NR_gettid)

#ifndef NOLOG
#define log_enqueue(fmt...)  do{printf("\033[33m[%d]", gettid());printf(fmt);printf("\033[m");}while(0)
#define log_dequeue(fmt...)  do{printf("\033[32m[%d]", gettid());printf(fmt);printf("\033[m");}while(0)
#else
#define log_enqueue(fmt...)  
#define log_dequeue(fmt...)  

#endif

