#include "corpus_cleaner.hpp"
#include "util.hpp"
#include "normalizer.hpp"
#include "simdjson.h"

using namespace simdjson;

/**
 * @brief Loggging Document to output_file_path
 * @details 
 *
 * Example:
 *
 * string input_file_path = "./input.jsonl";
 * ifstream ifs(input_file_path);
 * Document document;
 * string line = "";
 * while(getline(ifs,line)){
 *     ReadDocumentFromJsonl(document,line);  
 *     // Write process for document.
 * } 
 *
 * @param Document document: document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @attention 
**/
void ReadDocumentFromJsonlOneLine(Document &document,
                                  string input_jsonl_line)
{
    simdjson::ondemand::parser parser;
    simdjson::ondemand::document jsonl_line = parser.iterate(input_jsonl_line);   
    string_view line_view;
    
    jsonl_line["text"].get(line_view);
    document.text = string(line_view);
    
    jsonl_line["id"].get(line_view); 
    document.id  = string(line_view); 
    
    jsonl_line["is_rejected"].get(line_view);
    document.is_rejected = stoi(string(line_view));
    
    //split metadata to ,
    jsonl_line["metadata"].get(line_view);
    string tmp = string(line_view);
    stringstream ss(tmp);
    string token;
    while(getline(ss, token, ',')){
        document.metadata.insert(token);
    } 

    jsonl_line["language"].get(line_view);
    document.language=string(line_view);

    jsonl_line["language_score"].get(line_view);
    document.language_score=stod(string(line_view));
    
    jsonl_line["perplexity"].get(line_view);
    document.perplexity=stod(string(line_view));
}   

/**
 * @brief Loggging Document to output_file_path
 * @details 
 * @param Document document: document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @attention 
**/
void WriteDocumentToJsonl(Document &document,
                         string output_file_path)
{
    ofstream output_file(output_file_path, ios::app);
    
    output_file << "{" ;
    output_file << "\"text\":\"" <<document.text << "\",";
    output_file << "\"id\":\"" << document.id << "\","; 
    output_file << "\"is_rejected\":\"" << document.is_rejected << "\",";
    output_file << "\"metadata\":\"";
    for (auto iter = document.metadata.begin(); iter != document.metadata.end(); ++iter) {
        output_file << *iter << ",";
    }
    output_file << "\",";
    output_file << "\"language\":\"" <<document.language << "\",";
    output_file << "\"language_score\":\"" <<document.language_score << "\",";
    output_file << "\"perplexity\":\"" <<document.perplexity << "\"";
    output_file <<"}"<< endl;
    output_file.close();
}   


/**
 * @brief Convert input files to jsonl that has Document's element.
 * @details 
 *  * @param Document document: document
 * @param Document document: document
 * @param Document document: document
 * @param Document document: output document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @attention 
**/
void ConvertTextToDocument(string sentence, 
                           string filename, 
                           string file_line_count,
                           Document &document)
{
    document.text = sentence;
    document.id  = filename+"_"+file_line_count; 
    document.is_rejected = false;
    // document.metadata;
    document.language="";
    document.language_score=-1;
    document.perplexity=-1;
}

/**
 * @brief Convert input files to jsonl that has Document's element.
 * @details 
 * @param Document document: document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @attention 
**/
void ConvertInputFilesToJsonl(const string input_folder_path,
                              const string output_folder_path)
{

    string target_line="",source_line="";
    vector<string> file_list;
    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileNameListWithoutExtention(input_folder_path,&file_list);
    Document document; 
    string line;
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        ifstream input_file(input_folder_path+"/"+file_list[i]+".txt"); 
        string output_file_path(output_folder_path+"/"+file_list[i]+".jsonl");
        uint64_t line_count =0;
        while(getline(input_file, line)){
            ConvertTextToDocument(line,file_list[i],to_string(line_count),document);
            WriteDocumentToJsonl(document,output_file_path);
            line_count++;
        }
        input_file.close();

    } 
}   

/**
 * @brief Format statistics
 * @details 
 * 
 * The usage is following.
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
                             bool sentence_segment,
                             float language_threshold,
                             double perplexity_threshold)
{
    this->input_path = input_path;
    this->output_path = output_path;
    this->intermediate_path = "../results/intermediate/";
    this->min_length = min_length;
    this->max_length = max_length;
    this->accept_language = accept_language;
    this->sentence_segment = sentence_segment;
    this->language_threshold = language_threshold;
    this->perplexity_threshold = perplexity_threshold;

    mkdir(this->intermediate_path.c_str(), 0777);
    mkdir(this->output_path.c_str(), 0777);

    //copy input to output
    //TODO: Read from input_path's files, and write to output_path in jsonl format.
    ConvertInputFilesToJsonl(this->input_path,this->output_path);

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


/**
 * @brief KenLM's Perplexity Quality filtering 
 * @details 
 *  Please Refer document of "TODO"
 *  1. 
 *  2. If the perplexity is less than "threshold", the "document" is to be rejected.
 *  
 *  The usage is following.
 *  
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @ref TODO
 * @attention 
**/
void CorpusCleaner::PerplexityFilter(Document &document)
{
    document.perplexity = this->kenlm_filter.PerplexityWithSentencePiece(ConvertUTF8ToWstring(document.text));
    
    // If kenlm's perplexity is less than threshold, the text is to be rejected.
    document.is_rejected=true;
    if(document.perplexity<=this->perplexity_threshold){
        document.is_rejected=false;
    }

    if(document.is_rejected)    document.metadata.insert(__func__);
}

/**
 * @brief Language filtering 
 * @details 
 *  string in = "吾輩は猫である。名前はまだ無い。";
 *  FastTextEx language_filter;
 *  pair<float, string> score;
 *  score = language_filter.filter(in);
 *  // score.first ==1.00005, score.second ==__label__ja
 * 
 *  string in2 = "I am a cat. No name yet.";
 *  score = language_filter.filter(in2);
 *  // score.first ==0.75237, score.second ==__label__en 
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @ref 
 *  https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja#python-written-by-hideaki-t--overlast
 *  https://fasttext.cc/docs/en/supervised-tutorial.html
 * @attention 
**/
void CorpusCleaner::LanguageFilter(Document &document)
{
    vector<pair<float, string>> predictions;
    int32_t k = 1;
    float threshold = 0.0;

    this->language_filter.predictOneLine(document.text, predictions, k, threshold);
    //return pair<float, string> : float:　Language assessment score, string: Language determination results
    pair<float,string> result = make_pair((float)predictions[0].first,predictions[0].second);

    document.language = result.second;
    document.language_score = result.first;

    document.is_rejected=true;
    if(accept_language.find(document.language)!=accept_language.end()){
        // If fasttext's score is less than threshold, the text to be rejected.
        if(document.language_score>=this->language_threshold){
            document.is_rejected=false;
        }
    }
    if(document.is_rejected)    document.metadata.insert(__func__);
}


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
    GetFileNameListWithoutExtention(input_folder_path,&file_list);
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        for(int j=i;j<(int)file_list.size();j++){
            ifstream target_file(input_folder_path+"/"+file_list[i]+".jsonl");
            ofstream output_file(output_folder_path+"/"+file_list[i]+".jsonl");
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
    GetFileNameListWithoutExtention(input_folder_path,&file_list);
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

// /**
//  * @brief MinHashLSH Deduplication files in the this->intermediate folder
//  * @details 
//  *  Follow the steps below to remove duplication between all lines of all files in the this->intermediate folder.
//  *  1. Get the list of files in this->intermediate_folder and set it to vector<string> file_list
//  *  2. Compare all lines of source_file and target_file in file_list.
//  *  3. Check duplication between all lines of souce file and all lines of target_file.
//  *  Therefore, characters like 🌀 cannot be matched using regular expressions. 
//  *  I considered deduplication using set or multiset, 
//  *  but I did not use this method because the file size could exceed the memory capacity.
//  * 
//  * The usage is following.
//  * 
//  * @param string input_folder_path: input folder path
//  * @param string output_folder_path: output folder path
//  * @return Stats: statics imformation of this function.
//  * @ref 
//  * @attention TODO: fix return stats.
// **/
// Stats CorpusCleaner::MinhashDeduplication(string input_folder_path, string output_folder_path)
// {
//     chrono::system_clock::time_point start, end;
//     start = chrono::system_clock::now(); 

//     string target_line="",source_line="";
//     vector<string> file_list;
//     // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
//     GetFileNameListWithoutExtention(input_folder_path,&file_list);
//     GenerateDedupeLSH generate_dedup_lsh;  
//     LSHDeduplicator deduplicator(online_dedup=false,
//                              blacklist_path=this->blacklist_path,
//                              store_blacklist=this->store_blacklist); 
//     // Compare all lines of source_file and target_file
//     for(int i=0;i<(int)file_list.size();i++){
//         //for(int j=i;j<(int)file_list.size();j++){
//             ifstream target_file(input_folder_path+"/"+file_list[i]);
//             ofstream output_file(output_folder_path+"/"+file_list[i]);
//             while (getline(target_file, target_line)){
            
//                 // Read Document from jsonl
//                 // TODO:
//                 Document document;
//                 ReadDocumentFromJsonl(document,target_file)
                
//                 lshs = generate_dedup_lsh.CalculateLSH(target_line);
//                 deduplicator.Apply(lshs);
                
//                 // output document
//                 DumpDocumentToJsonl(document,output_file_path)
//                 //もしgenerateがbucket_size以上になったら、一旦LSHDeduplicatorを削除し、再生成する
//                 if(){
//                     //もしseenがbucket_size以上なら(black_listがbucket_size以上)seenを初期化 
//                     if()
//                 }
                
                
//                 //vector<string> segments;
//                 //SegmentSentence(target_line, segments);

//             }
//             CopyFile(output_folder_path+"/"+file_list[i],input_folder_path+"/"+file_list[i]);
//        //}
//     }

//     end = chrono::system_clock::now(); 
//     double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count(); 
//     //TODO: fix here.
//     Stats stats;
//     stats.elapsed_time=elapsed;
//     stats.file_name="";
//     stats.process_name=__func__;
//     stats.result_file_size=-1;
//     return stats;
// }

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
        &CorpusCleaner::LanguageFilter,
        //&CorpusCleaner::URLRemover ,
        //&CorpusCleaner::EmojiRemover, 
        //&CorpusCleaner::SpecialCharacterRemover, 
        //&CorpusCleaner::LengthFilter, 
        &CorpusCleaner::QuotesRemover, 
        &CorpusCleaner::PerplexityFilter
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
    GetFileNameListWithoutExtention(this->intermediate_path,&filename_list);

    // const string input_file_extension = ".txt";
    // Execute the each CorpusCleaner processing on all files in the intermediate folder.
    for (auto filename: filename_list){
    
        // load data
        ifstream input_file(this->intermediate_path+"/"+filename+".jsonl");
        string  output_file_path(this->output_path+"/"+filename+".jsonl");
        string line="";
        uint64_t line_count=0;
        Document document;
        while (getline(input_file, line)) {
        
            // TODO: ReadDocumentFromJsonl();
            ReadDocumentFromJsonlOneLine(document,line);
            // document.text = line;
            // document.id = filename+to_string((unsigned long long)line_count);

            // Loop processing as many times as cleaner_list
            for (const auto& cleaner : cleaner_list) {     
                //TODO: Exclude strings that cannot be executed
                Stats stats = PipelineStep(document,(cleaner));
                //OutputStats(stats);
                // if rejected, break and turn to next line.
                if(document.is_rejected)    break;
            }
            
            // dump data
            WriteDocumentToJsonl(document,output_file_path);
            line_count++;
        }
        
        input_file.close();
        
    }
    
    // Loop processing as many times as deduplicate_list
    for (const auto& step : deduplicate_list) {
        CopyFolder(this->output_path, this->intermediate_path); 
        Stats stats = (this->*step)(this->intermediate_path,this->output_path);
        OutputStats(stats);
    }
    return 0;
}

