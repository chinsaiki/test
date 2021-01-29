#include "Microwave.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct Microwave_PrivateData
{
    Switchable switchable;
    Switchable_State state;
} PrivateData;

/**
 * The implementation of the Switchable interface methods.
 * @{
 */ 
static void turnOn(Switchable_Handle handle);
static void turnOff(Switchable_Handle handle);
static Switchable_State getState(Switchable_Handle handle);
/** @} */

Microwave_Handle Microwave_create()
{
    PrivateData* me = (PrivateData*)malloc(sizeof(PrivateData));
    assert(me != NULL);

    // after initialization the microwave is off
    me->state = SWITCHABLE_STATE_OFF; 

    // initialize the interface with the concrete method implementations
    me->switchable.turnOn = &turnOn;
    me->switchable.turnOff = &turnOff;
    me->switchable.getState = &getState;

    // init the interface's handle with the 'me' pointer
    me->switchable.handle = (Switchable_Handle)me;

    return me;
}

/**
 * Returns a pointer to the implemented multipliable interface.
 */ 
Switchable* Microwave_getSwitchableInterface(Microwave_Handle me)
{
    assert(me != NULL);
    return &me->switchable;
}

static void turnOn(Switchable_Handle handle)
{
    PrivateData* me = (PrivateData*)handle;
    assert(me != NULL);
    me->state = SWITCHABLE_STATE_ON;
    printf("Microwave turned On.\n");
}

static void turnOff(Switchable_Handle handle)
{
    PrivateData* me = (PrivateData*)handle;
    assert(me != NULL);
    me->state = SWITCHABLE_STATE_OFF; 
    printf("Microwave turned Off.\n");
}

static Switchable_State getState(Switchable_Handle handle)
{
    PrivateData* me = (PrivateData*)handle;
    assert(me != NULL);

    return me->state;
}
