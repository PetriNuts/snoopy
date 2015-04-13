#!/bin/sh

CWD=$(pwd)
cd ..
SN=$(pwd)

LD_LIBRARY_PATH=$SN/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

$SN/bin/snoopy
