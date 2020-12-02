#pragma once

#ifdef DEBUG
#define debug(fmt...) do{\
    fprintf(stderr,"[%s:%d]:", __func__, __LINE__);\
    fprintf(stderr,fmt);\
}while(0)
#else
#define debug(fmt...)
#endif

