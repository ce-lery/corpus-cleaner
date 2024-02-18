#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
using namespace std;
namespace fs = filesystem;

uint32_t strlen_utf8(string input);
void RemoveFolder(const string& path);
string CalculateNextEmoji(string pre_emoji);
void GetFileList(string folder_path, vector<string> *file_list);
void CopyFolder(string source_folder, string target_folder);
void CopyFile(string source_path, string target_path);
