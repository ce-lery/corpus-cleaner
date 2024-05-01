# Specification

## Contents

<!-- ## Policy
This repository's policy is **"Simple & Speedy"**.-->

- [Contents](#contents)
- [Folder Structure](#folder-structure)
- [File Format](#file-format)
- [Corpus Cleaner Feature](#corpus-cleaner-feature)
- [MultiProcess Cleaning](#multiprocess-cleaning)
- [Test](#test)
- [Documentation](#documentation)


## Folder Structure

This tool assumes the following folder structure.

### Input Structure

Please place the dataset files to "/results/dataset/original/". 

Example:

```bash
corpus-cleaner
|-- results
|   `-- dataset
|         `-- original
|             |-- sample_dataset_train.txt   # This file is to be cleaned.
|             `-- sample_dataset_test.txt    # This file is to be cleaned.
```

### Intermediate Structure

The contents of each number folder are as follows.  
After running this tool, the files in the "results/dataset/original" folder will be divided into multiple parts (in the example below, 8 parts from 0 to 7) and each will be copied to the "dataset/i/input" folder.  

```bash
corpus-cleaner
|-- results
|   |-- dataset
|         |-- 0
|         |   |-- input
|         |   `-- output
|         |-- 1
|         |   |-- input
|         |   `-- output
|       ~ ommit ~ 
|         |-- 7
|         |   |-- input
|         |   `-- output
|         |-- cleaned
|         `-- original
```

The breakdown of the numbered folders is as follows.  
First, the contents of the input folder are copied to the intermediate folder.  


```bash
corpus-cleaner
|-- results
|   |-- dataset
|         |-- 0
|             |-- input
|             |   |-- sample_dataset_train.txt
|         　　|   `-- sample_dataset_test.txt   
|             `-- output
|                 |-- cleaned
|                 |   `-- sample_dataset_train.jsonl
|                 |-- exception
|                 |   `-- exception.txt
|                 |-- intermediate
|                 |   |-- sample_dataset_train.txt
|                 |   `-- sample_dataset_test.txt   
|                 `-- rejected
|                     `-- sample_dataset_train.jsonl
```

### Output Structure


```bash
corpus-cleaner
|-- results
|   |-- dataset
|         |-- 0
|         |   |-- input
|         |   `-- output
|         |-- 1
|         |   |-- input
|         |   `-- output
|       ~ ommit ~ 
|         |-- 7
|         |   |-- input
|         |   `-- output
|         |-- cleaned
|         `-- original
```


Note that if the cleaned file already exists, processing after the constructor of the CorpusCleaner class will not proceed to prevent it from being overwritten.  
The following warning appears on the console, so move or delete the relevant file to another directory, and then try the process again.  

```bash
ERROR: output_path or rejected_path folder already exists. Please RENAME to delete the selection.
```

## File Format

### Input File

This tool takes a .txt file as input. Each .txt file is expected to contain one data series per line.  
The each line format is following.

```txt
東洋史(とうようし)は、東洋を広く扱った歴史であり東洋学の歴史分野のことであり「東洋史学」(とうようしがく)とも称される。ヨーロッパ語の「東洋史」(たとえば英語の「Oriental History」)の訳語であり、現在の日本語の慣例ではおおむねマグリブから日本にかけての北アフリカ、ユーラシア大陸(ただしヨーロッパ地域を除く)および周辺諸島の歴史を扱う。
趣味はバイク。MARVELも大好きでキャプテンアメリカと同じハーレーダビッドソンを所有している。
```


### Output File
This tool is output cleaned file that is jsonl format.  
The each line format is following.  

```json
{
  "text":" <cleaned data>",
  "id":"<original file name>_<the line number of original file>",
  "is_rejected":"<0 or 1 (0: data that is to be remained, 1: data that is to be removed)>",  
  "metadata":"<the function name list that processes tesxt>",
  "language":"<language classification by fasttext. __label__ja is Japanese, __label__en is English, etc.>",
  "language_score":"<language classification score by fasttext.>",
  "perplexity":"<perplexity value by kenlm model.>"  
}
```

Example:

```json
{"text":"東洋史(とうようし)は、東洋を広く扱った歴史であり東洋学の歴史分野のことであり「東洋史学」(とうようしがく)とも称される。ヨーロッパ語の「東洋史」(たとえば英語の「Oriental History」)の訳語であり、現在の日本語の慣例ではおおむねマグリブから日本にかけての北アフリカ、ユーラシア大陸(ただしヨーロッパ地域を除く)および周辺諸島の歴史を扱う。","id":"wiki_test_27","is_rejected":"0","metadata":"Normalizer,","language":"__label__ja","language_score":"0.999781","perplexity":"6298.67"}
{"text":"趣味はバイク。MARVELも大好きでキャプテンアメリカと同じハーレーダビッドソンを所有している。","id":"wiki_test_2934","is_rejected":"1","metadata":"Normalizer,PerplexityFilter,","language":"__label__ja","language_score":"1.00005","perplexity":"179648"}
```

## Corpus Cleaner Feature

In this chapter, I explain corpus cleaner feature that is CorpusCleaner class.

### Flow 

TODO: write about   
TODO: write about using memory resource  

### List of Filtering Feature

For usage examples of each function, please refer to the API specifications.  

|Features|Details|Remarks|
|:--|:--|:--|
|Normalizer|Perform the normalization process described in the [link](https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja) below. <br> Neologdn's normalization contents include, for example, "replace half-width katakana with full-width" and "replace full-width spaces with half-width spaces."|See [here](https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja) for details on normalization contents.|
|URLRemover|Remove URLs matching regular expression that is "(https?\|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".|  |
|SpecialCharactersRemover|Remove special character.<br> For example, ☀, ♡, ☆, and so on.<br>Removes special characters within a specific Unicode range.|Please refer [this URL](https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html).<br>Special characters to be removed include some emojis.|
|EmojiRemover|Remove emoji characters that is \U0001F300(🌀) to \U0001F9FF(🧿).|  |
|QuotesRemover|Remove quotes. For example, [1], {245}, and so on.|  |
|LengthFilter|Remove too long sentence and too short sentence.|  |
|LanguageFilter|Classify sentence composition language and quality using fastText.<br> This function is applied to japanese and english.|This function uses fastText. About fastText, please refer [here](https://fasttext.cc/docs/en/crawl-vectors.html).|
|MinhashDeduplicater|Deduplicate sentence using minhash.<br>SentencePiece is used when tokenizing sentences.|  |
|ZeroPunctuationFilter|Remove sentence without punctuation that is "、","､","。","｡","．",".","？","?","！","!".|  |
|PerplexityFilter|KenLM's Perplexity Quality filtering.| |
|SentenceSegmenter|Execute Rule-based sentence separation (sentence segmentation) using [ja_sentence_segmenter](https://github.com/wwwcojp/ja_sentence_segmenter).|Please refer this [article](https://qiita.com/heimaru1231/items/b6ed09d4787e4e28175a).|


<!-- |[Remove kaomoji](corpus_cleaner/remover.py#L39)|Remove emoticons that exactly match the list in kaomoji.txt.<br>Approximately 1,400 types of emoticons will be removed.|Before using this feature, we recommend normalizing the text using TxtNormalizer().|
|[ftfy Fixer](corpus_cleaner/fixer.py#L6)|Fix broken Unicode using [ftfy](https://ftfy.readthedocs.io/en/latest/).|Inspired by [NVIDIA NeMo Data Curator](https://docs.nvidia.com/nemo-framework/user-guide/latest/modelguide/pretrainingdatasets/index.html).|
|[Txt Reader](corpus_cleaner/txt_reader.py)|BaseDiskReader that reads .txt file. |[Datatrove Readers](https://github.com/huggingface/datatrove/tree/main/src/datatrove/pipeline/readers) can read other formats (.csv,.json, ...etc.). Please read [here](https://github.com/huggingface/datatrove?tab=readme-ov-file).|
|[Txt Writer](corpus_cleaner/txt_writer.py)|DiskWriter that writes .txt file.|[Datatrove](https://github.com/huggingface/datatrove/tree/main/src/datatrove/pipeline/writers) can write other formats (.csv,.json, ...etc.). Please read [here](https://github.com/huggingface/datatrove?tab=readme-ov-file).|-->


## MultiProcess Cleaning

In main.cc, we create 8 (fixed value) processes and each process processes a different file to speed it up.

## Test

In this repository, I do test using [GoogleTest](https://github.com/google/googletest).
Test folder is tests/, and test file is [CorpusCleaner_test.cpp](./tests/CorpusCleaner_test.cpp).

If you want to do test, Please execute following command.

```bash
# echo $PWD 
# /path/to/corpus-cleanaer/
bash scripts/test.sh
```

When you do the command, the results of test is output.

```bash
+ ./test_corpus_cleaner-googletest
Running main() from /home/corpus-cleaner/tests/build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 24 tests from 1 test suite.
~ omission ~
[ RUN      ] CorpusCleanerTest.ExceptionReadDocumentFromJsonlOneLine
[       OK ] CorpusCleanerTest.ExceptionReadDocumentFromJsonlOneLine (209 ms)
[----------] 24 tests from CorpusCleanerTest (3140 ms total)

[----------] Global test environment tear-down
[==========] 24 tests from 1 test suite ran. (3140 ms total)
[  PASSED  ] 24 tests.
```

## Documentation

