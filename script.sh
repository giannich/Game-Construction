#!/bin/bash
export LD_LIBRARY_PATH="/usr/local/lib64:/usr/local/lib:$LD_LIBRARY_PATH"
export OPENTHREADS_INC_DIR="/usr/local/include"
export OPENTHREADS_LIB_DIR="/usr/local/lib64:/usr/local/lib"
export PATH="lib/Box2D:lib:$OPENTHREADS_LIB_DIR:$PATH"
#g++ osg2cpp.cpp -c
make