#!/bin/bash

# reference
# https://huggingface.co/datasets/oscar
# https://huggingface.co/datasets/oscar/blob/main/oscar.py

# setting log output
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

# download dataset
mkdir -p results/data/wiki/
mkdir -p results/dataset/original/


# merge dataset for creating tokenizer
# TODO:
# cp wiki_mrph.txt merge_dataset_for_tokenizer.txt
# TODO:
# cat ja.txt >> merge_dataset_for_tokenizer.txt


# activate venv
if [ ! -d ".env" ];then
    python -m venv .env
    soruce .env/bin/activate
    pip install -r python/requirements.txt
fi

GIT_LFS_SKIP_SMUDGE=1 git clone https://huggingface.co/datasets/graelo/wikipedia.git
cd wikipedia/data/20230901/ja/
git lfs pull --include "train-*-of-0016.parquet"
cd ../../../../
# GIT_LFS_SKIP_SMUDGE=1 git clone https://huggingface.co/datasets/izumi-lab/wikipedia-ja-20230720.git
# cd wikipedia-ja-20230720/data
# git lfs pull --include "train-*-of-00008-*.parquet"
# cd ../../
source .env/bin/activate
python python/util/parquet2txt.py

rm -r wikipedia

# train test split
echo "# train test split wiki.txt"
LINES=`wc -l ./results/dataset/original/wiki.txt | awk '{print $1}'`
echo $LINES

TRAIN_DATA_LINES=$(($LINES*90/100))
REMAIN_DATA_LINES=$(($LINES-$TRAIN_DATA_LINES))
echo $TRAIN_DATA_LINES
echo $REMAIN_DATA_LINES

head -n $TRAIN_DATA_LINES ./results/dataset/original/wiki.txt > ./results/dataset/original/wiki_train.txt
tail -n $REMAIN_DATA_LINES ./results/dataset/original/wiki.txt > ./results/dataset/original/wiki_remain.txt

TEST_DATA_LINES=$(($REMAIN_DATA_LINES/2))
VALID_DATA_LINES=$(($REMAIN_DATA_LINES-$TEST_DATA_LINES))
head -n $TEST_DATA_LINES ./results/dataset/original/wiki_remain.txt > ./results/dataset/original/wiki_test.txt
tail -n $VALID_DATA_LINES ./results/dataset/original/wiki_remain.txt > ./results/dataset/original/wiki_valid.txt

rm ./results/dataset/original/wiki_remain.txt 
rm ./results/dataset/original/wiki.txt 

# cleaning
bash scripts/build.sh
bash scripts/run.sh

# move results
# rm -r ./results/dataset/original/
mv ./results/dataset/* ./results/data/wiki/

