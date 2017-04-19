#!/bin/sh

#
# Move all IRQ servicing over to CPU0 (iff possible).
#
echo "0" > /proc/irq/32/smp_affinity_list
echo "0" > /proc/irq/38/smp_affinity_list
echo "0" > /proc/irq/39/smp_affinity_list
echo "0" > /proc/irq/49/smp_affinity_list
echo "0" > /proc/irq/92/smp_affinity_list
echo "0" > /proc/irq/93/smp_affinity_list
echo "0" > /proc/irq/106/smp_affinity_list
echo "0" > /proc/irq/108/smp_affinity_list
echo "0" > /proc/irq/114/smp_affinity_list
echo "0" > /proc/irq/119/smp_affinity_list


#
# Allow access to the cycle counter.
#
insmod ~/Projects/enable_arm_pmu/ko/enable_arm_pmu.ko

#
# Set a lower-than-normal cpu freq to stop clock throttling.
#
cpufreq-set -c 0 -f 912MHz
cpufreq-set -c 1 -f 912MHz
cpufreq-set -c 2 -f 912MHz
cpufreq-set -c 3 -f 912MHz

#
# Remove output logs.
#
rm -f /tmp/Control.log
rm -f /tmp/Outlet.log
rm -f /tmp/Inlet.log
rm -f /tmp/Server.log

#
# Control *must* be first. Order is important.
#
taskset -c 3 ./Control > /tmp/Control.log &
sleep 1
taskset -c 1 ./Outlet > /tmp/Outlet.log &
sleep 1
taskset -c 2 ./Inlet > /tmp/Inlet.log &
sleep 1
taskset -c 0 ./Server > /tmp/Server.log &
