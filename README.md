# corpus-cleaner

<!-- ![](image/comparison.png) -->
<!-- <img src="image/comparison.png" width="500"> -->

## Overview

Welcome to my repository!   
This repository is a library for quality filtering, deduplication, and unnecessary vocabulary removal for Japanese corpus. This library wraps around [huggingface/datatrove](https://github.com/huggingface/datatrove).  

The features are following table.

<!-- |Features|Examples|Details|Remarks|
|:--|:--|:--|:--|
|[Normalizer](corpus_cleaner/normalizer.py#L6)|[here](examples/example_normalizer.py)|Normalize sentences using [neologdn](https://github.com/ikegami-yukino/neologdn). Neologdn's normalization contents include, for example, "replace half-width katakana with full-width" and "replace full-width spaces with half-width spaces."|See [here](https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja) for details on normalization contents.|
|[Remove emoji](corpus_cleaner/remover.py#L8)|[here](examples/example_remover.py)|Remove emojis using [emoji](https://pypi.org/project/emoji/) library.||
|[Remove URL](corpus_cleaner/remover.py#L22)|[here](examples/example_remover.py)|Remove URLs matching regular expression.||
|[Remove kaomoji](corpus_cleaner/remover.py#L39)|[here](examples/example_remover.py)|Remove emoticons that exactly match the list in kaomoji.txt.<br>Approximately 1,400 types of emoticons will be removed.|Before using this feature, we recommend normalizing the text using TxtNormalizer().|
|[Remove special characters](corpus_cleaner/remover.py#L61)|[here](examples/example_remover.py)|Remove special character.<br> For example, ☀, ♡, ☆, and so on.<br>Removes special characters within a specific Unicode range.|Please refer [this URL](https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html).<br>Special characters to be removed include some emojis.|
|[ftfy Fixer](corpus_cleaner/fixer.py#L6)|[here](examples/example_fixer.py)|Fix broken Unicode using [ftfy](https://ftfy.readthedocs.io/en/latest/).|Inspired by [NVIDIA NeMo Data Curator](https://docs.nvidia.com/nemo-framework/user-guide/latest/modelguide/pretrainingdatasets/index.html).|
|[Sentence Segmentation](corpus_cleaner/splitter.py)|[here](examples/example_splitter.py)|Execute Rule-based sentence separation (sentence segmentation) using [ja_sentence_segmenter](https://github.com/wwwcojp/ja_sentence_segmenter).|Please refer this [article](https://qiita.com/heimaru1231/items/b6ed09d4787e4e28175a).|
|[Excess Filtering](corpus_cleaner/excess_filter.py)|[here](examples/example_excess_filter.py)|Remove too long sentence and too short sentence.||
|[Sentence Deduplication](https://github.com/huggingface/datatrove/blob/main/src/datatrove/pipeline/dedup/sentence_dedup.py)|[here](examples/example_sentence_deduplication.py)|Remove sentences that match exactly.|This function's implementation is in the [huggingface/datatrove](https://github.com/huggingface/datatrove).|
|[Minhash Deduplication](corpus_cleaner/minhash.py)|[here](examples/example_minhash.py)|Deduplicate sentence using minhash.<br>Unlike the default minhash, Mecab is used when tokenizing sentences.||
|[Txt Reader](corpus_cleaner/txt_reader.py)|[here](examples/example_txt_reader.py)|BaseDiskReader that reads .txt file. |[Datatrove Readers](https://github.com/huggingface/datatrove/tree/main/src/datatrove/pipeline/readers) can read other formats (.csv,.json, ...etc.). Please read [here](https://github.com/huggingface/datatrove?tab=readme-ov-file).|
|[Txt Writer](corpus_cleaner/txt_writer.py)|[here](examples/example_remover.py)|DiskWriter that writes .txt file.|[Datatrove](https://github.com/huggingface/datatrove/tree/main/src/datatrove/pipeline/writers) can write other formats (.csv,.json, ...etc.). Please read [here](https://github.com/huggingface/datatrove?tab=readme-ov-file).|
|[Language filter](https://github.com/huggingface/datatrove/blob/main/src/datatrove/pipeline/filters/language_filter.py)|[here](examples/example_language_filter.py)|Classify sentence composition language and quality using fastText.|This function's implementation is in the huggingface/datatrove. Please refer [fastText](https://fasttext.cc/docs/en/crawl-vectors.html).|
|[Cleaned Analysys](corpus_cleaner/analysis.py)|[here](examples/example_analysis.py)|Visualize the file size of the dataset before and after corpus cleaning.|| -->

## Quick Started

If you want to try out the contents of this repository quickly and easily, please use this [ipynb file](examples/quick_start.ipynb).
<!-- TODO: gist -->

## Getting Started

Build a python environment using Docker files.

```bash
git clone https://github.com/ce-lery/corpus-cleaner-cpp.git
cd corpus-cleaner-cpp
docker build -t corpus-cleaner-cpp-image ./
docker run -v ./:/home/corpus-cleaner-cpp/ -it --gpus all corpus-cleaner-cpp-image
```

Run the shell script with the following command.  
Execute python virtual environment construction, pretrain, and fine tuning in order.  

```bash
bash run_all.sh
```

## Usage
TBD.
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
- [ ] Implement minhash
- [x] Implement json read & write
- [ ] Implement "," filter
- [ ] Implement tqdm.
- [ ] Write document & create doxygen
- [ ] Exception handling

### ver.0.2.0

- [ ] Convert original file's " to \".
- [ ] Implement pybind & python code
- [ ] Speedup
- [ ] Implement loader json & jsonl
- [ ] Remove ad header and footer
- [ ] Remove HTML mark
- [ ] Set Github Action's CI/CD

### ver.0.3.0

<!-- - [x] Implement MinHash
- [ ] Save removed file 
- [x] Normalize setence
- [x] Execute ftfy
- [x] Remove URL, emoji, kaomoji
- [x] Remove special characters
- [x] Split Sentence(textformatting, [ja_sentence_segmenter](https://github.com/wwwcojp/ja_sentence_segmenter))
- [x] Graph the number of file lines reduced between the original data and the removed data
- [x] test code
- [x] Write README.md
- [ ] check txt_reader.py (Is it not being processed due to high memory consumption?)
- [ ] datatrove log output
- [ ] datatrove stats output of pipeline1
- [ ] fix dataset path of examples (due to move dataset folder)
- [ ] Remove the trailing \ or \n. (Remove them by executed .strip? Test it. )
- [ ] At the end. 。, Delete lines other than.
- [ ] parallel execute for sentence segmentation
- [ ] Write quick_start.ipynb
- [ ] Graph step by step filtering
- [ ] Write main.py (ALl in one.)
- [ ] Example data download bash
- [ ] Filter of LLM perplexity (If I feel like it.) -->
