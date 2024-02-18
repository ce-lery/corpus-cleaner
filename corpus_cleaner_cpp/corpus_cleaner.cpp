#include "corpus_cleaner.hpp"
#include "util.hpp"


/***constructor***/
CorpusCleaner::CorpusCleaner(string input_path,
                             string output_path,
                             uint32_t min_length,
                             uint32_t max_length)

{
    this->input_path = input_path;
    this->output_path = output_path;
    this->intermediate_path = "../results/intermediate/";
    this->min_length = min_length;
    this->max_length = max_length;

    mkdir(this->intermediate_path.c_str(), 0777);
    mkdir(this->output_path.c_str(), 0777);
    int path_i = this->input_path.find_last_of("/")+1;
    string input_filename = this->input_path.substr(path_i);

    //copy input to output
    fs::path sourceDir = input_path;
    fs::path destinationDir = output_path;
    for (const auto &file : fs::directory_iterator(sourceDir)) {
        if (fs::is_regular_file(file.path())) {
            fs::copy(file, destinationDir / file.path().filename(), fs::copy_options::overwrite_existing);
        }
    }
}
/***deconstructor***/
//TODO: remove intermediate_file
CorpusCleaner::~CorpusCleaner()
{
    rmdir(this->intermediate_path.c_str());
}
/***member function***/

/**
 * @brief Remove too long sentence and too short sentence.
 * @details 
 *  Remove too long sentence that is length is more thanand too short sentence.
 *  The length of too long sentence is more than "max_length".
 *  The length of too short sentence is lesser than "min_length".
 * @example
 *   string input_path="wiki_train.txt";
 *   string output_path = "wiki_train2.txt";
 *   uint32_t min_length=10;
 *   uint32_t max_lemgth = 1000;
 *   ExcessFilter(input_path,output_path,min_length,max_lemgth);
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @param uint32_t min_length=5: Minimum threshold to count as 1 sentence.
 * @param uint32_t max_length=1000: Maximum threshold to count as 1 sentence.
 * @return double: elapsed time
 * @attention 
**/
double CorpusCleaner::ExcessFilter(string input_path, string output_path)
{  
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    std::ifstream input_file(input_path);
    ofstream output_file(output_path);
    string line="";

    // #pragma omp parallel for ordered
    while (getline(input_file, line)) {
        uint32_t line_length = strlen_utf8(line);
        if (this->min_length <= line_length && line_length <= this->max_length) {
            // #pragma omp ordered 
            {output_file << line << endl;}
        }
    }
    input_file.close();
    output_file.close();
    cout << "Excess filtering is completed." << endl;

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    // cout << "Elapsed time: " << elapsed << " seconds" << endl;
    return elapsed;
}


/**
 * @brief Remove URLs matching regular expression.
 * @details 
 *  Remove URLs matching regular expression.
 *  The regular expression is "(https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".
 * @example
 *      string input_path = "temp.txt";
 *      string output_path = "temp1.txt";
 *      CorpusCleaner corpus_cleaner(input_path,output_path);
 *      corpus_cleaner.URLRemover();
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return None
 * @attention 
**/
double CorpusCleaner::URLRemover(string input_path, string output_path)
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    ifstream input_file(input_path);
    ofstream output_file(output_path);
    string line="";

    // #pragma omp parallel for ordered
    while (getline(input_file, line)) {
        line =  regex_replace(line, regex(R"((https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+))"), "");
        // #pragma omp ordered 
        {output_file << line << endl;}
    }
    input_file.close();
    output_file.close();
    cout << "Removing URL is completed." << endl;

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    // cout << "Elapsed time: " << elapsed << " seconds" << endl;
    return elapsed;
}

/**
 * @brief Remove URLs matching regular expression.
 * @details 
 *  Remove URLs matching regular expression.
 *  The regular expression is "(https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".
 * @example
 *      string input_path = "temp.txt";
 *      string output_path = "temp1.txt";
 *      CorpusCleaner corpus_cleaner(input_path,output_path);
 *      corpus_cleaner.URLRemover();
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return None
 * @attention 
**/
double CorpusCleaner::CleanPipeline()
{
    // using MyFunc =  double (CorpusCleaner::*)();// メンバ関数ポインタの型を定義
    vector<double (CorpusCleaner::*)(string,string)> cleaner_array = { &CorpusCleaner::URLRemover ,&CorpusCleaner::ExcessFilter, }; // メンバ関数ポインタの配列を作成

    // double (CorpusCleaner::*pipeline[])() = {&CorpusCleaner::ExcessFilter,&CorpusCleaner::URLRemover};
    cout << cleaner_array.size() << endl;
    for (const auto& step : cleaner_array) {
        vector<string> filename_list;
        // pipeline_listの数だけ、処理をループ
        // copy output folder to intermediate folder
        fs::path sourceDir = this->output_path;
        fs::path destinationDir = this->intermediate_path;
        for (const auto &file : fs::directory_iterator(sourceDir)) {
            if (fs::is_regular_file(file.path())) {
                fs::copy(file, destinationDir / file.path().filename(), fs::copy_options::overwrite_existing);
            }
        }
        // Get list of files in intermediate folder
        fs::path path = this->intermediate_path;
        for (const auto &entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                // std::cout << entry.path().filename() << std::endl;
                filename_list.push_back(string(entry.path().filename()));
            }
        }
        // Perform the following processing on all files in the intermediate folder.
        for (auto filename: filename_list){
            (this->*step)(this->intermediate_path+"/"+filename,this->output_path+"/"+filename);
        }
    }
    return 0;
}



