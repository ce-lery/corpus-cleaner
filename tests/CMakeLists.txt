# refer:https://qiita.com/utkamioka/items/cacb1001bd2abf605b15

# set CMAKE version
cmake_minimum_required(VERSION 3.22)
# set project name
project(test_corpus_cleaner CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(kenlm)

# include_directories(${CMAKE_SOURCE_DIR}/../corpus_cleaner/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/kenlm/lm) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/kenlm/util) 
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/icu/usr/local/include) 
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/fastText/build/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/fastText/src) 
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sentencepiece/build/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sentencepiece/src) 
# add_subdirectory(corpus_cleaner)
# add_subdirectory(tests)
# Compile sample.cpp and sample_test.cpp to create static library libgreetings.a
add_executable(${PROJECT_NAME}-googletest 
               CorpusCleaner_test.cpp 
               ../corpus_cleaner/corpus_cleaner.cpp
               ../corpus_cleaner/corpus_cleaner.hpp
               ../corpus_cleaner/util.cpp
               ../corpus_cleaner/util.hpp
               ../corpus_cleaner/normalizer.cpp
               ../corpus_cleaner/normalizer.hpp
               ../corpus_cleaner/minhash.cpp
               ../corpus_cleaner/minhash.hpp
               ../scripts/smhasher/src/MurmurHash3.cpp
               ../scripts/smhasher/src/MurmurHash3.h
               ../corpus_cleaner/language_filter.cpp
               ../corpus_cleaner/language_filter.hpp
               ../corpus_cleaner/perplexity_filter.cc
               ../corpus_cleaner/perplexity_filter.hh
              )
# Option of optimization , warning, ...etc.
target_compile_options(${PROJECT_NAME}-googletest PUBLIC -O2 -Wall)
# Create an executable file called ${PROJECT_NAME}-googletes.out from main.cpp
target_link_libraries(${PROJECT_NAME}-googletest PUBLIC gtest_main 
                      icuio 
                      icui18n 
                      icutu 
                      icuuc 
                      icudata 
                      fasttext
                      kenlm_util 
                      kenlm
                      sentencepiece)
# target_link_libraries(${PROJECT_NAME}-googletest PUBLIC )
target_compile_definitions(${PROJECT_NAME}-googletest PUBLIC -DKENLM_MAX_ORDER=${KENLM_MAX_ORDER})

# Link lib${CMAKE_CURRENT_SOURCE_DIR}.a when creating ${PROJECT_NAME}-googletes.out
target_include_directories(${PROJECT_NAME}-googletest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
enable_testing()