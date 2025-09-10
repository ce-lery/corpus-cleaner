#!/bin/bash

# reference
# https://huggingface.co/datasets/oscar
# https://huggingface.co/datasets/oscar/blob/main/oscar.py

# setting log output
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

# # download dataset
# mkdir -p results/data/oscar/
mkdir -p results/dataset/original/

# for i in $(seq 1 120)
# do
#     # refer:https://s3.amazonaws.com/datasets.huggingface.co/oscar/1.0/unshuffled/original/ja/ja_sha256.txt
#     wget "https://s3.amazonaws.com/datasets.huggingface.co/oscar/1.0/unshuffled/original/ja/ja_part_${i}.txt.gz" -P "./results/original/oscar/"
#     gunzip "./results/original/oscar/ja_part_${i}.txt.gz"
#     # rm "./results/original/oscar/ja_part_${i}.txt.gz"
# done

# # merge files of dataset
# for i in $(seq 1 120)
# do
#     cat "./results/original/oscar/ja_part_${i}.txt" >> "./results/original/oscar2109_ja.txt" 
#     rm  "./results/original/oscar/ja_part_${i}.txt"
# done

# rm -r "./results/original/oscar"

# train test split
echo "# train test split oscar2109_ja.txt"
LINES=`wc -l ./results/original/oscar2109_ja.txt | awk '{print $1}'`
echo $LINES

TRAIN_DATA_LINES=$(($LINES*90/100))
REMAIN_DATA_LINES=$(($LINES-$TRAIN_DATA_LINES))
# TEST_DATA_LINES=$((($LINES-$TRAIN_DATA_LINES)/2))
# VALID_DATA_LINES=$(($LINES-$TRAIN_DATA_LINES) - $TEST_DATA_LINES)
echo $TRAIN_DATA_LINES
echo $REMAIN_DATA_LINES
# echo $TEST_DATA_LINES

head -n $TRAIN_DATA_LINES ./results/original/oscar2109_ja.txt > ./results/original/oscar2109_ja_train.txt
tail -n $REMAIN_DATA_LINES ./results/original/oscar2109_ja.txt > ./results/original/oscar2109_ja_remain.txt

TEST_DATA_LINES=$(($REMAIN_DATA_LINES/2))
VALID_DATA_LINES=$(($REMAIN_DATA_LINES-$TEST_DATA_LINES))
head -n $TEST_DATA_LINES ./results/original/oscar2109_ja_remain.txt > ./results/original/oscar2109_ja_test.txt
tail -n $VALID_DATA_LINES ./results/original/oscar2109_ja_remain.txt > ./results/original/oscar2109_ja_valid.txt

rm ./results/original/oscar2109_ja_remain.txt 
rm ./results/original/oscar2109_ja.txt 

# cleaning
bash scripts/build.sh
bash scripts/run.sh

# move results
rm -r ./results/dataset/original/
mv ./results/dataset/* ./results/data/oscar/


