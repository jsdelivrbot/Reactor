#!/bin/sh


insmod ~/Projects/enable_arm_pmu/ko/enable_arm_pmu.ko
taskset -c 1 ./Inlet &
taskset -c 2 ./Outlet &
taskset -c 3 ./Control &
