#!/bin/bash
# 显示 local timer rate
perf stat -a -A -e irq_vectors:local_timer_entry sleep 120
# 结果如下
#Performance counter stats for 'system wide':
#
# CPU0                   730      irq_vectors:local_timer_entry                                   
# CPU1                   238      irq_vectors:local_timer_entry                                   
# CPU2                    13      irq_vectors:local_timer_entry                                   
# CPU3                    10      irq_vectors:local_timer_entry                                   
#
#        8.801915407 seconds time elapsed
#		
