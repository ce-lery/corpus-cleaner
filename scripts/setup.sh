#!/bin/bash

# echo $(pwd)
# /home/corpus-cleaner-cpp/

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

bash scripts/setup_fasttext.sh
bash scripts/setup_icu.sh
bash scripts/setup_kenlm.sh
bash scripts/setup_murmurhash.sh
bash scripts/setup_sentencepiece.sh
bash scripts/setup_sigmdjson.sh

touch scripts/setup_completed

set +x