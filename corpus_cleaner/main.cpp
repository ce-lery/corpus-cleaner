
#include "corpus_cleaner.hpp"
#include <unicode/uclean.h> // u_cleanup()
#include "simdjson.h"
using namespace simdjson;


/**
 * @brief Get line count of filename file
 * @details 
 * Example:
 *   string input_path = "../data/input/";
 *   ConutLines(input_path);
 * @param string filename: file name
 * @return uint64_t: count of file line
**/
uint64_t ConutLines(const string& filename) 
{
    ifstream file(filename);
    uint64_t line_count = 0;
    string line="";
    while (getline(file, line)) {
        line_count++;
        // cout << line_count << endl;
    }
    file.close();
    return line_count;
}

/**
 * @brief split one file into multiple equal parts based on the number of lines
 * @details 
 * @param string filename: file name
 * @return uint64_t: count of file line
**/
void SplitFiles(const vector<string>& output_files, const string& input_file) 
{
    ifstream input(input_file);

    vector<ofstream> outputs((int)output_files.size());
    // get file name of input
    filesystem::path path_string(input_file);
    string filename=path_string.filename();
    cout << "### Split Files ###" << endl;
    cout << filename << endl;
    cout << input_file<<endl;
    for (int i = 0; i < (int)output_files.size(); i++) {
        string output_filename = output_files[i]+"/"+filename;
        outputs[i].open(output_filename);
        if (!outputs[i]) {
            cerr << "Error: Can't create file: " << output_filename << endl;
            return;
        }
    }
    // cout << "split"<<endl;

    string line="";
    uint64_t chunk_count=(ConutLines(input_file)+(uint64_t)output_files.size())/(uint64_t)output_files.size();
    cout << ConutLines(input_file) << endl;
    cout << (uint64_t)output_files.size()<< endl;

    cout << "file lines:" << chunk_count << endl;
    uint64_t file_index = 0;
    uint64_t line_count = 0;
    while (getline(input, line)) {

        outputs[file_index] << line << endl;
        line_count++;
        if (line_count % chunk_count == 0) {
            file_index++;
        }
    }
    // cout << "split completed."<<endl;

    input.close();
    for (auto& output : outputs) {
        output.close();
    }
}


/**
 * @brief split one file into multiple equal parts based on the number of lines
 * @details 
 * @param sconst vector<string>& input_files: file list that is merged
 * @param const string& output_file: merged file
 * @return void: None
**/
void MergeFiles(const vector<string>& input_files, const string& output_file) 
{
    ofstream output(output_file, ios::binary);
    if (!output) {
        cerr << "Failed to open output file: " << output_file << endl;
        return;
    }

    for (const auto &input_file : input_files) {
        ifstream input(input_file, ios::binary);
        if (!input) {
            cerr << "Failed to open input file: " << input_file << endl;
            continue;
        }

        // write input files to output file
        output << input.rdbuf();
        input.close();
    }

    output.close();
}


void CorpusClean(const string input_folder_path,             
                 const string output_folder_path,
                 const string config_path)
{

    //string input_folder_path = "../../results/dataset/input/";
    //string output_folder_path = "../../results/dataset/output/";
//     uint32_t min_length= 5;
//     uint32_t max_length = 500000;
//     set<string> accept_language{"__label__ja"};
// //  RemoveFolder(output_folder_path);
//     bool store_rejected = true; 
//     bool execute_sentence_segment = false; // TODO: switch true
//     double language_threshold = 0.3;
//     double perplexity_threshold = 80000;
//     vector<string> specific_phrases = {"このページは曖昧さ回避のためのページです。一つの語句が複数の意味・職能を有する場合の水先案内のために、異なる用法を一覧にしてあります。お探しの用語に一番近い記事を選んで下さい。このページへリンクしているページを見つけたら、リンクを適切な項目に張り替えて下さい。",
//                               };

    simdjson::ondemand::parser parser;
    padded_string json = padded_string::load("../data/config.json");
    simdjson::ondemand::document config = parser.iterate(json);

    GenerateDedupLSH generate_dedup_lsh(4,200,20,10);
    LSHDeduplicator deduplicator(true,
                                 output_folder_path+"/blacklist.txt",
                                 bool(config["store_blacklist"]),
                                 1280000000);
    
    // create instance
    CorpusCleaner corpus_cleaner(input_folder_path,
                                 output_folder_path,
                                 config_path,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    
    // Execute cleaning pipeline
    corpus_cleaner.CleanPipeline(); 
}  

int MultiProcessCorpusClean(string config_file_path)
{
    // Please put the original .txt file in the original folder
    // const string original_folder_path = "../../results/dataset/original/";
    // const string base_folder_path = "../../results/dataset/";
    // const string results_folder_path = "../../results/dataset/cleaned/";
    // const string rejected_folder_path = "../../results/dataset/rejected/";
    // const string blacklist_folder_path = "../../results/dataset/blacklist/";
    filesystem::create_directories(fs::path(results_folder_path));
    filesystem::create_directories(fs::path(rejected_folder_path));
    filesystem::create_directories(fs::path(blacklist_folder_path));

    // const bool store_blacklist = false;
    // get file list
    vector<string> filelist;
    GetFileNameListWithoutExtention(original_folder_path,&filelist);

    // Get the number of CPU threads and define it as thread_number = cpu thread count - 4; (value of 1 or more)
    int32_t num_threads = std::thread::hardware_concurrency() - 2;
    num_threads = (num_threads>0) ? num_threads : 1;
    // num_threads=1;
    cout << "Multi Process Number: "<< num_threads << endl;
    
    // Create thread number folders in original_folder_path
    vector<string> output_files;
    for(int i=0;i<num_threads;i++){
        string temp = base_folder_path+to_string(i)+"/input/";
        fs::path new_directory(temp);
        filesystem::create_directories(new_directory);
        output_files.push_back(temp);
    } 
    // Divide the files under original_folder_path into thread_number and put them into the input_folder_path+"i"th folder.
    for(auto file:filelist)    SplitFiles(output_files, original_folder_path+"/"+file+".txt");

    // Use thread library to execute MultiProcessCorpusClean() in parallel for each thread
    vector<thread> threads;
    for(int i=0;i<num_threads;i++){
        string input_folder_path = base_folder_path+to_string(i)+"/input/";
        string output_folder_path = base_folder_path+to_string(i)+"/output/";
        threads.emplace_back(CorpusClean,
                             input_folder_path,
                             output_folder_path,
                             config_file_path);
    }    

    // call each thread
    for(auto& process : threads) {
        process.join();
    }
    
    //merging all results files and make them into one
    for(auto file:filelist){
        vector<string> splited_filelist;
        // Merge cleaned
        for(int i=0;i<num_threads;i++)    splited_filelist.push_back(base_folder_path+to_string(i)+"/output/cleaned/"+file+".jsonl");
        MergeFiles(splited_filelist,results_folder_path+file+".jsonl");
        for(int i=0;i<num_threads;i++)        filesystem::remove(base_folder_path+to_string(i)+"/output/cleaned/"+file+".txt");
        
        //Merge rejected
        splited_filelist.clear();
        for(int i=0;i<num_threads;i++)    splited_filelist.push_back(base_folder_path+to_string(i)+"/output/rejected/"+file+".jsonl");
        MergeFiles(splited_filelist,rejected_folder_path+file+".jsonl");
        for(int i=0;i<num_threads;i++)        filesystem::remove(base_folder_path+to_string(i)+"/output/rejected/"+file+".txt");

        // Merge blacklist
        splited_filelist.clear();
        if(store_blacklist){
            for(int i=0;i<num_threads;i++)    splited_filelist.push_back(base_folder_path+to_string(i)+"/output/blacklist.txt");
            MergeFiles(splited_filelist,blacklist_folder_path+"/blacklist_"+file+".txt");
        }
    }

    // for(int i=0;i<num_threads;i++)    RemoveFolder(base_folder_path+to_string(i));

    //cleanup normalizer
    u_cleanup();
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc > 3) {
        cout << "Argument Error. Please input 1 option."
        return -1;
    }
    string config_path = argv[1];
    MultiProcessCorpusClean(config_path); 
    return 0;
}


// #include "corpus_cleaner.hpp"

// int main(void)
// {
//     string input_folder_path = "../../results/dataset/original/";
//     string output_folder_path = "../../results/dataset/output/";
//     uint32_t min_length= 5;
//     uint32_t max_length = 5000;
//     set<string> accept_language{"__label__ja"};
// //  RemoveFolder(output_folder_path);
//     bool store_rejected = true; 
//     bool execute_sentence_segment = false; // TODO: switch true
//     double language_threshold = 0.3;
//     double perplexity_threshold = 40000;

//     GenerateDedupLSH generate_dedup_lsh(4,200,20,10);
//     LSHDeduplicator deduplicator(true,"../../results/dataset/blacklist.txt",true,1280000000);
    
//     // create instance
//     CorpusCleaner corpus_cleaner(input_folder_path,
//                                  output_folder_path,
//                                  min_length,
//                                  max_length,
//                                  accept_language,
//                                  store_rejected,
//                                  execute_sentence_segment,
//                                  language_threshold,
//                                  perplexity_threshold,
//                                  &generate_dedup_lsh,
//                                  &deduplicator);
    
//     // Execute cleaning pipeline
//     corpus_cleaner.CleanPipeline();
//     return 0;
// }