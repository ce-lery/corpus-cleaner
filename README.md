# corpus-cleaner

@mainpage  
![doxygen deploy](https://github.com/ce-lery/corpus-cleaner/actions/workflows/doxygen-gh-pages.yml/badge.svg)
![Build](https://github.com/ce-lery/corpus-cleaner/actions/workflows/build.yml/badge.svg)
![Test](https://github.com/ce-lery/corpus-cleaner/actions/workflows/test.yml/badge.svg)
<!--  -->
<!-- ![](image/comparison.png) -->
<!-- <img src="image/comparison.png" width="500"> -->

## Overview

Welcome to my repository!   
This repository is a C++ library includes quality filtering, deduplication, and unnecessary vocabulary removal for Japanese corpus.  
The features are following.

- **Normalizer**: Sentence normalization created by [mecab-neologd](https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja)
- **URL Remover**: Remove URLs matching regular expression
- **Special Characters Remover**: Remove certain special characters (☀, ♡, ☆, etc.)
- **Emoji Remover**: Remove emoji characters that is \U0001F300 to \U0001F9FF.
- **Quotes Remover**: Remove quotes ([1], {245})
- **Length Filter**: Remove too long sentence and too short sentence
- **Language Filter**: Determine whether it is a Japanese document
- **Minhash Deduplicator**: Deduplication using Minhash
- **ZeroPunctuationFilter**: Delete documents without punctuation
- **Sentence Segmenter**: Divide the corpus into sentences based on rules
- **Perplexity Filter**: Perplexity filter using kENLM

<!-- 
## Quick Started

If you want to try out the contents of this repository quickly and easily, please use this [ipynb file](examples/quick_start.ipynb).
(TODO: gist) -->

## Getting Started

### Clone Repository

```bash
git clone https://github.com/ce-lery/corpus-cleaner.git
cd corpus-cleaner
```

### Install Step

#### Docker

Build a python environment using Docker files.

```bash
docker build -t corpus-cleaner-image ./
docker run -v ./:/home/corpus-cleaner/ -it --gpus all corpus-cleaner-image
```

#### Other (Local Install)

```bash
sudo apt-get update
sudo apt-get install cmake gdb libboost-system-dev libboost-thread-dev libboost-program-options-dev libboost-test-dev libeigen3-dev zlib1g-dev libbz2-dev liblzma-dev  pkg-config  libgoogle-perftools-dev curl wget build-essential nano flex bison
```

### Common Step

Run the shell script with the following command.  
In this script, you install third party library.

```bash
bash scripts/setup.sh
```

Build source code of corpus-cleaner.

```bash
bash scripts/build.sh
```

Please place the files to be cleaned in "./results/data/original".
The file format is ".txt". For example, "wiki.txt", "cc100_train.txt", and so on.

```bash
mkdir -p results/data/original/
# Please place the files to be cleaned in "./results/data/original".
```

Run corpus_cleaner. Please wait a minute.  

```bash
bash scripts/run.sh
```

The cleaning result files will be created in "results/data/cleaned".  
The file format is jsonl.  

## Specification

Document is [here](https://ce-lery.github.io/corpus-cleaner/).  
If you want to see this tool's specification and API references, please refer [here](./docs/specification.md).

## Usage

### Basic Usage

The basic usage of corpus-cleaner is same as [Getting Started](#Getting_Started).

### Select Filtering Feature

If you want to disable Sentence Segmenter, please set "bool sentence_segment=false", and create instance of CorpusCleaner class.

```cpp
CorpusCleaner corpus_cleaner(input_folder_path,
                             output_folder_path,
                             min_length,
                             max_length,
                             accept_language,
                             store_rejected,
                             execute_sentence_segment, // <--------- switch here to false
                             language_threshold,
                             perplexity_threshold,
                             &generate_dedup_lsh,
                             &deduplicator);
```

If you want to disable filter, please Comment out the corresponding filter function in the variable TODO:. 

```cpp
int32_t CorpusCleaner::CleanPipeline(void)
{
    // Set CorpusCleaner process that will be executed.
    // They will be executed in the order you set them.
    vector<void (CorpusCleaner::*)(Document &)> cleaner_list = { 
        &CorpusCleaner::Normalizer,
        &CorpusCleaner::URLRemover,
        &CorpusCleaner::EmojiRemover, 
        # &CorpusCleaner::SpecialCharacterRemover,
        # &CorpusCleaner::QuotesRemover,   // <- If you comment or exclude function of 
        # &CorpusCleaner::LengthFilter,    // <- cleaner_list, the functions are disabled.
        # &CorpusCleaner::ZeroPunctuationFilter,
        &CorpusCleaner::LanguageFilter,
        &CorpusCleaner::MinhashDeduplication,
        &CorpusCleaner::PerplexityFilter,
    }; 
    // ~ ommit ~
}
```

Maybe, this step will be changed in the future.

<!-- ### Add new filtering feature

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
    ``` -->

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

## License

This repository is licensed under the Apache License, Version2.0.  
Please refer [LICENSE](LICENSE) file.

## Third Party Library

In this repository, I use following third party library.  
Please note the lisence.  

|Library|License|Purpose|
|:--|:--|:--|
|[icu](https://github.com/unicode-org/icu?tab=readme-ov-file)|[UNICODE LICENSE V3](https://github.com/unicode-org/icu?tab=License-1-ov-file#readme)|For NFKC normalization of Normalizer.|
|[kenlm](https://github.com/kpu/kenlm?tab=readme-ov-file)|**LGPL license**|For perplexity filtering.<br>Since I have not embedded this tool in this repository (installed it when I use it), <br>I think that this repository is not covered by the LGPL license.<br>Judging from the fact that [cc_net](https://github.com/facebookresearch/cc_net), which also uses KENLM, is under the MIT license|
|[SentencePiece](https://github.com/google/sentencepiece)|Apache-2.0 license|For tokenization in perplexity filtering.|
|[smhasher](https://github.com/rurban/smhasher)|MIT licensed.|For hash value generation for Mihash processing.|
|[simdjson](https://github.com/simdjson/simdjson)|Apache-2.0 license|For jsonl parsing.|
|[fastText](https://github.com/facebookresearch/fastText)|MIT license|For language filtering.|
|[GoogleTest](https://github.com/google/googletest)|BSD-3-Clause license|For test.|
|[doxygen](https://github.com/doxygen/doxygen)|(GPL-2.0 license)|For Documentation.<br>This license does not apply to works produced by doxygen|

## Test

```bash
bash scripts/test.sh
```

## Contribution

We welcome your contributions to this repository.
To contribute, please see [CONTRIBUTING.md](CONTRIBUTING.md).

## TODO

### ver.0.1.0

- [ ] Write document & create doxygen

### ver.0.2.0

- [x] Set Github Action's CI/CD（build）
- [ ] Implement pybind & python code
- [ ] Implement loader json & jsonl
- [ ] Morphological analysis (by jagger)
- [ ] Remove ad header and footer
- [ ] Remove HTML mark
- [ ] Implement dump .txt format file(only is_removed=false).
- [ ] Remove repeated expressions

### ver.0.3.0

- [ ] Speedup?
