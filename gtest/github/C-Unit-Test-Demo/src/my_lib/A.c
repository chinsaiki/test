#include "A.h"
#include <assert.h>
#include <stdlib.h>

/**
 * Private data structure.
 * Contains all member variables.
 */ 
typedef struct __A_PrivateData
{
    int initialValue;
} PrivateData;

A_Handle A_create(int initialValue)
{
    PrivateData* me = (PrivateData*)malloc(sizeof(PrivateData));
    assert(me != NULL);

    me->initialValue = initialValue;

    return me;
}

int A_multiplyWithInitial(A_Handle me, int x)
{
    assert(me != NULL);

    return me->initialValue * x;
}

