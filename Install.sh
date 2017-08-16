#!/bin/sh


scp enable_arm_pmu/ko/enable_arm_pmu.ko root@$1/tmp
scp Inlet/Output/Main.elf root@$1:/tmp/Inlet
scp Outlet/Output/Main.elf root@$1:/tmp/Outlet
scp Control/Output/Main.elf root@$1:/tmp/Control
scp Server/Output/Main.elf root@$1:/tmp/Server
scp Run.sh root@$1:/tmp
scp Kill.sh root@$1:/tmp



