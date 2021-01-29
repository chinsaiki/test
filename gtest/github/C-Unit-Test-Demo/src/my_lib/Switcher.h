/**
 * The Switcher likes to switch.
 */ 

#ifndef SWITCHER_H_
#define SWITCHER_H_

#include "Switchable.h"

typedef struct Switcher_PrivateData* Switcher_Handle;


Switcher_Handle Switcher_create(Switchable* switchable);

void Switcher_updateSwitchable(Switcher_Handle me, Switchable* switchable);

void Switcher_doSomeSwitching(Switcher_Handle me, unsigned int howManyTimes);

#endif // SWITCHER_H_