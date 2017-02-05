#!/bin/sh


insmod ~/Projects/enable_arm_pmu/ko/enable_arm_pmu.ko

#
# Control *must* be first. Order is important.
#
taskset -c 0 ./Control > /tmp/Control.log &
sleep 1
taskset -c 1 ./Outlet > /tmp/Outlet.log &
sleep 1
taskset -c 2 ./Inlet > /tmp/Inlet.log &
sleep 1
taskset -c 3 ./Server > /tmp/Server.log &
