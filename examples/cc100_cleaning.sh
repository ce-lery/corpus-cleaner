#!/bin/bash

# reference
# https://huggingface.co/datasets/oscar
# https://huggingface.co/datasets/oscar/blob/main/oscar.py

# # setting log output
# mkdir -p results/log/$(basename "$0" .sh)
# log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
# exec &> >(tee -a $log)
# set -x

# # download dataset
# mkdir -p results/data/cc100/
mkdir -p results/dataset/original/

# # 0.069TB
# echo "# download japanese CC-100"
# wget http://data.statmt.org/cc-100/ja.txt.xz -P "./results/original/"
# unxz -v ./results/original/ja.txt.xz
head -n15 ./results/original/ja.txt
mkdir -p ./results/dataset/original
mv ./results/original/ja.txt ./results/dataset/original/cc100.txt

# cleaning

# merge dataset for creating tokenizer
# TODO:
# cp wiki_mrph.txt merge_dataset_for_tokenizer.txt
# TODO:
# cat ja.txt >> merge_dataset_for_tokenizer.txt

# # train test split
# echo "# train test split cc100.txt"
# LINES=`wc -l ./results/dataset/original/cc100.txt | awk '{print $1}'`
# echo $LINES

# TRAIN_DATA_LINES=$(($LINES*90/100))
# REMAIN_DATA_LINES=$(($LINES-$TRAIN_DATA_LINES))
# echo $TRAIN_DATA_LINES
# echo $REMAIN_DATA_LINES

# head -n $TRAIN_DATA_LINES ./results/dataset/original/cc100.txt > ./results/dataset/original/cc100_train.txt
# tail -n $REMAIN_DATA_LINES ./results/dataset/original/cc100.txt > ./results/dataset/original/cc100_remain.txt

# TEST_DATA_LINES=$(($REMAIN_DATA_LINES/2))
# VALID_DATA_LINES=$(($REMAIN_DATA_LINES-$TEST_DATA_LINES))
# head -n $TEST_DATA_LINES ./results/dataset/original/cc100_remain.txt > ./results/dataset/original/cc100_test.txt
# tail -n $VALID_DATA_LINES ./results/dataset/original/cc100_remain.txt > ./results/dataset/original/cc100_valid.txt

# rm ./results/dataset/original/cc100_remain.txt 
# rm ./results/dataset/original/cc100.txt 

# cleaning
bash scripts/build.sh
bash scripts/run.sh

# move results
rm -r ./results/dataset/original/
mv ./results/dataset/* ./results/data/cc100/


