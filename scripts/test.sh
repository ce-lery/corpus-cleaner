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
cmake ..
make all
./test_corpus_cleaner-googletest