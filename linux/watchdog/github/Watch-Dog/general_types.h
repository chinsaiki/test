#ifndef     _GENERAL_TYPES_
#define     _GENERAL_TYPES_

/***** Enums ******************************************************************/
enum status
{
    FAILURE = -1,
    SUCCESS = 0
};

enum boolean
{
    FALSE,
    TRUE
};

/***** Type Definitions *******************************************************/
typedef enum status status_t; 
typedef enum boolean boolean_t; 

#endif     /* _GENERAL_TYPES_ */