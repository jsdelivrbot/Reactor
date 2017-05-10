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

