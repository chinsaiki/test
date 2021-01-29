#include "Switcher.h"
#include <stdlib.h>
#include <assert.h>


typedef struct Switcher_PrivateData
{
    Switchable* switchable;
} PrivateData;


Switcher_Handle Switcher_create(Switchable* switchable)
{
    PrivateData* me = (PrivateData*)malloc(sizeof(PrivateData));
    assert(me != NULL);

    me->switchable = switchable;

    return me;
}

void Switcher_updateSwitchable(Switcher_Handle me, Switchable* switchable)
{
    assert(me != NULL);
    me->switchable = switchable;
}

void Switcher_doSomeSwitching(Switcher_Handle me, unsigned int howManyTimes)
{
    unsigned int i;
    assert(me != NULL);

    Switchable_State currentState = me->switchable->getState(me->switchable->handle);
    Switchable_State nextState = currentState == SWITCHABLE_STATE_ON ? SWITCHABLE_STATE_OFF : SWITCHABLE_STATE_ON;

    for (i = 0; i < howManyTimes; ++i) {
        if (nextState == SWITCHABLE_STATE_ON) {
            me->switchable->turnOn(me->switchable->handle);
        } else {
            me->switchable->turnOff(me->switchable->handle);
        }

        nextState = nextState == SWITCHABLE_STATE_ON ? SWITCHABLE_STATE_OFF : SWITCHABLE_STATE_ON;
    }
}

