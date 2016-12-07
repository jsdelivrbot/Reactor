#!/bin/sh


scp Inlet/Output/Main.elf steve@$1:~/Inlet
scp Outlet/Output/Main.elf steve@$1:~/Outlet
scp Control/Output/Main.elf steve@$1:~/Control
scp Run.sh steve@$1:~/



