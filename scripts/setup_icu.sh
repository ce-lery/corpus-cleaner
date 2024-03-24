#!/bin/bash

# echo $(pwd)
# /home/corpus-cleaner-cpp/scripts
# refer:https://qiita.com/midheartty/items/590b0911c000a24abe34

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

wget https://github.com/unicode-org/icu/releases/download/release-74-2/icu4c-74_2-Ubuntu22.04-x64.tgz
gunzip -d < icu4c-74_2-Ubuntu22.04-x64.tgz | tar xvf -

ICUPATH=$PWD/icu/usr/local
echo $ICUPATH
export C_INCLUDE_PATH=$ICUPATH/include
export CPLUS_INCLUDE_PATH=$ICUPATH/include
export LIBRARY_PATH=$ICUPATH/lib
export LD_LIBRARY_PATH=$ICUPATH/lib
# clang++ -licuio -licui18n -licutu -licuuc -licudata -std=c++1z ../corpus_cleaner/normalizer.cpp
g++ ../corpus_cleaner/normalizer.cpp -licuio -licui18n -licutu -licuuc -licudata # どちらでも可
# g++ normalizer.cpp -licuio -licui18n -licutu -licuuc -licudata
./a.out
rm -r icu4c-74_2-Ubuntu22.04-x64.tgz

cd ../

set +x