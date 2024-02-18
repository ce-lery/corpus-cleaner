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
 * @brief copy source_folder to target_folder
 * @details 
 * @example
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
void GetFileList(string folder_path, vector<string> *file_list)
{
    fs::path path = folder_path;
    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            file_list->push_back(string(entry.path().filename()));
        }
    }
    return;
}
