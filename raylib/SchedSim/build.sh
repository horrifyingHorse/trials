#!/bin/bash

cd ./build/
cmake ..
make
cp ../procs.proc .
./SchedSim
