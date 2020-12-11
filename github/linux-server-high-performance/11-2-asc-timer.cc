#include <stdio.h>
#include "11-2-asc-timer.h"

void cb_func(client_data *) /*任务回调函数*/
{

}


int main()
{
    util_timer timer;

    timer.expire = 1;
    timer.cb_func = cb_func;

    
}
