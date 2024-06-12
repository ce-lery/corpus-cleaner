#include <bits/stdc++.h>
#include "language_filter.hpp"
#include "perplexity_filter.hh"
#include "minhash.hpp"
#include "jagger.h"
#include "normalizer.hpp"

using namespace std;
namespace fs = filesystem;

/**
 * @brief   Structure for storing statistical information for each process of CorpusCleaner
 * @details 
 *      Each process of CorpusCleaner obtains the following specific information.
 *       - text: one sentence of corpus
 *       - id: text identification
 *       - is_rejected: True if this text is eligible for deletion
 *       - metadata: tags added during the filtering process
 *       - language: Language determined by LanguageFilter 
 *       - language_score: Language score calculated by LanguageFilter
 *       - perplexity: perplexity calculated by PerplexityFilter
 *       These will be used later for drawing processing, etc.
 * @note    
**/
typedef struct _DOCUMENT {
    string text="";
    string id="";
    bool is_rejected=false;
    set<string> metadata;
    string language;
    float language_score=0;
    double noun_ratio=0;
    double perplexity=999999;
} Document;


/**
 * @brief   Structure for storing statistical information for each process of CorpusCleaner
 * @details 
 *      Each process of CorpusCleaner obtains the following statistical information.
 *       1. CopusCleaner processing name
 *       2. Processed file name
 *       3. Elapsed processing time
 *       4. File size after processing
 *
 *       These will be used later for drawing processing, etc.
 * @note    
**/
typedef struct _STATS {
    string process_name;
    string file_name;
    double elapsed_time;
    uint32_t result_file_size;
} Stats;
void ConvertTextToDocument(string sentence, 
                           string filename, 
                           string file_line_count,
                           Document &document);
void ConvertInputFilesToJsonl(string input_folder_path,string output_folder_path);
void ReadDocumentFromJsonlOneLine(Document &document,string input_jsonl_line);
void WriteDocumentToJsonl(Document &document,string output_file_path);
Stats MakeStats(string process_name,
                string output_path,
                double elapsed_time);
void OutputStats(Stats stats);

class CorpusCleaner
{
private:
    /***member value***/
    string input_path;
    string intermediate_path;
    string output_path;
    string rejected_path;
    string exception_path;
    uint32_t min_length=5;
    uint32_t max_length=1000;
    set<string> accept_language{"__label__ja"};
    bool sentence_segment=true;
    bool store_rejected=true;
    float language_threshold=0.3;
    double perplexity_threshold=999999;
    fasttext::FastTextEx language_filter;
    KenLMFilter kenlm_filter;
    //TODO: add vecter of result's file size of each cleaning process. At the end, analysys it.
    GenerateDedupLSH *generate_dedup_lsh;  
    LSHDeduplicator *deduplicator; 
    Jagger jagger_parser;
    StringNormalizer string_normalizer;

public:
    /***constructor***/
    CorpusCleaner(string input_path,
                  string output_path,
                  uint32_t min_length,
                  uint32_t max_length,
                  set<string> accept_language,
                  bool store_rejected,
                  bool sentence_segment,
                  float language_threshold,
                  double perplexity_threshold,
                  GenerateDedupLSH *generate_dedup_lsh,
                  LSHDeduplicator *deduplicator);
    /***destructor***/
    ~CorpusCleaner();
    /***member function***/
    void ControlCharacterRemover(Document &document);
    void Normalizer(Document &document);
    void URLRemover(Document &document);
    void SpecialCharacterRemover(Document &document);
    void EmojiRemover(Document &document);
    void QuotesRemover(Document &document);
    void LengthFilter(Document &document);
    void LanguageFilter(Document &document);
    void PerplexityFilter(Document &document);
    void MinhashDeduplication(Document &document);
    void ZeroPunctuationFilter(Document &document);
    void NounRatioFilter(Document &document);
    void SentenceSegmenter(string input_folder_path,string output_folder_path);
    Stats PipelineStep(Document &document, void (CorpusCleaner::*cleaner)(Document &));
    int32_t CleanPipeline(void);
    void StoreException(string function_name, string reference);
};