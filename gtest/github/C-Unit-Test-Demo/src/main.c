#include <stdio.h>

#include "my_lib/A.h"
#include "my_lib/Microwave.h"
#include "my_lib/Radio.h"
#include "my_lib/Switcher.h"

int main(void)
{
    printf("Init A with 17...\n");
    A_Handle a = A_create(17);

    printf("Calling A with 5\n");
    int resultA = A_multiplyWithInitial(a, 5);

    printf("Result: %d\n", resultA);


    printf("\n\nLet's do some switching!\n");


    Radio_Handle radio = Radio_create();
    Switchable* radioSwitch = Radio_getSwitchableInterface(radio);

    Microwave_Handle microwave = Microwave_create();
    Switchable* microwaveSwitch = Microwave_getSwitchableInterface(microwave);



    printf("\nLet's start with the radio!\n");
    Switcher_Handle switcher = Switcher_create(radioSwitch);
    Switcher_doSomeSwitching(switcher, 5);

    printf("\n...let's switch to the microwave now - during runtime!\n");

    Switcher_updateSwitchable(switcher, microwaveSwitch);
    Switcher_doSomeSwitching(switcher, 7);


    return 0;
}