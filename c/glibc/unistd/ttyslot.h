//find the slot of the current user's terminal in some file

#include <unistd.h>    /* on BSD-like systems, and Linux */
#include <stdlib.h>    /* on System V-like systems */

int ttyslot(void);

