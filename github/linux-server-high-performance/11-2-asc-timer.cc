#include <stdio.h>
#include "lst_timer.h"
#include "11-2-asc-timer.h"

void cb_func(client_data *data) /*任务回调函数*/
{
    printf("1\n");
}

static sort_timer_lst timer_lst;

int main()
{
    util_timer timer;

    timer.expire = 1;
    timer.cb_func = cb_func;
    
    timer_lst.add_timer(&timer);
    
    while(1);
}
