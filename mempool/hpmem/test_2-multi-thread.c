#include <stdio.h>
#include "hpmalloc.h"

#define NR_MODULE 4
#define NR_THREAD_PER_MODULE 2

struct module_struct{
    module_id_t module_id;
    pthread_t threads[NR_THREAD_PER_MODULE];
    
};

static module_id_t moduleID[NR_MODULE] ={ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };
static struct module_struct modules[NR_MODULE];

void basic_test_clobbered(struct module_struct*module)
{
    char *ptr = hpmalloc(64, module->module_id);
    
//    *(char*)(ptr-1) = 1; //head 越界
//    *(char*)(ptr+64) = 1;//tail 越界
    *(char*)(ptr) = 1;
    *(char*)(ptr+63) = 1;
    
//    printf("%ld -> %s\n", pthread_self(), hpmalloc_strerror(hpmemcheck(ptr)));
    
    hpfree(ptr);
}

void *task_routine(void*arg)
{
    struct module_struct*module = (struct module_struct*)arg;

    static int count_loop = 0;
    
    basic_test_clobbered(module);

    char *ptr = hpmalloc(64, module->module_id);
    
    while(1) {
        printf("%3ld used %ld bytes.\n", module->module_id, hpmalloc_module_used_memory(module->module_id));
        hpfree(hpmalloc(64, module->module_id));
        sleep(1);
        ++count_loop;
        if(count_loop == 50) {
            *(char*)(ptr-1) = 1;//tail 越界
            sleep(1);
            hpfree(ptr);
        }
    }

    pthread_exit(NULL);
}

void init_module(module_id_t id, struct module_struct *module)
{
    module->module_id = id;

    int i;
    for(i=0;i<NR_THREAD_PER_MODULE;i++) {
        pthread_create(&module->threads[i], NULL, task_routine, module);
    }
}

void join_module(struct module_struct*module)
{
    int i;
    for(i=0;i<NR_THREAD_PER_MODULE;i++) {
        pthread_join(module->threads[i], NULL);
    }

}


int main(int argc, char **argv)
{
    printf("ready to test.\n");
    int i;
    for(i=0;i<NR_MODULE;i++) {
        init_module(moduleID[i], &modules[i]);
    }
    
    for(i=0;i<NR_MODULE;i++) {
        join_module(&modules[i]);
    }
    
}


