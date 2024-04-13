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
    try{
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
    catch(...){ 
        cout << "Exeption("<<__func__<<"): input_jsonl_line "<<input_jsonl_line<< endl;
        StoreException(__func__,"input_jsonl_line{"+input_jsonl_line+"}");
        throw;
    }
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
    try{
        document.text = EscapeWord(document.text);
  
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
    catch(...){ 
        string exception_detail = "document.text: "+document.text+" document.id: "+document.id;
        cout << "Exeption("<<__func__<<"): "<< exception_detail << endl;
        StoreException(__func__,"input_jsonl_line{"+exception_detail+"}");
        throw;
    }
}   


/**
 * @brief Convert input files to jsonl that has Document's element.
 * @details 
 * @param string sentence: sentence
 * @param string filename: filename without file extention 
 * @param string file_line_count: the line number of sentence in "filename"
 * @param Document document: document converted
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
    cout << "### Convert input file(.txt) to .jsonl. ###" << endl;

    vector<string> filename_list;
    vector<uint64_t> file_line_number_list;

    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileNameListWithoutExtention(input_folder_path,&filename_list);
    GetFileLineNumberList(input_folder_path,&filename_list,".txt",&file_line_number_list);
    
    string line;
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)filename_list.size();i++){
        string filename = filename_list[i];
        uint64_t file_line_number = file_line_number_list[i];
        ifstream input_file(input_folder_path+"/"+filename+".txt"); 
        string output_file_path(output_folder_path+"/"+filename+".jsonl");
        uint64_t line_count = 0;
        chrono::system_clock::time_point start, end;
        start = chrono::system_clock::now();

        while(getline(input_file, line)){
            Document document; 
            end = std::chrono::system_clock::now();
            uint32_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            ProceedProgressBar(line_count+1,file_line_number,elapsed_time);
            ConvertTextToDocument(line,filename,to_string(line_count),document);
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

    // int index_filename = output_path.find_last_of("/")+1;
    stats.process_name=process_name;
    // stats.file_name=output_path.substr(index_filename);
    stats.file_name="";
    stats.elapsed_time=elapsed_time;
    // stats.result_file_size=filesystem::file_size(output_path);
    stats.result_file_size=0;
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


/**
 * @brief Save exception in file 
 * @details 
 * @param string reference: reference infomation. For example, sentence.
 * @param string function_name: function name cause exeption
 * @return None
 * @attention 
**/
void StoreException(string function_name, string reference)
{
    string filename = "../../results/dataset/exception/exception.txt";
    ofstream output_file(filename, ios::app);
    
    output_file << "Function Name: "<< function_name << " , ";
    output_file << "Reference:" << reference << " , ";
    output_file << endl;
    output_file.close();
}

/***constructor***/
CorpusCleaner::CorpusCleaner(string input_path,
                             string output_path,
                             uint32_t min_length,
                             uint32_t max_length,
                             set<string> accept_language,
                             bool sentence_segment,
                             float language_threshold,
                             double perplexity_threshold,
                             GenerateDedupLSH *generate_dedup_lsh,
                             LSHDeduplicator *deduplicator)
{
    this->input_path = input_path;
    this->output_path = output_path;
    this->intermediate_path = "../../results/dataset/intermediate/";
    this->min_length = min_length;
    this->max_length = max_length;
    this->accept_language = accept_language;
    this->sentence_segment = sentence_segment;
    this->language_threshold = language_threshold;
    this->perplexity_threshold = perplexity_threshold;
    // this->blacklist_path =  "../../results/others/blacklist.jsonl";
    // this->store_blacklist = true;
    this->generate_dedup_lsh=generate_dedup_lsh;
    this->deduplicator=deduplicator;


    // TODO: remove intermediate,output,exception

    mkdir(this->intermediate_path.c_str(), 0777);
    mkdir(this->output_path.c_str(), 0777);
    mkdir("../../results/dataset/exception/", 0777);
    // TODO: mkdir black list path 

    //Read from input_path's files, and write to output_path in jsonl format.
    // TODO: uncommentout next line
    ConvertInputFilesToJsonl(this->input_path,this->output_path);

}
/***deconstructor***/
CorpusCleaner::~CorpusCleaner()
{
    //remove intermediate folder
    // TODO: uncommentout next line
    // RemoveFolder(this->intermediate_path);
}
/***member function***/


/**
 * @brief Remove too long sentence and too short sentence.
 * @details 
 *  Remove too long sentence that is length is more thanand too short sentence.
 *  The length of too long sentence is more than "max_length".
 *  The length of too short sentence is lesser than "min_length".
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @attention 
**/
void CorpusCleaner::LengthFilter(Document &document)
{  
    // cout << __func__ << endl;

    uint32_t line_length = strlen_utf8(document.text);
    if (line_length < this->min_length || this->max_length < line_length) {     
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
    // cout << __func__ << endl;

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
    // cout << __func__ << endl;

    try{
        this->language_filter.predictOneLine(document.text, predictions, k, threshold);
        //return pair<float, string> : float:　Language assessment score, string: Language determination results
        pair<float,string> result;
        if((int)predictions.size()==0)   result=make_pair(0.0,"other"); //Remove. This case often applies to lines containing only spaces.
        else    result = make_pair((float)predictions[0].first,predictions[0].second);

        document.language = result.second;
        document.language_score = result.first;

        document.is_rejected=true;
        if(accept_language.find(document.language)!=accept_language.end()){
            // If fasttext's score is less than threshold, the text to be rejected.
            if(document.language_score>=this->language_threshold){
                document.is_rejected=false;
            }
        }
    }
    catch(...){
        cout << "Exception:LanguageFilter" << endl;
        throw;
    }
    // cout << __func__ << " completed."<< endl;

    if(document.is_rejected)    document.metadata.insert(__func__);
}


/**
 * @brief Remove URLs matching regular expression.
 * @details 
 *  Remove URLs matching regular expression.
 *  The regular expression is "(https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @attention 
**/
void CorpusCleaner::URLRemover(Document &document)
{
    static regex url_pattern(R"((https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+))");
    string sentence =  regex_replace(document.text,url_pattern,"");
    // cout << __func__ << endl;

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
 * 
 * Example:
 * TODO.
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

    // cout << __func__ << endl;

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

    // cout << __func__ << endl;

    for(int i=0;i<1792;i++){
        ReplaceSubstring(sentence,emoji,"");
        emoji = CalculateNextEmoji(emoji);
    }
    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}

/**
 * @brief Remove quotes. For example, [1], {245}, and so on.
 * @details 
 *  Remove remarks matching regular expression.
 *  The regular expression is "(\[([0-9]+)\]|\{([0-9]+)\})".
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @attention Don't use this on corpus that contain formulas or programs.
**/
void CorpusCleaner::QuotesRemover(Document &document)
{
    static regex remaks_pattern(R"((\[([0-9]+)\]|\{([0-9]+)\}))");
    string sentence =  regex_replace(document.text,remaks_pattern,"");

    // cout << __func__ << endl;

    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}


/**
 * @brief Neologd Normalize sentence
 * @details 
 *  Please Refer document of "NormalizeNeologd()"
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
    
    // cout << __func__ << endl;

    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence; 
    // cout << __func__ <<" completed"<< endl;

}


/**
 * @brief MinHashLSH Deduplication files in the this->intermediate folder
 * @details 
 *  Follow the steps below to remove duplication between all lines of all files in the this->intermediate folder.
 *  1. Get the list of files in this->intermediate_folder and set it to vector<string> file_list
 *  2. Compare all lines of source_file and target_file in file_list.
 *  3. Check duplication between all lines of souce file and all lines of target_file.
 *  Therefore, characters like 🌀 cannot be matched using regular expressions. 
 *  I considered deduplication using set or multiset, 
 *  but I did not use this method because the file size could exceed the memory capacity.
 * 
 * The usage is following.
 * 
 * @param string input_folder_path: input folder path
 * @param string output_folder_path: output folder path
 * @return Stats: statics imformation of this function.
 * @ref 
 * @attention TODO: fix return stats.
**/
void CorpusCleaner::MinhashDeduplication(Document &document)
{
    // Read Document from jsonl
    vector<string> lshs = this->generate_dedup_lsh->CalculateLSH(ConvertUTF8ToWstring(document.text));
    
    // cout << __func__ << endl;

    try{
        // cout <<1<<endl;
        if(this->deduplicator->Apply(&lshs)){
            document.is_rejected = true;
            document.metadata.insert(__func__);
        }
        // cout <<2<<endl;

        //If seen is greater than or equal to bucket_size, clear seen to 0
        if(this->deduplicator->SizeOfSeen()>=this->deduplicator->GetTotalBucketSize()){
            // cout <<4<<endl;
            this->deduplicator->InitializeSeen();
        }
        // cout <<3<<endl;

    }
    catch(...){
        cout << "Exception:MinhashDeduplication" << endl;
        throw;
    }
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
void CorpusCleaner::SentenceSegmenter(string input_folder_path, string output_folder_path)
{
    string target_line="",source_line="";
    vector<string> file_list;
    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileNameListWithoutExtention(input_folder_path,&file_list);
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        ifstream target_file(input_folder_path+"/"+file_list[i]+".jsonl");
        string output_file_path(output_folder_path+"/"+file_list[i]+".jsonl");
        while (getline(target_file, target_line)) {
            vector<string> segments;
            Document document;
            try{ReadDocumentFromJsonlOneLine(document,target_line);}
            catch(...){continue;}
            
            try{SegmentSentence(document.text, segments);}
            catch(...){
                StoreException(__func__, "target_line:{"+target_line+"}");
                continue;
            }
            
            try{    
                uint64_t sentence_count=0;
                // for(auto segment:segments)  cout << segment <<" , ";
                if((int64_t)segments.size()!=1){
                    for(auto sentence:segments){
                        Document document_segmented = document;
                        document_segmented.text = sentence;
                        document_segmented.id = document.id+"_"+to_string(sentence_count);
                        document_segmented.metadata.insert(__func__);
                        WriteDocumentToJsonl(document_segmented,output_file_path);
                    }
                }
                else    WriteDocumentToJsonl(document,output_file_path);
            }
            catch(...){continue;}
        }
    }
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
 * @attention This filter's speed is too late!!! Don't use!
**/
void CorpusCleaner::ExactDeduplication(string input_folder_path, string output_folder_path)
{
    string target_line="",source_line="";
    vector<string> file_list;
    bool duplication=false;
    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileNameListWithoutExtention(input_folder_path,&file_list);
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        ifstream target_file(input_folder_path+"/"+file_list[i]+".jsonl");
        string  output_file_path(this->output_path+"/"+file_list[i]+".jsonl");

        uint32_t target_counter=0,source_counter=0;
        while (getline(target_file, target_line)) {
            duplication=false;
            source_counter=0;
            Document target_document;
            target_counter++;
            ReadDocumentFromJsonlOneLine(target_document,target_line);
            for(int j=i;j<(int)file_list.size();j++){
                ifstream source_file(input_folder_path+"/"+file_list[j]+".jsonl"); 
                while(getline(source_file,source_line)){
                    try{
                        Document source_document;
                        source_counter++;
                        ReadDocumentFromJsonlOneLine(source_document,source_line);
                        // cout << "target:"<<file_list[i]+".jsonl line:"<<target_counter<<" text"<<target_document.text<<" ";
                        // cout << "source:"<<file_list[j]+".jsonl line:"<<source_counter<<" text"<<source_document.text<<endl;
                        if(input_folder_path+"/"+file_list[i]+".jsonl"==input_folder_path+"/"+file_list[j]+".jsonl"&&target_counter>=source_counter)continue;

                        // check duplication
                        if(target_document.is_rejected||source_document.is_rejected) continue;
                        // deduplicate
                        if(target_document.text==source_document.text){
                            duplication=true;
                            // cout << "Deduplicated." << endl;
                            target_document.is_rejected=true;
                            // cout <<target_document.is_rejected<<endl;
                            target_document.metadata.insert(__func__);
                            break;
                        }
                    }
                    catch(...){
                        cout << "exception" << endl;
                        source_counter++;
                        StoreException(__func__, 
                                      "source_file{"+input_folder_path+"/"+file_list[j]+".jsonl"+"},source_counter{"+to_string(source_counter)+"}");
                    }
                }
                if(duplication)break;
            }
            WriteDocumentToJsonl(target_document,output_file_path);
            // cout << "WriteDocumentToJsonl" << endl;
            // CopyFile(output_folder_path+"/"+file_list[i]+".jsonl",input_folder_path+"/"+file_list[i]+".jsonl");
        }
    }
}


/**
 * @brief PipelineStep
 * @details 
 * @param Document &document: document is to be filtered
 * @param void (CorpusCleaner::*cleaner)(Document &): filter function list
 * @return Stats: statics imformation of this function.
 * @ref 
**/
Stats CorpusCleaner::PipelineStep(Document &document, void (CorpusCleaner::*cleaner)(Document &))
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now();

    // cout << "PiepilieStep function" << endl;
    // Execute filtering function
    try{ (this->*cleaner)(document); }
    catch(...){
        cout << "Exeption(PipelineStep): "<<document.id <<" "<<document.text << endl;
        StoreException(__func__, "document.text{"+document.text+"}");
        throw;
        return MakeStats(__func__,"",0);
    }
    
    end = chrono::system_clock::now(); 
    double elapsed = chrono::duration_cast<chrono::duration<double>>(end - start).count(); 
    // TODO: fix second parameters
    return MakeStats(__func__,"",elapsed);
}

/**
 * @brief PipelineStep for all files in folder
 * @details 
 * @param Document &document: document is to be filtered
 * @param void (CorpusCleaner::*cleaner)(Document &): filter function list
 * @return Stats: statics imformation of this function.
 * @ref 
**/
Stats CorpusCleaner::PipelineStepForFoldersAll(string input_folder_path, 
                                               string output_folder_path,
                                               void (CorpusCleaner::*cleaner)(string ,string ))
{
   chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now(); 

    try{ (this->*cleaner)(input_folder_path, output_folder_path); }
    catch(...){
        //cout << "Exeption(PipelineStepForFoldersAll): "<<document.id <<" "<<document.text << endl;
        //StoreException(__func__, "document.text{"+document.text+"}");
        throw;
        return MakeStats(__func__,"",0);
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
int32_t CorpusCleaner::CleanPipeline(void)
{
    // Set CorpusCleaner process that will be executed.
    // They will be executed in the order you set them.
    vector<void (CorpusCleaner::*)(Document &)> cleaner_list = { 
        &CorpusCleaner::Normalizer,
        &CorpusCleaner::LengthFilter, 
        &CorpusCleaner::LanguageFilter,
        &CorpusCleaner::URLRemover ,
        &CorpusCleaner::EmojiRemover, 
        &CorpusCleaner::SpecialCharacterRemover, 
        &CorpusCleaner::QuotesRemover, 
        &CorpusCleaner::MinhashDeduplication,
        &CorpusCleaner::PerplexityFilter,
    }; 
    vector<void (CorpusCleaner::*)(string,string)> deduplicate_list = { 
        // &CorpusCleaner::ExactDeduplication, 
    }; 
    
    cout << "### Start Clean Pipeline. ###" << endl;

    if(this->sentence_segment==true){
        cout << "### Execute Sentence Segmenter. ###" << endl;
        // Loop processing as many times as deduplicate_list
        MoveFolder(this->output_path, this->intermediate_path); 
        SentenceSegmenter(this->intermediate_path,this->output_path);
        // OutputStats(stats);
    }
    
    vector<string> filename_list;
    vector<uint64_t> file_line_number_list;
    // copy output folder to intermediate folder
    MoveFolder(this->output_path, this->intermediate_path); 
    // Get list of files in intermediate folder
    GetFileNameListWithoutExtention(this->intermediate_path,&filename_list);
    GetFileLineNumberList(this->intermediate_path,&filename_list,".jsonl",&file_line_number_list);

    cout << "### Excecute CleanPipeline. ###" << endl;
    // Execute the each CorpusCleaner processing on all files in the intermediate folder. 
    for (int i=0;i<(int)filename_list.size();i++){
        // load data
        string filename = filename_list[i];
        uint64_t file_line_number = file_line_number_list[i];
        cout << "Start Cleaning "+this->intermediate_path+filename+".jsonl" << endl;

        ifstream input_file(this->intermediate_path+filename+".jsonl");
        string  output_file_path(this->output_path+filename+".jsonl"); 
        string line="";
        uint64_t line_count=-1; // The fist incrementation is overflow.
        chrono::system_clock::time_point start, end;
        start = chrono::system_clock::now();
    
        while (getline(input_file, line)) {
            Document document;
            line_count++;   
            // load data
            // cout << "read document" << endl;
            end = std::chrono::system_clock::now();
            uint32_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            ProceedProgressBar(line_count+1,file_line_number,elapsed_time);
            try{ReadDocumentFromJsonlOneLine(document,line);}
            catch(...){continue;}

            // cout << "pipeline step" << endl;
            // Loop processing as many times as cleaner_list
            for (const auto& cleaner : cleaner_list) {     
                //TODO: Exclude strings that cannot be executed
                try{Stats stats = PipelineStep(document,(cleaner));}
                catch(...){continue;}
                //OutputStats(stats);
                // if rejected, break and turn to next line.
                if(document.is_rejected)    break;
            }
            
            // dump data
            try{WriteDocumentToJsonl(document,output_file_path);}
            catch(...){continue;}
        }
        cout << endl;
        input_file.close();   
    }
    
    cout << "### Execute ExactDeduplication. ###" << endl;
    // Loop processing as many times as deduplicate_list
    for (const auto& step : deduplicate_list) {
        MoveFolder(this->output_path, this->intermediate_path); 
        Stats stats = PipelineStepForFoldersAll(this->intermediate_path,
                                                this->output_path,
                                                step);  
        OutputStats(stats);
    }

    return 0;
}

