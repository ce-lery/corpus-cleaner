# corpus-cleaner

<!-- ![](image/comparison.png) -->
<!-- <img src="image/comparison.png" width="500"> -->

## Overview

**This repository is under development and the features may contain bugs.**   
**If you find any bugs, please post an issue.**

Welcome to my repository!   

This repository is a C++ library for Japanese corpus cleaning.   
The features are following table.  

|Features|Details|Remarks|
|:--|:--|:--|
|[Normalizer](corpus_cleaner/normalizer.py#L6)|Normalize sentences using [neologdn](https://github.com/ikegami-yukino/neologdn). Neologdn's normalization contents include, for example, "replace half-width katakana with full-width" and "replace full-width spaces with half-width spaces."|See [here](https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja) for details on normalization contents.|
|[Emoji Remover](corpus_cleaner/remover.py#L8)|Remove emojis using [emoji](https://pypi.org/project/emoji/) library.||
|[URL Remover](corpus_cleaner/remover.py#L22)|Remove URLs matching regular expression.||
|[Quotes Remover](corpus_cleaner/remover.py#L22)|Remove quotes. For example, [1], {245}, and so on.||
|[Zero Punctuation Filter](corpus_cleaner/remover.py#L22)|Remove sentence without punctuation that is "、", "､", "。", "｡", "．", ".", "？", "?", "！", "!".||
|[Special Characters Remover](corpus_cleaner/remover.py#L61)|Remove special character.<br> For example, ☀, ♡, ☆, and so on.<br>Removes special characters within a specific Unicode range.|Please refer [this URL](https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html).<br>Special characters to be removed include some emojis.|
|[Sentence Segmenter](corpus_cleaner/splitter.py)|Execute Rule-based sentence separation (sentence segmentation) using [ja_sentence_segmenter](https://github.com/wwwcojp/ja_sentence_segmenter).|Please refer this [article](https://qiita.com/heimaru1231/items/b6ed09d4787e4e28175a).|
|[Length Filter](corpus_cleaner/excess_filter.py)|Remove too long sentence and too short sentence.||
|[Sentence Deduplicater](https://github.com/huggingface/datatrove/blob/main/src/datatrove/pipeline/dedup/sentence_dedup.py)|Remove sentences that match exactly.|This function's implementation is in the [huggingface/datatrove](https://github.com/huggingface/datatrove).|
|[Minhash Deduplicater](corpus_cleaner/minhash.py)|Deduplicate sentence using minhash.<br>Unlike the default minhash, Mecab is used when tokenizing sentences.||
|[Language Filter](https://github.com/huggingface/datatrove/blob/main/src/datatrove/pipeline/filters/language_filter.py)|Classify sentence composition language and quality using fastText.<br> This function is applied to japanese and english.|This function's implementation is in the huggingface/datatrove. Please refer [fastText](https://fasttext.cc/docs/en/crawl-vectors.html).|
|[Perplexity Filter](https://github.com/huggingface/datatrove/blob/main/src/datatrove/pipeline/filters/language_filter.py)|Classify sentence composition language and quality using fastText.|This function's implementation is in the huggingface/datatrove. Please refer [fastText](https://fasttext.cc/docs/en/crawl-vectors.html).|


<!-- |[Remove kaomoji](corpus_cleaner/remover.py#L39)|Remove emoticons that exactly match the list in kaomoji.txt.<br>Approximately 1,400 types of emoticons will be removed.|Before using this feature, we recommend normalizing the text using TxtNormalizer().|
|[ftfy Fixer](corpus_cleaner/fixer.py#L6)|Fix broken Unicode using [ftfy](https://ftfy.readthedocs.io/en/latest/).|Inspired by [NVIDIA NeMo Data Curator](https://docs.nvidia.com/nemo-framework/user-guide/latest/modelguide/pretrainingdatasets/index.html).|
|[Txt Reader](corpus_cleaner/txt_reader.py)|BaseDiskReader that reads .txt file. |[Datatrove Readers](https://github.com/huggingface/datatrove/tree/main/src/datatrove/pipeline/readers) can read other formats (.csv,.json, ...etc.). Please read [here](https://github.com/huggingface/datatrove?tab=readme-ov-file).|
|[Txt Writer](corpus_cleaner/txt_writer.py)|DiskWriter that writes .txt file.|[Datatrove](https://github.com/huggingface/datatrove/tree/main/src/datatrove/pipeline/writers) can write other formats (.csv,.json, ...etc.). Please read [here](https://github.com/huggingface/datatrove?tab=readme-ov-file).|-->

## Quick Started

If you want to try out the contents of this repository quickly and easily, please use this [ipynb file](examples/quick_start.ipynb).
<!-- TODO: gist -->

## Getting Started

Build a python environment using Docker files.

```bash
git clone https://github.com/ce-lery/corpus-cleaner.git
cd corpus-cleaner
docker build -t corpus-cleaner-image ./
docker run -v ./:/home/corpus-cleaner/ -it --gpus all corpus-cleaner-image
```

Run the shell script with the following command.  

```bash
bash scripts/setup.sh
```

Build source code of corpus-cleaner.

```bash
bash scripts/build.sh
```

Run corpus_cleaner.

```bash
./corpus_cleaner/build/corpus_cleaner
```


## Usage

### Document

API document is [here](). Prease refer it.  

### Basic usage

The basic usage of corpus-cleaner is same as [Getting Started](#Getting_Started).

### Select Filtering feature

If you want to disable Sentence Segmenter, please set "bool sentence_segment=false", and create instance of CorpusCleaner class.

```bash
```

If you want to disable filter, please Comment out the corresponding filter function in the variable TODO:. 

Maybe, this step will be changed in the future.

## Development

### Add new filtering feature

You can add your original filtering feature. Please do the following steps.

1. Write the Filtering function in corpus_cleaner.cpp.  
    ```cpp
    aa
    aa
    ```
2. Define the prototype declaration in corpus_cleaner.hpp.  
    ```cpp
    ```
3. Build source code of corpus-cleaner.  
    ```bash
    bash scripts/build.sh
    ```
4. Run corpus_cleaner.
    ```bash
    ./corpus_cleaner/build/corpus_cleaner
    ```

<!-- The basic flow is as follows.

1. Download dataset.
2. Run main.py.   
    ```bash
    python main.py
    ``` 
3. Wait until main.py finishes processing.
4. Check the results output in the "results" folder.

If you want to add new filtering functionality, try the following steps.   
Here, I will explain how to add functions using the Normalize function as an example.  

1. Import Necessary module.  
    ```python
    import neologdn
    from datatrove.data import DocumentsPipeline
    from datatrove.pipeline.base import PipelineStep
    ```  
2. Create a new class that inherits TextPipeplineStep etc.  
    ```python
    class TxtNormalizer(PipelineStep):
    ``` 
3. Add the necessary initialization processing to the constructor.   
 (If it is not particularly necessary, you can just write the minimum string as shown below.)  
    ```python
        def __init__(
            self,
        ):
            super().__init__()
    ```
4. Write the processing details in the run() function.  
    ```python
        def run(self, data: DocumentsPipeline, rank: int = 0, world_size: int = 1) -> DocumentsPipeline:
        for document in data:
               document.text=neologdn.normalize(document.text.rstrip())
            yield document
    ``` 
5. Write a program that uses TxtNormalizer() and run it (e.g.  [example_normalizer.py](examples/example_normalizer.py)).

The complete scripts are [normalizer.py](corpus_cleaner/normalizer.py) and [example_normalizer.py](examples/example_normalizer.py). Please refer them. -->

## Test

```bash
bash scripts/test.sh
```

## Contribution

We welcome your contributions to this repository. To contribute, please see [CONTRIBUTING.md](CONTRIBUTING.md).

## TODO

### ver.0.1.0
- [x] Implement normalizer
- [x] Implement Remover emoji
- [x] Implement Remover URL
- [ ] ~~Implement Remover kaomoji~~
- [x] Implement Remover special characters
- [x] Implement Sentence Segmentation 
- [x] Implement Excess Filtering
- [x] Implement Sentence Deduplication
- [x] Implement Minhash Deduplication
- [x] Implement language filter
- [x] Implement Test
- [x] Imprement quotes Remover ("{1}", "{13}", and so on.)
- [x] Refactor piplinestep (FILE read write only once at the beginning and end)
- [x] Implement minhash
- [x] Implement json read & write
- [x] Exception handling
- [x] Test Clean pipeline
- [x] Implement tqdm.
- [x] Implement Zero punctuation filter
- [ ] Implement dump .txt format file(only is_removed=false).
- [ ] Download [OSCAR](https://huggingface.co/datasets/oscar) dataset.
- [ ] Write document & create doxygen

### ver.0.2.0

- [ ] Implement pybind & python code
- [ ] Implement loader json & jsonl
- [ ] Remove ad header and footer
- [ ] Remove HTML mark
- [ ] Remove repeated expressions
- [ ] Set Github Action's CI/CD

### ver.0.3.0

- [ ] Speedup?
