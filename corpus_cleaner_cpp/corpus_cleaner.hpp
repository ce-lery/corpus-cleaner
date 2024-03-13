#include <bits/stdc++.h>

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
 *
 *       These will be used later for drawing processing, etc.
 * @note    
**/
typedef struct _DOCUMENT {
    string text="";
    string id="";
    bool is_rejected=false;
    set<string> metadata;
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
    uint32_t min_length=5;
    uint32_t max_length=1000;
    set<string> accept_language{"__label__ja"};
    //TODO: add vecter of result's file size of each cleaning process. At the end, analysys it.

public:
    /***constructor***/
    CorpusCleaner(string input_path,
                  string output_path,
                  uint32_t min_length,
                  uint32_t max_length,
                  set<string> accept_language);
    /***destructor***/
    ~CorpusCleaner();
    /***member function***/
    void LengthFilter(Document &document);
    void URLRemover(Document &document);
    void SpecialCharacterRemover(Document &document);
    void EmojiRemover(Document &document);
    void QuotesRemover(Document &document);
    void Normalizer(Document &document);
    void LanguageFilter(Document &document);
    Stats PipelineStep(Document &document, void (CorpusCleaner::*cleaner)(Document &));
    Stats SentenceSegmenter(string input_folder_path,string output_folder_path);
    Stats ExactDeduplication(string input_folder_path,string output_folder_path);
    Stats MinhashDeduplication(string input_folder_path, string output_folder_path);
    double CleanPipeline();
};