#include <gtest/gtest.h>
#include "../corpus_cleaner_cpp/corpus_cleaner.hpp"
#include "../corpus_cleaner_cpp/util.hpp"

// namespace {

class CorpusCleanerTest : public ::testing::Test {
protected:
    // You can freely delete empty functions in the following functions.

    CorpusCleanerTest() {
        // Write the setup that will be executed for each test here.
    }

    virtual ~CorpusCleanerTest() {
        // Write here the clean-up that will be executed 
        //after each test and will not throw an exception.
    }

    // When constructor and destructor are not enough.
    // You can define the following methods:

    virtual void SetUp() {
        // This code is right after the constructor (just before each test)
        // will be called.
        string output_path = "../data/output/";
        mkdir(output_path.c_str(), 0777);
    }

    virtual void TearDown() {
        // This code is placed immediately after each test (just before the destructor)
        // will be called.
        string output_path = "../data/output/";
        RemoveFolder(output_path);
    }

    // Objects declared here are available to all tests within the test case.
    //string a;
    //uin32_t b;
};

// } //namespace

bool CompareFiles(const string& file1, const string& file2) {
    ifstream f1(file1, ios::binary);
    ifstream f2(file2, ios::binary);

    //compare file size
    if(filesystem::file_size(file1)!=filesystem::file_size(file2))  return false;

    // compare file contents
    if (f1 && f2) {
        char c1, c2;
        while (f1.get(c1) && f2.get(c2)) {
            if (c1 != c2)   return false; //file1!=file2
        }
        return true; //file1==file2
    } 
    else    return false; //can't open file, or can't find file.
}

TEST_F(CorpusCleanerTest, Excessfilter) {
    string input_path = "../data/input/test_ExcessFilter.txt";
    string output_path = "../data/output/test_ExcessFilter.txt";
    string answer_path = "../data/answer/test_ExcessFilter.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.ExcessFilter(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}


TEST_F(CorpusCleanerTest, URLRemover) {
    string input_path = "../data/input/test_URLRemover.txt";
    string output_path = "../data/output/test_URLRemover.txt";
    string answer_path = "../data/answer/test_URLRemover.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.URLRemover(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}
