/******************************************************************************
 * File name  : uid.h
 * Developer  : HRD8
 * Date		  : 2019-02-18
 * Description: version 1
 ******************************************************************************/

#ifndef _UID_H_
#define _UID_H_

#include <sys/types.h> /* pid_t, time_t, size_t */ 

/**** Type defenitions ****************/
typedef struct u_id_t unique_id_t;

/**** Structs *************************/
struct u_id_t
{
	time_t time;
    pid_t pid;
    size_t counter;
};

/**** Functions ***********************/
/****** UIDCreate **************************************************************
 *	Description:Creating new unique id.
 *	Input:		
 *  Output:		unique_id_t 
 *	Compexity:	O(1)
 */
unique_id_t UIDCreate(void);

/****** UIDIsSame **************************************************************
 *	Description:Comparing two ids.
 *	Input:		
 *  Output:		Boolean: 1 if same, 0 otherwise.
 *	Compexity:	O(1)
 */
int UIDIsSame(unique_id_t uid1, unique_id_t uid2);

/****** UIDIsBad ***************************************************************
 *	Description:Check bad id
 *	Input:		
 *  Output:		Boolean: 1 if bad, 0 otherwise.
 *	Compexity:	O(1)
 */
int UIDIsBad(unique_id_t uid);

/****** UIDGenerateBad *********************************************************
 *	Description:Generate bad id
 *	Input:		
 *  Output:		unique_id_t - Bad 
 *	Compexity:	O(1)
 */
unique_id_t UIDGenerateBad(void);

#endif /*_UID_H_*/