#!/bin/bash

cd $BUILD_TOP/nuttx/nuttx
make distclean
cd $BUILD_TOP/nuttx/nuttx/tools

./configure.sh hdk/muc/dtv
#./configure.sh hdk/muc/base_powered

cd $BUILD_TOP/nuttx/nuttx
make

