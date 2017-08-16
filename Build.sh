#!/bin/bash

if [ -z "$SOURCELIBRARY_BASE" ]; then SOURCELIBRARY_BASE=../../../ProjectLayout/SourceLibrary; fi

pushd Outlet/Output
make SOURCE_LIBRARY=$SOURCELIBRARY_BASE PROJECT_BASE=../../Outlet clean all
popd


pushd Inlet/Output
make SOURCE_LIBRARY=$SOURCELIBRARY_BASE PROJECT_BASE=../../Inlet clean all
popd


pushd Control/Output
make SOURCE_LIBRARY=$SOURCELIBRARY_BASE PROJECT_BASE=../../Control clean all
popd


pushd Server/Output
make SOURCE_LIBRARY=$SOURCELIBRARY_BASE PROJECT_BASE=../../Server clean all
popd

pushd CursesUI/Output
make SOURCE_LIBRARY=$SOURCELIBRARY_BASE PROJECT_BASE=../../CursesUI clean all
popd

pushd enable_arm_pmu/ko
make CROSS_COMPILE=$PWD/../../Platform/BuildRoot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-  ARCH=arm  KDIR=$PWD/../../Platform/BuildRoot/output/build/linux-4.9 clean all
popd

