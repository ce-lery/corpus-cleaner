#include <bits/stdc++.h>

using namespace std;
namespace fs = filesystem;

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
    //TODO: add vecter of result's file size of each cleaning process. At the end, analysys it.

public:
    /***constructor***/
    CorpusCleaner(string input_path,
                  string output_path,
                  uint32_t min_length,
                  uint32_t max_length);
    /***destructor***/
    //TODO: remove intermediate_file
    ~CorpusCleaner();
    /***member function***/
    Stats ExcessFilter(string input_path, string output_path);
    Stats URLRemover(string input_path, string output_path);
    Stats SpecialCharacterRemover(string input_path, string output_path);
    Stats EmojiRemover(string input_path, string output_path);
    Stats SentenceSegmenter(string input_path, string output_path);
    Stats Normalizer(string input_path,string output_path);

    Stats SentenceDeduplication(string input_folder_path,string output_folder_path);
    double CleanPipeline();
};