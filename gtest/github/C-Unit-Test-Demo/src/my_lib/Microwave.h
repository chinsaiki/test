#ifndef MICROWAVE_H_
#define MICROWAVE_H_

#include "Switchable.h"

typedef struct Microwave_PrivateData* Microwave_Handle;


Microwave_Handle Microwave_create();

/**
 * Returns a pointer to the implemented Switchable interface.
 */ 
Switchable* Microwave_getSwitchableInterface(Microwave_Handle me);


#endif // MICROWAVE_H_