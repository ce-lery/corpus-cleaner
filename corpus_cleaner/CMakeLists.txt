# refer:https://qiita.com/utkamioka/items/cacb1001bd2abf605b15

# set CMAKE version
cmake_minimum_required(VERSION 3.22)
# set project name
project(corpus_cleaner CXX)

add_subdirectory(kenlm)

# include_directories(${CMAKE_SOURCE_DIR}/../corpus_cleaner/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kenlm/lm) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kenlm/util) 
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/include) 
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/fastText/build/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/fastText/src) 
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sentencepiece/build/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sentencepiece/src) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/jagger-extension/jagger-2023-02-18/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/jagger-extension/jagger-2023-02-18)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/include)

# Compile sample.cpp and sample_test.cpp to create static library libgreetings.a
add_executable(${PROJECT_NAME}
               main.cpp 
               corpus_cleaner.cpp
               corpus_cleaner.hpp
               util.cpp
               util.hpp
               normalizer.cpp
               normalizer.hpp
               minhash.cpp
               minhash.hpp
               ../scripts/smhasher/src/MurmurHash3.cpp
               ../scripts/smhasher/src/MurmurHash3.h
               ../scripts/jagger-extension/jagger-2023-02-18/src/jagger.cc
               ../scripts/jagger-extension/jagger-2023-02-18/src/ccedar_core.cc
               language_filter.cpp
               language_filter.hpp
               perplexity_filter.cc
               perplexity_filter.hh
               simdjson.cpp
               simdjson.h
              )
# Option of optimization , warning, ...etc.
target_compile_options(${PROJECT_NAME} PUBLIC -O2 -Wall -pthread -g)
# Create an executable file called ${PROJECT_NAME}.out from main.cpp
target_link_libraries(${PROJECT_NAME} PUBLIC
                      icuio 
                      icui18n 
                      icutu 
                      icuuc 
                      icudata 
                      fasttext
                      kenlm_util 
                      kenlm
                      sentencepiece)
# target_link_libraries(${PROJECT_NAME} PUBLIC )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DKENLM_MAX_ORDER=${KENLM_MAX_ORDER})

# Link lib${CMAKE_CURRENT_SOURCE_DIR}.a when creating ${PROJECT_NAME}-googletes.out
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
