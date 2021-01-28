/******************************************************************************
 * File name:   watchdog_test.c
 * Developer:   Lior Katz
 * Date:        2019-05-06 12:53:56
 * Reviewer:
 ******************************************************************************/

#include <stdio.h>     /* printf()     */
#include <stdlib.h>
#include <unistd.h>    /* sleep */
#include "watchdog.h"

/******************************************************************************
*                                  main                                      *
******************************************************************************/
int main(int argc, char const **argv)
{
    unsigned int remained_sleep = 30;

    printf("\n\n\tAPP IS ALIVE\n\n");
    printf("\n\nkeep me alive status: %d\n\n", WDKeepMeAlive(argc, (char **)argv));

    while (remained_sleep != 0)
    {
        remained_sleep = sleep(remained_sleep);
    }

    printf("\n\n\tcomeon\n\n");
    WDLetMeDie();
    printf("\n\n\tAPP IS DEAD\n\n");

    return 0;
}
