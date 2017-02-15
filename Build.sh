#!/bin/bash

pushd Outlet/Output
make SOURCE_LIBRARY=~/BlockWorks/ProjectLayout/SourceLibrary PROJECT_BASE=../../Outlet clean all
popd


pushd Inlet/Output
make SOURCE_LIBRARY=~/BlockWorks/ProjectLayout/SourceLibrary PROJECT_BASE=../../Inlet clean all
popd


pushd Control/Output
make SOURCE_LIBRARY=~/BlockWorks/ProjectLayout/SourceLibrary PROJECT_BASE=../../Control clean all
popd


pushd Server/Output
make SOURCE_LIBRARY=~/BlockWorks/ProjectLayout/SourceLibrary PROJECT_BASE=../../Server clean all
popd
