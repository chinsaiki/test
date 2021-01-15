#!/bin/bash

sudo ethtool -c eth0

# Coalesce parameters for eth0:
#Adaptive RX: off  TX: off
#stats-block-usecs: 0
#sample-interval: 0
#pkt-rate-low: 0
#pkt-rate-high: 0

