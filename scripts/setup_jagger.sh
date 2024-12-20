#!/bin/bash

# echo $(pwd)
# /home/corpus-cleaner-cpp
# refer:https://qiita.com/midheartty/items/590b0911c000a24abe34

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

cd scripts

git clone https://github.com/ce-lery/jagger-extension.git -b v0.1.1
cd jagger-extension
bash scripts/build.sh

set +x
