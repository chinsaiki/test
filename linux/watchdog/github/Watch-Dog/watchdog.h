/******************************************************************************
 * File name:   watchdog.h
 * Developer:   Esty
 * Date:        2019-05-06
 * Reviewer:
 ******************************************************************************/
#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <stddef.h>      /* size_t */


/***************************** WDKeepMeAlive **********************************/
/*
 * description  :  Initiates application protection.
 *                 Reruns application incase of unexpected termination.
 *                 Protection is set until WDLetMeDie is called.
 *                 Max wait time on termination is 5 seconds.
 *
 * input		:  main argument argc   - number of program arguments
 *                 main argument argv[] - array of program arguments
 *
 * return value :  on success - SUCCESS(0).
 * 				   on failure - FAILURE(-1).
 *
 *
 */
int WDKeepMeAlive(int argc, char **argv);

/******************************* WDLetMeDie ***********************************/
/*
 * description  :  Terminates WDKeepMeAlive application protection.
 *
 */
void WDLetMeDie(void);

#endif     /* __WATCHDOG_H__ */
