#include "util.hpp"
using namespace std;

/**
 * @brief Get exact length of UTF-8 string in C
 * @details 
 * @example
 *   string input="こんにちわ。";
 *   uint32_t length = strlen_utf8(input);
 * @param string input: text string.
 * @return uint32_t: length of input
 * @ref https://ja.stackoverflow.com/questions/2988/c%E8%A8%80%E8%AA%9E%E3%81%A7%E3%81%AEutf-8%E6%96%87%E5%AD%97%E5%88%97%E3%81%AE%E6%AD%A3%E7%A2%BA%E3%81%AA%E9%95%B7%E3%81%95%E3%82%92%E5%8F%96%E5%BE%97%E3%81%99%E3%82%8B
 * @attention 
**/
uint32_t strlen_utf8(string input)
{
    uint32_t i = 0, length = 0;
    while (input[i])
    {
        if ((input[i] & 0xc0) != 0x80) length++;
        i++; 
    }
    return length;
}

/**
 * @brief Delete a folder with its contents
 * @details 
 *  Recursively delete files in a folder.
 * @example
 *   string folder_path="data/intermediate";
 *   RemoveFolder(folder_path);
 * @param string path: folder path to be deleted
 * @return None
 * @attention 
**/
void RemoveFolder(const std::string& path) 
{
    filesystem::path source_path = path;
    // Check the file that is existed.
    if (!filesystem::exists(source_path)) {
        // cerr << "Source directory does not exist: " << source_path << endl;
        return;
    }
    for (const auto& entry : filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            RemoveFolder(entry.path().string());
        } else {
            filesystem::remove(entry.path());
        }
    }
    filesystem::remove(path);
}

/**
 * @brief copy source_fileto target_file
 * @details 
 * @example
 *   string source_path="data/source/a.txt";
 *   string target_path="data/target/a.txt";
 *   CopyFolder(source_path,target_path);
 * @param string source_path: Copy source file path
 * @param string target_path: Copy target file path
 * @return None
 * @attention 
**/
void CopyFile(string source_path, string target_path) 
{
    filesystem::path source = source_path;
    filesystem::path destination = target_path;
    filesystem::copy(source, destination, filesystem::copy_options::overwrite_existing);
}

/**
 * @brief copy source_fileto target_file
 * @details 
 * @example
 *   string source_path="data/source/a.txt";
 *   string target_path="data/target/a.txt";
 *   CopyFolder(source_path,target_path);
 * @param string source_path: Copy source file path
 * @param string target_path: Copy target file path
 * @return None
 * @attention 
**/
void MoveFile(string source_path, string target_folder) 
{
    filesystem::path source = source_path;
    filesystem::path destination = target_folder;
    filesystem::path new_path = destination / source.filename();
    filesystem::rename(source, new_path);
}

/**
 * @brief copy source_folder to target_folder
 * @details 
 * Example:
 *   string source_folder="data/source";
 *   string target_folder="data/target";
 *   CopyFolder(source_folder,target_folder);
 * @param string source_folder: Copy source folder path
 * @param string target_folder: Copy target folder path
 * @return None
 * @attention 
**/
void CopyFolder(string source_folder, string target_folder) 
{
    // copy source_folder to target_folder
    filesystem::path sourceDir = source_folder;
    filesystem::path destinationDir = target_folder;
    for (const auto &file : filesystem::directory_iterator(sourceDir)) {
        if (filesystem::is_regular_file(file.path())) {
            filesystem::copy(file, destinationDir / file.path().filename(), filesystem::copy_options::overwrite_existing);
        }
    }
}

/**
 * @brief copy source_folder to target_folder
 * @details 
 * Example:
 *   string source_folder="data/source";
 *   string target_folder="data/target";
 *   MoveFolder(source_folder,target_folder);
 * @param string source_folder: Copy source folder path
 * @param string target_folder: Copy target folder path
 * @return None
 * @attention 
**/
void MoveFolder(string source_folder, string target_folder) 
{
    // Check the file that is existed.
    if (!filesystem::exists(source_folder)) {
        std::cerr << "Source directory does not exist: " << source_folder << std::endl;
        return;
    }
    if (!filesystem::exists(target_folder)) filesystem::create_directory(target_folder);

    // Move file and folder
    for (const auto& entry : filesystem::directory_iterator(source_folder)) {
        try {
            filesystem::path newPath = target_folder / entry.path().filename();
            filesystem::rename(entry.path(), newPath);
            std::cout << "Moved: " << entry.path().filename() << std::endl;
        } catch (const filesystem::filesystem_error& e) {
            std::cerr << "Error moving " << entry.path() << ": " << e.what() << std::endl;
        }
    }
}


/**
 * @brief Derive the next emoji
 * @details 
 * @example
 *   string emoji="🌀";
 *   CalculateNextEmoji(emoji); //return:🌁
 * @param string pre_emoji: emoji string
 * @return string: next emoji (pre_emoji + 1)
 * @ref https://guppy.eng.kagawa-u.ac.jp/OpenCampus/unicode.html
 * @note 
**/
string CalculateNextEmoji(string pre_emoji)
{
    string emoji=pre_emoji;
    //If pre_emoji[0]='\360', pre_emoji has 4 elements 
    //and is determined by pre_emoji[1]='\237'
    if(pre_emoji[0]=='\360'&&pre_emoji.size()!=4) return "";
    //If pre_emoji[0]='\342' then pre_emoji is determined by three elements
    if(pre_emoji[0]=='\342'&&pre_emoji.size()!=3) return "";

    emoji[emoji.size()-1]++;
    if(int(emoji[emoji.size()-1])>=-64){
        emoji[emoji.size()-1]=-128;
        emoji[emoji.size()-2]++;
    }
    return emoji;
}

/**
 * @brief Get filename list in folder_path
 * @details 
 * @example
 *   string input_path = "../data/input/";
 *   vector<string> file_list;
 *   GetFileList(input_path, &file_list);
 * @param string folder_path: folder path
 * @param vector<string> *file_list: (return) filename list
 * @return None
 * @attention 
**/
void GetFileNameListWithoutExtention(const string folder_path, vector<string> *file_list)
{
    filesystem::path path = folder_path;
    for (const auto &entry : filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            file_list->push_back(string(entry.path().stem()));
        }
    }
    return;
}

/**
 * @brief Get file line number list
 * @details 
 * @example
 *   string folder_path = "../data/input/";
 *   vector<string> file_list;
 *   GetFileNameListWithoutExtention(folder_path,&file_list);
 *   vector<string> file_line_number_list;
 *   GetFileLineNumberList(folder_path,&file_list,&file_line_number_list);
 * @param const string folder_path: folder path
 * @param const vector<string> *file_list: (return) filename list
 * @param const string file_extention: file extention of file_list (".json",".txt", and so on.)
 * @param vector<uint64_t> *file_line_number_list: (return) file line number list
 * @return None
 * @attention 
**/
void GetFileLineNumberList(const string folder_path, 
                           const vector<string> *file_list,
                           const string file_extention,
                           vector<uint64_t> *file_line_number_list)
{
    uint64_t line_count=0;
    for(int i=0;i<(int)file_list->size();i++){
        ifstream input_file(folder_path+(*file_list)[i]+file_extention);
        cout << folder_path+(*file_list)[i]+file_extention << endl;
        string line="";
        line_count=0;
        while(getline(input_file,line)) line_count++;
        
        file_line_number_list->push_back(line_count);
        cout << "file_name:"<<(*file_list)[i]+file_extention<<endl;
        cout << "file_lines:"<<(*file_line_number_list)[i]<<endl;
        input_file.close();
    }
    return;
}

/**
 * @brief Convert string to Wstring
 * @details
 * @example
 *   string input= "こんにちわ。";
 *   wstring input_w  = ConvertUTF8ToWstring(input);
 * @param const string& src: text sentence
 * @return wstring: text sentence converted wstring
 * @attention
**/
wstring ConvertUTF8ToWstring(const string& sentence)
{
  	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
  	return converter.from_bytes(sentence);
}

/**
 * @brief Convert Wstring to string
 * @details
 * @example
 *   wstring input_w = L"こんにちわ。";
 *   string input  = ConvertWstringToUTF8(input_w);
 * @param const string& src: text sentence
 * @return wstring: text sentence converted wstring
 * @attention
**/
string ConvertWstringToUTF8(const wstring& sentence)
{
  	wstring_convert<codecvt_utf8<wchar_t> > converter;
  	return converter.to_bytes(sentence);
}

/**
 * @brief Segmentation Sentence
 * @details 
 *  Segmentation sentence is following steps...
 *      1. Check if there are sentences enclosed in quotation marks.
 *      2. Puctuations enclosed in quotation marks are ignored.
 *      3. Replace "。"(puctuation_list) to "。\n".
 *      4. Remove the last character of the string.
 * @example
 *   string sentence = "../data/input/";
 *   SegmentationSentence(sentence);
 * @param string sentence: sentence
 * @return string: segmentated sentence
 * @note Be sure to use Normalizer to normalize your corpus before this process.
 * @attention 
 *  TODO: "." to be segmented. (ignore that "12.3", "wiki.txt". )
 *  In the case of Japanese sentences, 
 *  if you are talking about a full stop, there should be a space at the end, 
 *  and the missing mark should be \n(at the end of the sentence). 
 *  Only in that case should it be used as a full stop.
**/
void SegmentSentence(string sentence, vector<string> &segments)
{
    wstring sentence_w = ConvertUTF8ToWstring(sentence);
    wstring sentence_segmented=L"";
    vector<wchar_t> quote_list     = {L'「',L'(',L'"',L'['};
    vector<wchar_t> quote_end_list = {L'」',L')',L'"',L']'};
    vector<wchar_t> punctuation_list={L'。',L'｡',L'？',L'?',L'！',L'!'};
    wstring delimiter = L"<<<NEW_LINE>>>";

    for (int i=0;i<(int)sentence_w.size();i++) {
        bool found_quote=false;
        int32_t quote_index = -1; 

        //Check if there are sentences enclosed in quotation marks
        for(int j=0;j<(int)quote_list.size();j++){
            if(sentence_w[i]==quote_list[j]){
                found_quote=true;
                quote_index=j;
            }
        }

        //Puctuations enclosed in quotation marks are ignored
        if (found_quote) {
            for(int j=i+1;j<(int)sentence_w.size();j++){
                if(sentence_w[j]==quote_end_list[quote_index]){
                    sentence_segmented+=sentence_w.substr(i,j-i+1);
                    // cout << "「」:"<<ConvertWstringToUTF8(sentence_w.substr(i,j-i+1))<<endl;
                    i=j;
                    found_quote=false;
                    break;
                }
            }
            //If there is no end quote, the beginning quote is added to the "sentence_segmented".
            if(found_quote) sentence_segmented += sentence_w[i];
        } 
        else{
            sentence_segmented += sentence_w[i];
            // replace "。"(punctuation_list) to "。\n"
            for(auto punctuation: punctuation_list){
                if(sentence_w[i]==punctuation) {
                    sentence_segmented+=delimiter;
                    break;
                }
            }
        }
    }

    size_t pos = 0;
    size_t prevPos = 0;
    
    // segmented sentence push to  segments
    while ((pos = sentence_segmented.find(delimiter, prevPos)) != wstring::npos) {
        wstring sub = sentence_segmented.substr(prevPos, pos - prevPos);
        segments.push_back(ConvertWstringToUTF8(sub));
        prevPos = pos + delimiter.length();
    }

    // Add the last substring after the last delimiter
    if (prevPos < sentence_segmented.length()) {
        wstring sub = sentence_segmented.substr(prevPos);
        segments.push_back(ConvertWstringToUTF8(sub));
    }
}

/**
 * @brief Remove leading and trailing white space
 * @details
 * @example
 *   string sentence= "  こんにちわ。  ";
 *   sentence  = Strip(sentence); //"こんにちは。"
 * @param const string& sentence: text sentence
 * @return string: sentence has been processed
 * @attention
**/
string Strip(const string& sentence)
{
	// Search for non-blank characters from the beginning
    size_t start = sentence.find_first_not_of(" \t\n\r");
	// Search for non-blank characters from the end
    size_t end = sentence.find_last_not_of(" \t\n\r");
	// For empty string
    if (start == string::npos) return "";

	// Return substring from beginning to end
    return sentence.substr(start, end - start + 1);
}

void ReplaceSubstring(string& sentence, const string& target, const string& replacement) 
{
    size_t pos = 0;
    while ((pos = sentence.find(target, pos)) != string::npos) {
        sentence.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
}

/**
 * @brief Get file name from path without file extention (.txt).
 * @details
 * Example:
 * 
 *   string path = "/path/to/input.txt";
 *   cout << GetFileName(path)<< endl; // /path/to/input
 * @param const string& sentence: text sentence
 * @return string: sentence has been processed
 * @attention
**/
string GetFilePathWithoutExtention(const string& file_path) 
{  
    filesystem::path path_object(file_path);
    // Get file name (without extension)
    filesystem::path  fs_file_path_witout_extention = path_object.parent_path() / path_object.stem();
    string file_path_witout_extention = fs_file_path_witout_extention.string();
    return file_path_witout_extention;    
}


/**
 * @brief Extract file name list from file path list and add .jsonl extention
 * @details
 * Example:
 *   vector<string> path_list,filename_list;
 *   path_list.push_back("/path/to/input.txt");
 *   path_list.push_back("/path/to/input2.txt");
 *   GetFileNameListAddedJsonl(pathlist,filename_list)
 *   for(auto filename:filename_list)cout << filename << endl;
 *   // /path/to/input.jsonl /path/to/input2.jsonl
 * @param const vector<string> &file_path_list: original file path list
 * @param vector<string> &jsonl_file_path_list: (output) file path list added jsonl extention
 * @return void: None
 * @attention
**/
void GetFileNameListAddedJsonl(const vector<string> &file_path_list,
                               vector<string> &jsonl_file_path_list) 
{  
    for(auto file_path:file_path_list){
        string filepath_witout_extention = GetFilePathWithoutExtention(file_path);
        jsonl_file_path_list.push_back(filepath_witout_extention+".jsonl");
    }
}

/**
 * @brief Escape word
 * @details
 * If " or ' or \, replace to \", \', and \\.
 *
 * Example:
 *   string input = """;
 *   string output =  EscapeWord(input);
 *   // output == "\""
 * @param const string& sentence: text sentence
 * @return string: converted sentence
 * @attention
**/
string EscapeWord(const string& input) 
{
    string output="";
    for(char word: input){
        if(word == L'\"') output+= "\\\"";
        else if(word == '\t') output+="\\t";
        else if(word == '\\') output+="\\\\";
        else output+=word;
        // else if(word == '\'') output+="\\\'";
        // else if(word == '\t') output+="\\\t";
    } 
    return output;
    // else if(word == '　') output+="\\u3000";
}

/**
 * @brief Update progress bar
 * @details 
 * @example
 * @param uint64_t line_count
 * @param uint64_t file_line_number
 * @return None
 * @attention 
**/
void ProceedProgressBar(unsigned long long line_count,unsigned long long file_line_number,uint32_t elapsed_time_ms)
{
    //Division by 0 occurs
    if(file_line_number==0) return; 
    if(line_count==0) return;

    // clear terminal
    // cout << "\033[2J\033[1;1H";
    // cout << "\033[1;1H\033[2K";
    cout.fill('0');
    cout << "\033[2K";

    unsigned long long progress_percentage = (unsigned long long)((double(line_count))/double(file_line_number)*100);
    //printf("\r%llu%% |",progress_percentage);
    cout << "\r" << progress_percentage << "% |"; 
    for(int i=0;i<int(progress_percentage/5);i++) cout << "█";//printf("█");
    for(int i=0;i<int(20-progress_percentage/5);i++) cout << " ";//printf(" ");
    //printf("| %llu/%llu",line_count,file_line_number);
    cout << "| "<< line_count << "/" << file_line_number;
 
    //passed time
    uint32_t hours = elapsed_time_ms / (1000 * 60 * 60);
    uint32_t minutes = (elapsed_time_ms / (1000 * 60)) % 60;
    uint32_t seconds = (elapsed_time_ms / (1000)) % 60;
    //printf(" [%02d:%02d:%02d",hours,minutes,seconds);
    cout << setw(2) << "[" << hours << ":" << minutes << ":"<< seconds;
 
    //Remaining time
    uint32_t remaining_time = uint32_t(double(elapsed_time_ms)/line_count*(file_line_number-line_count));
    hours = remaining_time / (1000 * 60 * 60);
    minutes = (remaining_time / (1000 * 60)) % 60;
    seconds = (remaining_time / (1000)) % 60;
    //printf("<%02d:%02d:%02d",hours,minutes,seconds);
    cout << setw(2)  << "<"<<hours << ":" << minutes << ":" <<seconds;

    //second per iteration
    double msecond_per_iter = double(elapsed_time_ms)/line_count;
    //printf(", %.2fms/it",msecond_per_iter);
    //printf("]");
    cout << setprecision(2) << "," << msecond_per_iter << "ms/it" ; 
    cout << "]"; 
    fflush(stdout);
    if (progress_percentage==100)   cout << endl;
}