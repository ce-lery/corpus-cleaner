

#include "corpus_cleaner.hpp"

int main(void)
{
    string input_folder_path = "../../results/dataset/input/";
    string output_folder_path = "../../results/dataset/output/";
    uint32_t min_length= 5;
    uint32_t max_length = 1000;
    set<string> accept_language{"__label__ja"};
//  RemoveFolder(output_folder_path);
    bool execute_sentence_segment = true;
    double language_threshold = 0.3;
    double perplexity_threshold = 15000;
    
    GenerateDedupLSH generate_dedup_lsh(4,200,20,10);
    LSHDeduplicator deduplicator(true,"../../results/dataset/blacklist.txt",true,5120);
    
    // create instance
    CorpusCleaner corpus_cleaner(input_folder_path,
                                 output_folder_path,
                                 min_length,
                                 max_length,
                                 accept_language,
                                 execute_sentence_segment,
                                 language_threshold,
                                 perplexity_threshold,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    
    // Execute cleaning pipeline
    corpus_cleaner.CleanPipeline();
    return 0;
}