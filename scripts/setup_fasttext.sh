#!/bin/bash

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

cd scripts
git clone https://github.com/facebookresearch/fastText.git
cd fastText
mkdir build 
cd build
cmake ..
make && make install
cd ../../../

mkdir -p corpus_cleaner/build/
cd corpus_cleaner/build/
wget https://dl.fbaipublicfiles.com/fasttext/supervised-models/lid.176.bin

mkdir -p ../../tests/build/
cp lid.176.bin ../../tests/build/

cd ../

set +x