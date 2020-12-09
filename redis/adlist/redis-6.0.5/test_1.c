#include <malloc.h>

#include "adlist.h"

int adlist_test_listCreate()
{
    list *l1 = listCreate();

    listRelease(l1);
}

int main(int argc, char **argv)
{
    adlist_test_listCreate();
    
}
