#!/bin/bash

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

# make & activate venv
if [ ! -d "tests/googletest" ];then
    bash scripts/setup_googletest.sh
fi

cd tests
mkdir -p build & cd build

if [ ! -f "lid.176.bin" ];then
    wget https://dl.fbaipublicfiles.com/fasttext/supervised-models/lid.176.bin
fi

cmake ..
make all

ICUPATH=$PWD/../../scripts/icu/usr/local
echo $ICUPATH
export C_INCLUDE_PATH=$ICUPATH/include
export CPLUS_INCLUDE_PATH=$ICUPATH/include
export LIBRARY_PATH=$ICUPATH/lib
export LD_LIBRARY_PATH=$ICUPATH/lib

./test_corpus_cleaner-googletest

# valgrind --leak-check=full --show-leak-kinds=all ./test_corpus_cleaner-googletest