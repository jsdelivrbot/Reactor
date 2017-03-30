#!/bin/sh


#
#
#
insmod ~/Projects/enable_arm_pmu/ko/enable_arm_pmu.ko

#
#
#
cpufreq-set -c 0 -f 912MHz
cpufreq-set -c 1 -f 912MHz
cpufreq-set -c 2 -f 912MHz
cpufreq-set -c 3 -f 912MHz

#
#
#
rm -f /tmp/Control.log
rm -f /tmp/Outlet.log
rm -f /tmp/Inlet.log
rm -f /tmp/Server.log

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
