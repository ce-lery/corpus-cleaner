#!/bin/bash

# echo $(pwd)
# /home/corpus-cleaner-cpp/scripts
# refer:https://qiita.com/midheartty/items/590b0911c000a24abe34

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

cd scripts/

mkdir -p ../corpus_cleaner/kenlm
mkdir -p ../corpus_cleaner/build
git clone https://github.com/kpu/kenlm.git ../corpus_cleaner/kenlm
wget -c -P ../corpus_cleaner/build http://dl.fbaipublicfiles.com/cc_net/lm/ja.arpa.bin

mkdir -p ../tests/build
cp ../corpus_cleaner/build/ja.arpa.bin ../tests/build/

cd ../tests/
git clone https://github.com/kpu/kenlm.git

cd ../

set +x