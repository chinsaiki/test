#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void alpha(void)
{
    printf("I am alpha.\n");
}

void bravo(void)
{
    printf("I am bravo.\n");
}

int main(void)
{
    while (true)
    {
        alpha();
        sleep(1);
    }
    exit(EXIT_SUCCESS);
}
