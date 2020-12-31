# Copyright (c) RToax, 2020年 12月 31日 星期四 13:26:37 CST.
# All rights reserved.

#!/bin/bash

for script in $(ls scripts/)
do
. scripts/$script
done
