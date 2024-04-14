#!/bin/bash

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

mkdir -p results/dataset
cd results/dataset

###
### japanese wikipedia 
###

GIT_LFS_SKIP_SMUDGE=1 git clone https://huggingface.co/datasets/graelo/wikipedia.git
cd wikipedia/data/20230601/ja/
git lfs pull --include "train-*-of-0015.parquet"
cd ../../../../

source ../../.env/bin/activate
python ../../python/util/parquet2txt.py

### train test split ###
echo "# train test split wiki_mrph.txt"
LINES=`wc -l wiki.txt | awk '{print $1}'`
echo $LINES

TRAIN_DATA_LINES=$(($LINES*95/100))
TEST_DATA_LINES=$(($LINES-$TRAIN_DATA_LINES))
echo $TRAIN_DATA_LINES
echo $TEST_DATA_LINES

head -n $TRAIN_DATA_LINES wiki.txt > wiki_train.txt
tail -n $TEST_DATA_LINES wiki.txt > wiki_test.txt

rm -r wiki.txt
mkdir -p input
mv wiki_train.txt input/
mv wiki_test.txt input/

###
### japanese CC-100 
###

# 0.069TB
echo "# download japanese CC-100"
wget http://data.statmt.org/cc-100/ja.txt.xz
unxz -v ja.txt.xz
# remove white space line 
sed -i '/^$/d' ja.txt
head -n15 ja.txt

# cleaning
# https://www.gnu.org/software/parallel/parallel_tutorial.html

# train test split (train:test = 95:5)
echo "# train test split cc100_mrph.txt"
LINES=`wc -l ja.txt | awk '{print $1}'`
echo $LINES

TRAIN_DATA_LINES=$(($LINES*95/100))
TEST_DATA_LINES=$(($LINES-$TRAIN_DATA_LINES))
echo $TRAIN_DATA_LINES
echo $TEST_DATA_LINES

head -n $TRAIN_DATA_LINES ja.txt > cc100_train.txt
tail -n $TEST_DATA_LINES ja.txt > cc100_test.txt

rm -r ja.txt
mkdir -p input
mv cc100_train.txt input/
mv cc100_test.txt input/

###
### OSCAR
###

# # merge
# cat wiki_train.txt >> train.txt
# cat wiki_test.txt >> test.txt
# echo "train dataset size:"
# wc -l train.txt 
# echo "test dataset size:"
# wc -l test.txt 

# # delete intermediate
# rm -r wiki.txt
# rm -r wiki_preprocess.txt
# # rm -r ja.txt.xz
# rm -r wiki_train.txt
# rm -r wiki_test.txt

cd ../../

set +x