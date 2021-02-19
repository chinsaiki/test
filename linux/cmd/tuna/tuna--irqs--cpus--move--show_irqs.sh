#!/bin/bash

tuna --irqs=1 --cpus=1,2 --move
tuna --irqs=1 --show_irqs

tuna --irqs=1 --cpus=0,1,2,3 --move
tuna --irqs=1 --show_irqs
