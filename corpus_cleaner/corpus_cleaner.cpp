#include "corpus_cleaner.hpp"
#include "util.hpp"
#include "simdjson.h"
#include <locale>
using namespace simdjson;

/**
 * @brief Loggging Document to output_file_path
 * @details 
 * Example:
 * ```cpp
 * string input_file_path = "./input.jsonl";  
 * ifstream ifs(input_file_path);  
 * Document document;  
 * string line = "";  
 * while(getline(ifs,line)){  
 *     ReadDocumentFromJsonl(document,line);  
 *     // Write process for document.
 * } 
 * ```
 * @param Document document: document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @note 
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

        jsonl_line["noun_ratio"].get(line_view);
        document.noun_ratio=stod(string(line_view));

        jsonl_line["perplexity"].get(line_view);
        document.perplexity=stod(string(line_view));    
    }
    catch(...){ 
        throw;
    }
}   

/**
 * @brief Loggging Document to output_file_path
 * @details 
 * @param Document document: document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @note 
**/
void WriteDocumentToJsonl(Document &document,
                          string output_file_path)
{
    document.text = EscapeWord(document.text);
    ofstream output_file(output_file_path, ios::binary | ios::app);

    try{
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
        output_file << "\"noun_ratio\":\"" <<document.noun_ratio << "\",";
        output_file << "\"perplexity\":\"" <<document.perplexity << "\"";
        output_file <<"}"<< endl;
    }
    catch(...){ 
        output_file.close();
        throw;
    }
    output_file.close();
}   


/**
 * @brief Convert input files to jsonl that has Document's element.
 * @details 
 * @param string sentence: sentence
 * @param string filename: filename without file extention 
 * @param string file_line_count: the line number of sentence in "filename"
 * @param Document document: document converted
 * @return void: None
 * @note 
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
    document.noun_ratio=-1;
    document.perplexity=-1;
}

/**
 * @brief Convert input files to jsonl that has Document's element.
 * @details 
 * @param Document document: document
 * @param string output_file_path: Path of file for statistics.
 * @return void: None
 * @note 
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
 * Example:
 * ```cpp
 * ```
 * @param string process_name: Cleaning filter name.
 * @param string output_path: Path of file for statistics.
 * @param double elapsed_time: elapsed process time.
 * @return Stats: statistics
 * @note 
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
 * Example:
 * @param Stats stats: Statistics to be output.
 * @return None
 * @note 
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
 * @note 
**/
void CorpusCleaner::StoreException(string function_name, string reference)
{
    string filename = this->exception_path+"/exception.txt";
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
                             bool store_rejected,
                             bool sentence_segment,
                             float language_threshold,
                             double perplexity_threshold,
                             GenerateDedupLSH *generate_dedup_lsh,
                             LSHDeduplicator *deduplicator)
{
    this->input_path = input_path;
    this->output_path = output_path+"/cleaned/";
    this->intermediate_path = output_path+"/intermediate/";
    this->rejected_path = output_path+"/rejected/";
    this->exception_path = output_path+"/exception/";

    this->min_length = min_length;
    this->max_length = max_length;
    this->accept_language = accept_language;
    this->store_rejected = store_rejected;
    this->sentence_segment = sentence_segment;
    this->language_threshold = language_threshold;
    this->perplexity_threshold = perplexity_threshold;
    this->generate_dedup_lsh=generate_dedup_lsh;
    this->deduplicator=deduplicator;

    this->jagger_parser.read_model("../../scripts/jagger-extension/jagger-2023-02-18/model/kwdlc/patterns");

    if(filesystem::exists(this->output_path) | 
       filesystem::exists(this->rejected_path)) {
        // cout << "ERROR: output_path or rejected_path folder already exists. ";
        // cout << "Please RENAME to delete the selection." << endl;
        // exit(EXIT_FAILURE);
        // TODO: Fix

        RemoveFolder(this->output_path);
        RemoveFolder(this->rejected_path);

    }

    RemoveFolder(this->intermediate_path);
    RemoveFolder(this->exception_path);

    mkdir(output_path.c_str(), 0777);
    mkdir(this->intermediate_path.c_str(), 0777);
    mkdir(this->output_path.c_str(), 0777);
    mkdir(this->exception_path.c_str(), 0777);
    mkdir(this->rejected_path.c_str(), 0777);

    //Read from input_path's files, and write to output_path in jsonl format.
    // TODO: uncommentout next line
    // ConvertInputFilesToJsonl(this->input_path,this->output_path);
    CopyFolder(this->input_path,this->intermediate_path);

    // set locale for utf-8 string
    //std::locale::global(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
}

/***destructor***/
CorpusCleaner::~CorpusCleaner()
{
    //remove intermediate folder
    // TODO: uncommentout next line
    // RemoveFolder(this->intermediate_path);
}


/***member function***/
/**
 * @brief Remove control characters in text
 * @details 
 * Control characters are characters that is less than 0x1f, \u2028 and \u2029, and 0x7f.
 * 0x0a(LF) and 0x0d(CR) are not deleted by this function.
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @note 
 * @ref 
 * https://learn.microsoft.com/ja-jp/visualstudio/ide/encodings-and-line-breaks?view=vs-2022
 * https://scrapbox.io/bbr-program-memo/U+2028%E3%81%A8U+2029%E3%81%AE%E5%8F%96%E3%82%8A%E6%89%B1%E3%81%84
**/
void CorpusCleaner::ControlCharacterRemover(Document &document)
{  
    // cout << strlen_utf8(document.text) << endl;
    // cout << document.text.size() << endl;

    bool is_converted = false;
    wstring document_text_w_converted = L"";
    wstring document_text_w = ConvertUTF8ToWstring(document.text);

    for(int i=0;i<(int)document_text_w.size();i++){
        if(document_text_w[i] == '\0')    is_converted = true;
        else if(document_text_w[i] == U'\u2028'||document_text_w[i] == U'\u2029') 
        {
            is_converted = true;
            document_text_w_converted += LR"( )";
        }
        else if(document_text_w[i]<=0x1f)
        {
            // TODO: deal with CRLF
            is_converted = true;
            document_text_w_converted += LR"( )";
        }
        else if(document_text_w[i]==0x7f)
        {
            // "Del" chacacter
            is_converted = true;
            document_text_w_converted += LR"( )";
        }
        else    document_text_w_converted += document_text_w[i];
    }
    document.text = ConvertWstringToUTF8(document_text_w_converted);
    // cout << strlen_utf8(document.text) << endl;    // cout << document.text.size() << endl;

    if (is_converted)   document.metadata.insert(__func__);
}

/**
 * @brief Remove too long sentence and too short sentence.
 * @details 
 *  Remove too long sentence that is length is more thanand too short sentence.  
 *  The length of too long sentence is more than "max_length".  
 *  The length of too short sentence is lesser than "min_length".  
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @note 
**/
void CorpusCleaner::LengthFilter(Document &document)
{  
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
 *  Example:
 *  
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @note 
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
 * @brief Language filtering using fastText.
 * @details 
 * ```cpp
 *  string in = "吾輩は猫である。名前はまだ無い。";
 *  FastTextEx language_filter;
 *  pair<float, string> score;
 *  score = language_filter.filter(in);
 *  // score.first ==1.00005, score.second ==__label__ja
 * 
 *  string in2 = "I am a cat. No name yet.";
 *  score = language_filter.filter(in2);
 *  // score.first ==0.75237, score.second ==__label__en 
 * ```
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @ref 
 *  https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja#python-written-by-hideaki-t--overlast
 *  https://fasttext.cc/docs/en/supervised-tutorial.html
 * @note 
**/
void CorpusCleaner::LanguageFilter(Document &document)
{
    vector<pair<float, string>> predictions;
    int32_t k = 1;
    float threshold = 0.0;

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
        if(document.is_rejected)    document.metadata.insert(__func__);
    }
    catch(...){
        cout << "Exception:LanguageFilter" << endl;
        throw;
    }

}


/**
 * @brief Remove URLs matching regular expression.
 * @details 
 *  Remove URLs matching regular expression.  
 *  The regular expression is "(https?|ftp)(:\/\/[-_\.!~*\'()a-zA-Z0-9;\/?:\@&=\+\$,%#]+)".  
 * @param Document &document: single line text to clean be cleaned
 * @return void: None
 * @note 
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
 * 
 * Example:
 * TODO.
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @note 
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
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @note 
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

    if(sentence!=document.text) document.metadata.insert(__func__);
    document.text = sentence;
}

/**
 * @brief Remove sentence without punctuation.
 * @details 
 * Remove sentence without punctuation that is "、","､","。","｡","．",".","？","?","！","!".  
 * 
 * Example:
 * 
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @note 
 * This filter is heuristic.  
 * For example, a sentence that "https://github.com/" is not removed because it includes '.'.
**/
void CorpusCleaner::ZeroPunctuationFilter(Document &document)
{
    vector<string> punctures = {"、","､","。","｡","．",".","？","?","！","!"};
    string sentence = document.text;

    document.is_rejected = true;
    for(auto puncture: punctures){
        // If there is no puncture in sentence, the return of .find() is string::npos 
        if(document.text.find(puncture)!=string::npos){
            document.is_rejected = false;
            break;
        }
    }

    if(document.is_rejected)    document.metadata.insert(__func__);
}


/**
 * @brief Remove documents with more than 80% nouns by Jagger's morphological analysis.
 * @details 
 *  Follow the steps.  
 *  1. Get the morpheme list of document.text using Jagger::DivideMorpheme.  
 *  2. Count noun of morpheme.
 *  3. Calcurate noun ratio of morpheme.
 *  4. If noun ratio is more than 0.8, the document text is rejected.  
 * 
 * Example:
 * 
 * @param Document &document: single line text to be cleaned
 * @return void: None
 * @note 
**/
void CorpusCleaner::NounRatioFilter(Document &document)
{
    string sentence = document.text;

    try{
        //Get the morpheme list of document.text using Jagger::DivideMorpheme.  
        vector<string> morpheme, morpheme_form;
        this->jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);

        //Count noun of morpheme.
        uint32_t noun_count = 0;
        for(auto part: morpheme_form){
            if(part.substr(0,6)=="名詞")    noun_count++;
            else if(part.substr(0,16)=="接頭辞,名詞")   noun_count++;  //"約"
            else if(part.substr(0,16)=="接尾辞,名詞")   noun_count++;  //"年","月"
            // else if(part.substr(0,22)=="接頭辞,ナ形容詞")   noun_count++;  //"超"
        }

        //Calcurate noun ratio of morpheme.
        document.noun_ratio = (double)noun_count/(double)morpheme_form.size();
        // cout << "noun_ratio: "<< document.noun_ratio << endl;

        //If noun ratio is more than 0.8, the document text is rejected.  
        document.is_rejected = false;
        if(document.noun_ratio >=0.8) document.is_rejected = true;

        if(document.is_rejected)    document.metadata.insert(__func__);
    }
    catch(...){
        cout << "Exception:NounRatioFilter" << endl;
        throw;
    }
}

/**
 * @brief Neologd Normalize sentence
 * @details 
 *  Please Refer document of "NormalizeNeologd()".  
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref 
 *  https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja#python-written-by-hideaki-t--overlast
 * @note 
**/
void CorpusCleaner::Normalizer(Document &document)
{
    try{
        // If the sentence is too long, regular expression errors may occur, so take measures.
        this->LengthFilter(document);
        if(document.is_rejected==true) return;

        string sentence = string_normalizer.NormalizeNeologd(document.text);
        if(sentence!=document.text) document.metadata.insert(__func__);
        document.text = sentence; 
    }
    catch(...){
        cout << "Exception:Normalizer" << endl;
        throw;
    }
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
 *  Example:
 * 
 * @param string input_folder_path: input folder path
 * @param string output_folder_path: output folder path
 * @return Stats: statics imformation of this function.
 * @ref 
 * @note TODO: fix return stats.
**/
void CorpusCleaner::MinhashDeduplication(Document &document)
{
    // Read Document from jsonl
    vector<string> lshs = this->generate_dedup_lsh->CalculateLSH(ConvertUTF8ToWstring(document.text));
    try{
        if(this->deduplicator->Apply(&lshs)){
            document.is_rejected = true;
            document.metadata.insert(__func__);
        }

        //If seen is greater than or equal to bucket_size, clear seen to 0
        if(this->deduplicator->SizeOfSeen()>=this->deduplicator->GetTotalBucketSize()){
            cout << "MinhashDeduplicator: The size of Seen is more than total_bucket_size." << endl;
            cout << "Now, clear seen and blacklist." << endl;
            this->deduplicator->InitializeSeen();
        }

    }
    catch(...){
        cout << "Exception:MinhashDeduplication" << endl;
        throw;
    }
}

/**
 * @brief Simple sentence splitter for japanese text.
 * @details 
 * I used Pragmatic Segmenter's Japanese rules as a reference for sentence separation rules.  
 * The C++ regex library does not support 4-byte characters.  
 * Therefore, characters like 🌀 cannot be matched using regular expressions.  
 * So, in a full search, those that completely match the pictogram are searched and removed.  
 * 
 * Example: TODO
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @return Stats: statics imformation of this function.
 * @ref 
 *  https://github.com/wwwcojp/ja_sentence_segmenter/blob/main/ja_sentence_segmenter/split/simple_splitter.py
 *  https://github.com/diasks2/pragmatic_segmenter#golden-rules-japanese
 * @note 
**/
void CorpusCleaner::SentenceSegmenter(string input_folder_path, string output_folder_path)
{
    string target_line="",source_line="";
    vector<string> file_list;
    // Get the list of files in this->intermediate_folder and set it to vector<string> file_list
    GetFileNameListWithoutExtention(input_folder_path,&file_list);
    // Compare all lines of source_file and target_file
    for(int i=0;i<(int)file_list.size();i++){
        ifstream target_file(input_folder_path+"/"+file_list[i]+".txt");
        string output_file_path(output_folder_path+"/"+file_list[i]+".jsonl");
        int64_t line_count =-1;
        while (getline(target_file, target_line)) {
            vector<string> segments;
            Document document;
            line_count++;
            try{ConvertTextToDocument(target_line,file_list[i],to_string(line_count),document);}
            catch(...){
                string exception_detail = "line: "+target_line;
                cout << "Exeption(ConvertTextToDocument): "<< exception_detail << endl;
                StoreException("ConvertTextToDocument","input{"+exception_detail+"}");
                continue;
            }

            try{SegmentSentence(document.text, segments);}
            catch(...){
                StoreException(__func__, "target_line:{"+target_line+"}");
                continue;
            }
            
            try{    
                uint64_t sentence_count=0;
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
        target_file.close();
    }
}

/**
 * @brief PipelineStep
 * @details 
 * @param Document &document: document is to be filtered
 * @param void (CorpusCleaner::*cleaner)(Document &): filter function list
 * @return Stats: statics imformation of this function.
 * @note TODO: Implement following contents.
 * 1. add cleaner_list's number to getting parameter.
 * 2. 
 * 3. At the end of corpus_cleaner, output the stats to file.
 * @ref 
**/
Stats CorpusCleaner::PipelineStep(Document &document, void (CorpusCleaner::*cleaner)(Document &))
{
    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now();

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
 * @brief Pipeline that sequentially executes the configured CorpusCleaner methods
 * @details 
 *  Perform the following steps in order.  
 *  1. Set CorpusCleaner process to pipeline_list that will be executed. (Please read attention.)  
 *  2. Loop processing as many times as pipeline_list  
 *      2-1. copy output folder to intermediate folder  
 *      2-2. Get list of files in intermediate folder.  
 *      2-3. Execute the each CorpusCleaner processing on all files in the intermediate folder.  
 *
 * Example:
 *  ```cpp
 *   string input_folder_path = "../../results/dataset/input/";
 *   string output_folder_path = "../../results/dataset/output/";
 *   uint32_t min_length= 5;
 *   uint32_t max_length = 5000;
 *   set<string> accept_language{"__label__ja"};
 *   bool store_rejected = true; 
 *   bool execute_sentence_segment = false; // TODO: switch true
 *   double language_threshold = 0.3;
 *   double perplexity_threshold = 40000;
 *
 *   string blacklist_file_path = output_folder_path+"/blacklist.txt";
 *   GenerateDedupLSH generate_dedup_lsh(4,200,20,10);
 *   LSHDeduplicator deduplicator(true,blacklist_file_path,true,1280000000);
 *
 *   // create instance
 *   CorpusCleaner corpus_cleaner(input_folder_path,
 *                                output_folder_path,
 *                                min_length,
 *                                max_length,
 *                                accept_language,
 *                                store_rejected,
 *                                execute_sentence_segment,
 *                                language_threshold,
 *                                perplexity_threshold,
 *                                &generate_dedup_lsh,
 *                                &deduplicator);
 *   
 *   // Execute cleaning pipeline
 *   corpus_cleaner.CleanPipeline(); 
 * ```
 * @param void: None
 * @return None
 * @attention 
 *  CorpusCleaner processing is performed in the order set in Cleaner_array.  
 *  For example, set cleaner_array as follows: 
 * ```cpp
 *      vector<void (CorpusCleaner::*)(Document &)> cleaner_list = { 
 *          &CorpusCleaner::URLRemover ,  
 *          &CorpusCleaner::LengthFilter,  
 *          &CorpusCleaner::SpecialCharacterRemover
 *      };  
 * ```
 *  At this time, processing is performed in the order of  
 *  1. URLRemover, 2. LengthFilter, and 3. SpecialCharacterRemover.  
**/
int32_t CorpusCleaner::CleanPipeline(void)
{
    // Set CorpusCleaner process that will be executed.
    // They will be executed in the order you set them.
    vector<void (CorpusCleaner::*)(Document &)> cleaner_list = { 
        &CorpusCleaner::ControlCharacterRemover,
        &CorpusCleaner::Normalizer,
        &CorpusCleaner::URLRemover,
        &CorpusCleaner::EmojiRemover, 
        &CorpusCleaner::SpecialCharacterRemover, 
        &CorpusCleaner::QuotesRemover, 
        &CorpusCleaner::LengthFilter, 
        &CorpusCleaner::ZeroPunctuationFilter,
        &CorpusCleaner::LanguageFilter,
        &CorpusCleaner::NounRatioFilter,
        // &CorpusCleaner::MinhashDeduplication,
        &CorpusCleaner::PerplexityFilter,
    }; 
    
    cout << "### Start Clean Pipeline. ###" << endl;
    if(this->sentence_segment==true){
        cout << "### Execute Sentence Segmenter. ###" << endl;
        // Loop processing as many times as deduplicate_list
        // MoveFolder(this->output_path, this->intermediate_path); 
        SentenceSegmenter(this->intermediate_path,this->output_path);
        RemoveFolder(this->intermediate_path);
        // OutputStats(stats);
        // copy output folder to intermediate folder
        MoveFolder(this->output_path, this->intermediate_path); 
    }
    
    vector<string> filename_list;
    vector<uint64_t> file_line_number_list;
    
    // Get list of files in intermediate folder
    GetFileNameListWithoutExtention(this->intermediate_path,&filename_list);
    string extention = (this->sentence_segment==true) ? ".jsonl":".txt";
    GetFileLineNumberList(this->intermediate_path,&filename_list,extention,&file_line_number_list);

    cout << "### Excecute CleanPipeline. ###" << endl;
    // Execute the each CorpusCleaner processing on all files in the intermediate folder. 
    for (int i=0;i<(int)filename_list.size();i++){
        // load data
        string filename = filename_list[i];
        uint64_t file_line_number = file_line_number_list[i];
        cout << "Start Cleaning "+this->intermediate_path+filename+extention << endl;

        // ifstream input_file(this->intermediate_path+filename+extention,ios::binary);
        ifstream input_file(this->intermediate_path+filename+extention);
        string  output_file_path(this->output_path+filename+".jsonl"); 
        string  rejected_file_path(this->rejected_path+filename+".jsonl"); 
        string line = "";
        uint64_t line_count = 0; // The fist incrementation is overflow.
        uint64_t removed_line_count = 0;
        chrono::system_clock::time_point start, end;
        start = chrono::system_clock::now();
    
        while (!input_file.eof()) {
            line_count++;   

            try {getline(input_file, line);} 
            catch (const std::ios_base::failure& e) {
                std::cerr << "getline error: " << e.what() << std::endl;
                // input_file.clear();// clear the stream state
                input_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignore current line
                continue;
            }
            Document document;
            // load data
            end = std::chrono::system_clock::now();
            uint32_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            ProceedProgressBar(line_count,file_line_number,elapsed_time);
            if(this->sentence_segment==true){
                try{ReadDocumentFromJsonlOneLine(document,line);}
                catch(...){
                    string exception_detail = "line: "+line;
                    cout << "Exeption(ReadDocumentFromJsonlOneLine): "<< exception_detail << endl;
                    StoreException("ReadDocumentFromJsonlOneLine","input_jsonl_line{"+exception_detail+"}");
                    continue;
                }
            }
            else{
                try{ConvertTextToDocument(line,filename,to_string(line_count),document);}
                catch(...){
                    string exception_detail = "line: "+line;
                    cout << "Exeption(ConvertTextToDocument): "<< exception_detail << endl;
                    StoreException("ConvertTextToDocument","input{"+exception_detail+"}");
                    continue;
                }
            }

            // Loop processing as many times as cleaner_list
            for (const auto& cleaner : cleaner_list) {     
                try{
                    Stats stats = PipelineStep(document,(cleaner));
                    // TODO: store stats
                    // store number of execute function count
                }
                catch(...){continue;}
                // OutputStats(stats);
                // if rejected, break and turn to next line.
                if(document.is_rejected){
                    removed_line_count++;
                    break;
                }
            }

            // dump data
            try{
                if(document.is_rejected){
                    if(this->store_rejected) WriteDocumentToJsonl(document,rejected_file_path);
                }
                else    WriteDocumentToJsonl(document,output_file_path);
            }
            catch(...){
                string exception_detail = "document.text: "+document.text+" document.id: "+document.id;
                cout << "Exeption(WriteDocumentToJsonl): "<< exception_detail << endl;
                StoreException("WriteDocumentToJsonl","input_jsonl_line{"+exception_detail+"}");
                continue;
            }
        }

        // output removed results
        printf("Removed line number: %ld\n",removed_line_count);
        printf("Remaining line number: %ld\n",file_line_number - removed_line_count);
        printf("Removed ratio: %.2f%%\n",double(removed_line_count) / file_line_number * 100);
        printf("Remaining ratio: %.2f%%\n",100 - double(removed_line_count) / file_line_number * 100);
        input_file.close();   

        // TODO: calculate each filter's process time by (process_time/execute_count)
        // TODO: output stats to file
    }
    
    return 0;
}

