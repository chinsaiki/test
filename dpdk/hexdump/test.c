/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include "rte_hexdump.h"

int main()
{
    char str[] = {"A123123   B123CD.[ ;]{:][; sadf \[a;sdf ;\';;}{:| defg"};

    rte_hexdump(stdout, "Title rte_hexdump", str, sizeof(str)/sizeof(str[0]));

    rte_memdump(stdout, "Title rte_memdump", str, sizeof(str)/sizeof(str[0]));
}
