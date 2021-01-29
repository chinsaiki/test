#ifndef SWITCHABLE_MOCK
#define SWITCHABLE_MOCK

#include "my_lib/Switchable.h"

typedef struct __SwitchableMock
{
    Switchable switchable;
    unsigned int turnOnCallCounter;
    unsigned int turnOffCallCounter;
    unsigned int getStateCallCounter;
    Switchable_State switchableState;
} SwitchableMock;

void SwitchableMock_initialize(SwitchableMock* mock);

#endif