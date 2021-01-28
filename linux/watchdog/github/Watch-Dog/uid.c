/******************************************************************************
 * File name  : uid.c
 * Developer  : HRD8
 * Date		  : 2019-02-18
 * Description: version 1
 ******************************************************************************/
#include <time.h>   /* time		*/
#include <unistd.h> /* getpid 	*/

#include "uid.h"

/**** Functions ***********************/
/****** UIDCreate *************************************************************/
unique_id_t UIDCreate(void)
{
	static size_t count = 0;
	unique_id_t uid = {0};

	uid.time = time(NULL);
	uid.pid = getpid();
	uid.counter = ++count;

	return (uid);
}

/****** UIDIsSame *************************************************************/
int UIDIsSame(unique_id_t uid1, unique_id_t uid2)
{
	return (uid1.time 	== uid2.time && 
			uid1.pid 	== uid2.pid 	   && 
			uid1.counter== uid2.counter);
}

/****** UIDIsBad **************************************************************/
int UIDIsBad(unique_id_t uid)
{
	return (uid.time 	== 0 && 
			uid.pid 	== 0 && 
			uid.counter == 0);
}

/****** UIDGenerateBad ********************************************************/
unique_id_t UIDGenerateBad(void)
{
	unique_id_t uid = {0};

	return (uid);
}