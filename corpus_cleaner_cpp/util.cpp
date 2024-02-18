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