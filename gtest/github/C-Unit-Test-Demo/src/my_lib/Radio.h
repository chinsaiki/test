#ifndef RADIO_H_
#define RADIO_H_

#include "Switchable.h"

typedef struct Radio_PrivateData* Radio_Handle;


Radio_Handle Radio_create();

/**
 * Returns a pointer to the implemented Switchable interface.
 */ 
Switchable* Radio_getSwitchableInterface(Radio_Handle me);


#endif // RADIO_H_