#!/bin/bash

tuna --threads="httpd" --show_threads

#                    线程       ctxt_switches
#   pid SCHED_ rtpri affinity voluntary nonvoluntary             cmd 
#  2398   OTHER     0  0,1,2,3    935791           48           httpd  
#  2401   OTHER     0  0,1,2,3    311825            8           httpd  
#  2402   OTHER     0  0,1,2,3         1            2           httpd  
#  2403   OTHER     0  0,1,2,3         1           12           httpd  
#  2404   OTHER     0  0,1,2,3         1            6           httpd  
#  2405   OTHER     0  0,1,2,3         1            3           httpd  
#  2406   OTHER     0  0,1,2,3     93555           11           httpd  
#  2407   OTHER     0  0,1,2,3         1           16           httpd  
#  2409   OTHER     0  0,1,2,3         1           12           httpd  
#  2410   OTHER     0  0,1,2,3         1            8           httpd  
#  2411   OTHER     0  0,1,2,3         1           39           httpd  
tuna --threads=71544 --show_threads
