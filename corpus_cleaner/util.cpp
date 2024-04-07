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
    fs::path source = source_path;
    fs::path destination = target_path;
    fs::copy(source, destination, fs::copy_options::overwrite_existing);
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
    fs::path source = source_path;
    fs::path destination = target_folder;
    fs::path new_path = destination / source.filename();
    fs::rename(source, new_path);
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
    fs::path sourceDir = source_folder;
    fs::path destinationDir = target_folder;
    for (const auto &file : fs::directory_iterator(sourceDir)) {
        if (fs::is_regular_file(file.path())) {
            fs::copy(file, destinationDir / file.path().filename(), fs::copy_options::overwrite_existing);
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
    if (!fs::exists(source_folder)) {
        std::cerr << "Source directory does not exist: " << source_folder << std::endl;
        return;
    }
    if (!fs::exists(target_folder)) fs::create_directory(target_folder);

    // Move file and folder
    for (const auto& entry : fs::directory_iterator(source_folder)) {
        try {
            fs::path newPath = target_folder / entry.path().filename();
            fs::rename(entry.path(), newPath);
            std::cout << "Moved: " << entry.path().filename() << std::endl;
        } catch (const fs::filesystem_error& e) {
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
    fs::path path = folder_path;
    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            file_list->push_back(string(entry.path().stem()));
        }
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
wstring ConvertUTF8ToWstring(const string& src)
{
  	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
  	return converter.from_bytes(src);
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
string ConvertWstringToUTF8(const wstring& src)
{
  	wstring_convert<codecvt_utf8<wchar_t> > converter;
  	return converter.to_bytes(src);
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
    vector<wchar_t> puctuation_list={L'。',L'?',L'!'};
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
            // replace "。"(puctuation_list) to "。\n"
            for(auto puctuation: puctuation_list){
                if(sentence_w[i]==puctuation) {
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
    fs::path path_object(file_path);
    // Get file name (without extension)
    fs::path  fs_file_path_witout_extention = path_object.parent_path() / path_object.stem();
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