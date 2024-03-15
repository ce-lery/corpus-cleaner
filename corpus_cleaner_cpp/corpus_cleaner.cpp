#include "corpus_cleaner.hpp"
#include "util.hpp"
#include "normalizer.hpp"
#include "language_filter.hpp"

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
                             uint32_t max_length,
                             set<string> accept_language,
                             bool sentence_segment)
{
    this->input_path = input_path;
    this->output_path = output_path;
    this->intermediate_path = "../results/intermediate/";
    this->min_length = min_length;
    this->max_length = max_length;
    this->accept_language = accept_language;
    this->sentence_segment = sentence_segment;

    mkdir(this->intermediate_path.c_str(), 0777);
    mkdir(this->output_path.c_str(), 0777);

    //copy input to output
    CopyFolder(this->input_path,this->output_path);
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
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @attention 
**/
void CorpusCleaner::LengthFilter(Document &document)
{  
    uint32_t line_length = strlen_utf8(document.text);
    if (line_length < this->min_length || this->max_length < line_length) {
        // #pragma omp ordered 
        document.is_rejected=true;
        document.metadata.insert(__func__);
    }
}

using namespace fasttext;

/**
 * @brief Language filtering 
 * @details 
 *  Please Refer document of "LanguageFilter::filter"
 * @example TODO
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @ref 
 *  https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja#python-written-by-hideaki-t--overlast
 * @attention 
**/
// void CorpusCleaner::LanguageFilter(Document &document)
// {
//     FastTextEx language_filter;
//     pair<float,string> result = language_filter.filter(document.text)
    
//     document.language = result.second;
//     document.language_score = result.first;
     
//     document.is_rejected=true;
//     if(acceppt_language.find(language)!=acceppt_language.end()){
//         // If fasttext's score is less than threshold, the text to be rejected.
//         if(document.language_score>=language_threshold){
//             document.is_rejected=false;
//         }
//     }
// }


/**
 * @brief Remove URLs matching regular expression.
 * @details 
 *  Remove URLs matching regular expression.
 *  The regular expression is "(https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".
 * @example TODO
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @attention 
**/
void CorpusCleaner::URLRemover(Document &document)
{
    static regex url_pattern(R"((https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+))");
    string sentence =  regex_replace(document.text,url_pattern,"");
 
    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}

/**
 * @brief Remove special character. For example, ☀, ♡, ☆, and so on.
 * @details 
 *  Remove emoji characters that is \U00002600(☀) to \U000027ff(⟿),
 *  \U00002190(←) to \U000021ff(⇿),\U00002300(⌀) to \U000023ff(⏿)
 *  \U00002900(⤀) to \U0000297f(⥿),\U00002b00(⬀) to \U00002bff(⯿), 
 *  and \U0001f000(🀀) to \U0001f0ff(🃿).
 *  The C++ regex library does not support 4-byte characters. 
 *  Therefore, characters like 🀀 cannot be matched using regular expressions. 
 *  So, in a full search, those that completely match the pictogram are searched and removed.
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @attention 
**/
void CorpusCleaner::SpecialCharacterRemover(Document &document)
{
    string special_character = "";
    vector<string> start_character = {"☀","←","⌀","⤀","⬀","🀀"};
    vector<int> character_range = {512,112,256,128,256,256}; 
    string sentence=document.text;
    for(int i=0;i<(int)start_character.size();i++){
        special_character = start_character[i];
        //remove special_character that is ,for example, "☀" to "⟿"
        for(int j=0;j<character_range[i];j++){
            ReplaceSubstring(sentence,special_character,"");
            special_character = CalculateNextEmoji(special_character);
        }
    }
    
    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}

/**
 * @brief Remove emoji. For example, 🤗, 🐉, 📊, and so on.
 * @details 
 *  Remove emoji characters that is \U0001F300(🌀) to \U0001F9FF(🧿).
 *  The C++ regex library does not support 4-byte characters. 
 *  Therefore, characters like 🌀 cannot be matched using regular expressions. 
 *  So, in a full search, those that completely match the pictogram are searched and removed.
 * @example TODO
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @attention 
**/
void CorpusCleaner::EmojiRemover(Document &document)
{
    string sentence = document.text;
    string emoji ="🌀";
    //remove emoji that is "🌀" to "🧿"
    for(int i=0;i<1792;i++){
        ReplaceSubstring(sentence,emoji,"");
        emoji = CalculateNextEmoji(emoji);
    }
    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}

/**
 * @brief Remove remarks. For example, [1], {245}, and so on.
 * @details 
 *  Remove remarks matching regular expression.
 *  The regular expression is "(\[([0-9]+)\]|\{([0-9]+)\})".
 * @example TODO
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @attention Don't use this on corpus that contain formulas or programs.
**/
void CorpusCleaner::QuotesRemover(Document &document)
{
    static regex remaks_pattern(R"((\[([0-9]+)\]|\{([0-9]+)\}))");
    string sentence =  regex_replace(document.text,remaks_pattern,"");

    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}


/**
 * @brief Neologd Normalize sentence
 * @details 
 *  Please Refer document of "NormalizeNeologd()"
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref 
 *  https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja#python-written-by-hideaki-t--overlast
 * @attention 
**/
void CorpusCleaner::Normalizer(Document &document)
{
    string sentence = NormalizeNeologd(document.text);
    
    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence; 
}


/**
 * @brief Simple sentence splitter for japanese text.
 * @details 
 *  I used Pragmatic Segmenter's Japanese rules as a reference for sentence separation rules.
 *  The C++ regex library does not support 4-byte characters. 
 *  Therefore, characters like 🌀 cannot be matched using regular expressions. 
 *  So, in a full search, those that completely match the pictogram are searched and removed.
 * @example TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref 
 *  https://github.com/wwwcojp/ja_sentence_segmenter/blob/main/ja_sentence_segmenter/split/simple_splitter.py
 *  https://github.com/diasks2/pragmatic_segmenter#golden-rules-japanese
 * @attention 
**/
Stats CorpusCleaner::SentenceSegmenter(string input_folder_path, string output_folder_path)
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    string target_line="",source_line="";
    vector<string> file_list;
    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileList(input_folder_path,&file_list);
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        for(int j=i;j<(int)file_list.size();j++){
            ifstream target_file(input_folder_path+"/"+file_list[i]);
            ofstream output_file(output_folder_path+"/"+file_list[i]);
            while (getline(target_file, target_line)) {
                //
                vector<string> segments;
                SegmentSentence(target_line, segments);
                for(auto sentence:segments) output_file << sentence << endl;
            }
            CopyFile(output_folder_path+"/"+file_list[i],input_folder_path+"/"+file_list[i]);
        }
    }

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    //TODO: fix here.
    Stats stats;
    stats.elapsed_time=elapsed;
    stats.file_name="";
    stats.process_name=__func__;
    stats.result_file_size=-1;
    return stats;
}

/**
 * @brief Sentence Deduplication files in the this->intermediate folder
 * @details 
 *  Follow the steps below to remove duplication between all lines of all files in the this->intermediate folder.
 *  1. Get the list of files in this->intermediate_folder and set it to vector<string> file_list
 *  2. Compare all lines of source_file and target_file in file_list.
 *  3. Check duplication between all lines of souce file and all lines of target_file.
 *  Therefore, characters like 🌀 cannot be matched using regular expressions. 
 *  I considered deduplication using set or multiset, 
 *  but I did not use this method because the file size could exceed the memory capacity.
 * @example TODO
 * @param string input_folder_path: input folder path
 * @param string output_folder_path: output folder path
 * @return Stats: statics imformation of this function.
 * @ref 
 * @attention TODO: fix return stats.
**/
Stats CorpusCleaner::ExactDeduplication(string input_folder_path, string output_folder_path)
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    string target_line="",source_line="";
    vector<string> file_list;
    bool duplication=false;
    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileList(input_folder_path,&file_list);
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        for(int j=i;j<(int)file_list.size();j++){
            ifstream target_file(input_folder_path+"/"+file_list[i]);
            ofstream output_file(output_folder_path+"/"+file_list[i]);
            uint32_t target_counter=0,source_counter=0;
            while (getline(target_file, target_line)) {
                target_counter++;
                duplication=false;
                source_counter=0;
                ifstream source_file(input_folder_path+"/"+file_list[j]);
                while(getline(source_file,source_line)){
                    source_counter++;
                    if(input_folder_path+"/"+file_list[i]==input_folder_path+"/"+file_list[j]
                        &&target_counter>=source_counter)continue;
                    // check duplication
                    if(target_line==source_line){
                        duplication=true;
                        break;
                    }
                }
                if(!duplication)   output_file << target_line << endl;
            }
            CopyFile(output_folder_path+"/"+file_list[i],input_folder_path+"/"+file_list[i]);
        }
    }

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
    //TODO: fix here.
    Stats stats;
    stats.elapsed_time=elapsed;
    stats.file_name="";
    stats.process_name=__func__;
    stats.result_file_size=-1;
    return stats;
}


Stats CorpusCleaner::PipelineStep(Document &document, void (CorpusCleaner::*cleaner)(Document &))
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now();

    // クラスメソッドを呼び出すためには、クラスのインスタンスを指定する必要がある
    (this->*cleaner)(document);

    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::duration<double>>(end - start).count(); 
    // TODO: fix second parameters
    return MakeStats(__func__,"",elapsed);
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
    vector<void (CorpusCleaner::*)(Document &)> cleaner_list = { 
        //&CorpusCleaner::URLRemover ,
        //&CorpusCleaner::ExcessFilter, 
        //&CorpusCleaner::EmojiRemover, 
        //&CorpusCleaner::SpecialCharacterRemover, 
        //&CorpusCleaner::SentenceSegmenter, 
        &CorpusCleaner::QuotesRemover, 
        }; 
    vector<Stats (CorpusCleaner::*)(string,string)> deduplicate_list = { 
        // &CorpusCleaner::MinhashDeduplication, 
        // &CorpusCleaner::ExactDeduplication, 
        }; 
    
    if(this->sentence_segment==true){
        // Loop processing as many times as deduplicate_list
        CopyFolder(this->output_path, this->intermediate_path); 
        Stats stats = SentenceSegmenter(this->intermediate_path,this->output_path);
        OutputStats(stats);
    }
    
    vector<string> filename_list;
    // copy output folder to intermediate folder
    CopyFolder(this->output_path, this->intermediate_path); 
    // Get list of files in intermediate folder
    GetFileList(this->intermediate_path,&filename_list);

    // Execute the each CorpusCleaner processing on all files in the intermediate folder.
    for (auto filename: filename_list){
    
        // load data
        ifstream input_file(this->intermediate_path+"/"+filename);
        ofstream output_file(this->output_path+"/"+filename);
        string line="";
        uint64_t line_count=0;
        while (getline(input_file, line)) {
            Document document;
            document.text = line;
            document.id = filename+to_string((unsigned long long)line_count);

            // Loop processing as many times as cleaner_list
            for (const auto& cleaner : cleaner_list) {     
                //TODO: Exclude strings that cannot be executed
                Stats stats = PipelineStep(document,(cleaner));
                //OutputStats(stats);
                // if rejected, break and turn to next line.
                if(document.is_rejected)    break;
            }
            
            // dump data
            output_file << document.text << endl; 
            line_count++;
        }
        
        input_file.close();
        output_file.close();
        
    }
    
    // Loop processing as many times as deduplicate_list
    for (const auto& step : deduplicate_list) {
        CopyFolder(this->output_path, this->intermediate_path); 
        Stats stats = (this->*step)(this->intermediate_path,this->output_path);
        OutputStats(stats);
    }
    return 0;
}



