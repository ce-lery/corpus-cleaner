#!/bin/bash

# setting log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

cd scripts
wget https://github.com/doxygen/doxygen/releases/download/Release_1_10_0/doxygen-1.10.0.src.tar.gz
gunzip doxygen-1.10.0.src.tar.gz
tar xf doxygen-1.10.0.src.tar
cd doxygen-1.10.0
mkdir build
cd build
cmake ..
make
make install
cd ../../

# remove intermediate file
rm -r doxygen-1.10.0.src.tar.gz
rm -r doxygen-1.10.0.src.tar

cd ../