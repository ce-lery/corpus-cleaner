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

# Compile sample.cpp and sample_test.cpp to create static library libgreetings.a
add_executable(${PROJECT_NAME}-googletest sample.cpp sample_test.cpp)
# Create an executable file called ${PROJECT_NAME}-googletes.out from main.cpp
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
# Link lib${CMAKE_CURRENT_SOURCE_DIR}.a when creating ${PROJECT_NAME}-googletes.out
target_include_directories(${PROJECT_NAME}-googletest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
enable_testing()