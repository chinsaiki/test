#!/bin/bash
# To view the current interrupt requests (IRQs) and their affinity,
tuna --irqs=1,6 --show_irqs

#    # users            affinity
#   1 i8042             0,1,2,3
#   6 floppy            0,1,2,3
