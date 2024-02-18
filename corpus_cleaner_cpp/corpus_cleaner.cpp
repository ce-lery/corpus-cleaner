#include "corpus_cleaner.hpp"
#include "util.hpp"

/**
 * @brief Format statistics
 * @details 
 * @example
 *   string input_path = "../data/input/test_URLRemover.txt";
 *   double elapsed_time = 11.56;
 *   string process_name = "URLRemover";
 *   Stats stats = MakeStats(process_name,input_path,elapsed_time);
 * @param string process_name: Cleaning filter name.
 * @param string output_path: Path of file for statistics.
 * @param double elapsed_time: elapsed process time.
 * @return Stats: statistics
 * @attention 
**/
Stats MakeStats(string process_name,
                string output_path,
                double elapsed_time)
{
    Stats stats;

    int index_filename = output_path.find_last_of("/")+1;
    stats.process_name=process_name;
    stats.file_name=output_path.substr(index_filename);
    stats.elapsed_time=elapsed_time;
    stats.result_file_size=filesystem::file_size(output_path);
    return stats;
}   

/**
 * @brief Output statistics
 * @details 
 * @example
 * @param Stats stats: Statistics to be output.
 * @return None
 * @attention 
**/
void OutputStats(Stats stats)
{
    cout <<"######### "<<stats.process_name<<" #########"<<endl;
    cout << "file_name:" << stats.file_name << endl;
    cout << "elapsed_time[s]:"<< stats.elapsed_time << endl;
    cout << "result file size[Byte]:"<<stats.result_file_size<<endl;
    cout << endl;
}

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
CorpusCleaner::~CorpusCleaner()
{
    //remove intermediate folder
    RemoveFolder(this->intermediate_path);
}
/***member function***/

/**
 * @brief Remove too long sentence and too short sentence.
 * @details 
 *  Remove too long sentence that is length is more thanand too short sentence.
 *  The length of too long sentence is more than "max_length".
 *  The length of too short sentence is lesser than "min_length".
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @param uint32_t min_length=5: Minimum threshold to count as 1 sentence.
 * @param uint32_t max_length=1000: Maximum threshold to count as 1 sentence.
 * @return double: elapsed time
 * @attention 
**/
Stats CorpusCleaner::ExcessFilter(string input_path, string output_path)
{  
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    ifstream input_file(input_path);
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
    // cout << "Excess filtering is completed." << endl;

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    return MakeStats(__func__,output_path,elapsed);
}


/**
 * @brief Remove URLs matching regular expression.
 * @details 
 *  Remove URLs matching regular expression.
 *  The regular expression is "(https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @attention 
**/
Stats CorpusCleaner::URLRemover(string input_path, string output_path)
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
    // cout << "Removing URL is completed." << endl;

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    return MakeStats(__func__,output_path,elapsed);
}

// static std::wstring ConvertUTF8ToWstring(const std::string& src)
// {
// 	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
// 	return converter.from_bytes(src);
// }
// static std::string ConvertWstringToUTF8(const std::wstring& src)
// {
// 	std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
// 	return converter.to_bytes(src);
// }
/**
 * @brief Remove special character. For example, ☀, ♡, ☆, and so on.
 * @details 
 *  Remove special characters matching regular expression.
 *  The regular expression is 
 *  ([\u00002600-\u000027FF]|[\u00002190-\u000021FF]|[\u00002300-\u000023FF]|
 *  [\u00002900-\u0000297F]|[\u00002b00-\u00002bFF]).
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @attention 
**/
// Stats CorpusCleaner::SpecialCharacterRemover(string input_path, string output_path)
// {
//     chrono::system_clock::time_point start, end;
//     start = chrono::system_clock::now(); 

//     ifstream input_file(input_path);
//     ofstream output_file(output_path);
//     string line="";

//     wregex emoji_regex(L"[\u2600-\u27ff]|[\u2190-\u21FF]|[\u2300-\u23FF]|[\u2900-\u297F]|[\u2b00-\u2bFF]");
//     // #pragma omp parallel for ordered
//     while (getline(input_file, line)) {
//         cout << line << endl;

//         wstring line_u16 = ConvertUTF8ToWstring(line);
//         line_u16 =  regex_replace(
//             line_u16, 
//             emoji_regex,
//             L"");

//         // #pragma omp ordered 
//         {output_file << ConvertWstringToUTF8(line_u16) << endl;}
//         cout << ConvertWstringToUTF8(line_u16) << endl;
//     }
//     input_file.close();
//     output_file.close();
//     // cout << "Removing URL is completed." << endl;

//     end = chrono::system_clock::now(); 
//     double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
//     return MakeStats(__func__,output_path,elapsed);
// }

/**
 * @brief Remove emoji. For example, 🤗, 🐉, 📊, and so on.
 * @details 
 *  Remove emoji characters that is \U0001F300(🌀) to \U0001F9FF(🧿).
 *  The C++ regex library does not support 4-byte characters. 
 *  Therefore, characters like 🌀 cannot be matched using regular expressions. 
 *  So, in a full search, those that completely match the pictogram are searched and removed.
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @attention 
**/
Stats CorpusCleaner::EmojiRemover(string input_path, string output_path)
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    ifstream input_file(input_path);
    ofstream output_file(output_path);
    string line="";

    string emoji = "";
    // #pragma omp parallel for ordered
    while (getline(input_file, line)) {
        emoji = "🌀";
        //remove emoji that is "🌀" to "🧿"
        for(int i=0;i<1792;i++){
            line = regex_replace(line,regex(emoji),"");
            emoji = CalculateNextEmoji(emoji);
        }
        // #pragma omp ordered 
        {output_file << line << endl;}
    }
    input_file.close();
    output_file.close();
    // cout << "Removing URL is completed." << endl;

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    return MakeStats(__func__,output_path,elapsed);
}


/**
 * @brief Pipeline that sequentially executes the configured CorpusCleaner methods
 * @details 
 *  Perform the following steps in order.
 *  1. Set CorpusCleaner process to pipeline_list that will be executed. (Please read attention.)
 *  2. Loop processing as many times as pipeline_list
 *      2-1. copy output folder to intermediate folder
 *      2-2. Get list of files in intermediate folder.
 *      2-3. Execute the each CorpusCleaner processing on all files in the intermediate folder.
 * @example
 *  string input_path = "../data";
 *  string output_path = "../results/corpus_cleaner";
 *  CorpusCleaner corpus_cleaner(input_path,output_path,5,1000);
 *  corpus_cleaner.CleanPipeline();
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return None
 * @attention 
 *  CorpusCleaner processing is performed in the order set in Cleaner_array.
 *  For example, set cleaner_array as follows:
 *      vector<Stats (CorpusCleaner::*)(string,string)> cleaner_array = { 
 *          &CorpusCleaner::URLRemover ,
 *          &CorpusCleaner::ExcessFilter, 
 *          &CorpusCleaner::SpecialCharacterRemover}; 
 *  At this time, processing is performed in the order of 
 *  1. URLRemover, 2. ExcessFilter, and 3. SpecialCharacterRemover.
**/
double CorpusCleaner::CleanPipeline()
{
    // Set CorpusCleaner process that will be executed.
    // They will be executed in the order you set them.
    vector<Stats (CorpusCleaner::*)(string,string)> cleaner_array = { 
        // &CorpusCleaner::URLRemover ,
        // &CorpusCleaner::ExcessFilter, 
        &CorpusCleaner::EmojiRemover, 
        // &CorpusCleaner::SpecialCharacterRemover, 
        }; 

    // cout << cleaner_array.size() << endl;
    // Loop processing as many times as pipeline_list
    for (const auto& step : cleaner_array) {
        vector<string> filename_list;
    
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
                filename_list.push_back(string(entry.path().filename()));
            }
        }
        // Execute the each CorpusCleaner processing on all files in the intermediate folder.
        for (auto filename: filename_list){
            Stats stats = (this->*step)(this->intermediate_path+"/"+filename,this->output_path+"/"+filename);
            OutputStats(stats);
        }
    }
    return 0;
}



