#ifndef A_H_
#define A_H_

/**
 * Forward declaration of 'this' pointer type.
 */
typedef struct __A_PrivateData* A_Handle;

/**
 * Constructor.
 * 
 * @param initialValue The initial value to multiply with
 */
A_Handle A_create(int initialValue);

/**
 * Example function that multiplies with the initial value. 
 */
int A_multiplyWithInitial(A_Handle me, int x);

#endif