#ifndef SWITCHABLE_H_
#define SWITCHABLE_H_

// Demonstrates a simple interface in C

/**
 * Declares a pointer that will point to the concrete implementation
 * of the interface.
 */ 
typedef void* Switchable_Handle;

typedef enum __Switchable_State
{
    SWITCHABLE_STATE_ON,
    SWITCHABLE_STATE_OFF
} Switchable_State;

// Declares the methods of the interface
typedef void (*Switchable_turnOn)(Switchable_Handle);
typedef void (*Switchable_turnOff)(Switchable_Handle);
typedef Switchable_State (*Switchable_getState)(Switchable_Handle);

typedef struct __Switchable
{
    Switchable_Handle handle; // the explicit 'this' pointer
    Switchable_turnOn turnOn; // pointer to function implementation
    Switchable_turnOff turnOff; // pointer to function implementation
    Switchable_getState getState; // pointer to function implementation
} Switchable;

#endif