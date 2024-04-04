#!/bin/bash

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x


cd corpus_cleaner
wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.h
wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.cpp
cd ../

set +x