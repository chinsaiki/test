#include "SwitchableMock.h"
#include <assert.h>
#include <stdlib.h>


static void turnOn(Switchable_Handle handle);
static void turnOff(Switchable_Handle handle);
static Switchable_State getState(Switchable_Handle handle);

void SwitchableMock_initialize(SwitchableMock* mock)
{
    mock->switchable.turnOn = &turnOn;
    mock->switchable.turnOff = &turnOff;
    mock->switchable.getState = &getState;
    mock->switchable.handle = (Switchable_Handle)mock;
    mock->turnOnCallCounter = 0;
    mock->turnOffCallCounter = 0;
    mock->getStateCallCounter = 0;
    mock->switchableState = SWITCHABLE_STATE_OFF;
}

static void turnOn(Switchable_Handle handle)
{
    SwitchableMock* mock = (SwitchableMock*)handle;
    assert(mock != NULL);
    mock->turnOnCallCounter++;
}

static void turnOff(Switchable_Handle handle)
{
    SwitchableMock* mock = (SwitchableMock*)handle;
    assert(mock != NULL);
    mock->turnOffCallCounter++;
}

static Switchable_State getState(Switchable_Handle handle)
{
    SwitchableMock* mock = (SwitchableMock*)handle;
    assert(mock != NULL);
    mock->getStateCallCounter++;

    return mock->switchableState;
}

