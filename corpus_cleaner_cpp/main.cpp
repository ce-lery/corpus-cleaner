
#include "corpus_cleaner.cpp"
#include "util.cpp"

int main(void)
{
    string input_path = "../data/";
    string output_path = "../results/urlremover_test/";
    CorpusCleaner corpus_cleaner(input_path,output_path,5,1000);
    // uint32_t min_length=10;
    // uint32_t max_lemgth = 1000;
    // ExcessFilter(input_path,output_path,min_length,max_lemgth);
    corpus_cleaner.CleanPipeline();
    return 0;
}