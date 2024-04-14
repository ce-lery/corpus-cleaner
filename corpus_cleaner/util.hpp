#include <bits/stdc++.h>
#include <sys/stat.h>

using namespace std;
namespace fs = filesystem;

uint32_t strlen_utf8(string input);
void RemoveFolder(const string& path);
string CalculateNextEmoji(string pre_emoji);
void GetFileNameListWithoutExtention(string folder_path, vector<string> *file_list);
void GetFileLineNumberList(const string folder_path, 
                           const vector<string> *file_list, 
                           const string file_extention,
                           vector<uint64_t> *file_line_number_list);
void CopyFolder(string source_folder, string target_folder);
void CopyFile(string source_path, string target_path);
void MoveFolder(string source_folder, string target_folder);
wstring ConvertUTF8ToWstring(const string& sentence);
string ConvertWstringToUTF8(const wstring& sentence);
void SegmentSentence(string sentence, vector<string> &segments);
string Strip(const string& sentence);
void ReplaceSubstring(string& sentence, const string& target, const string& replacement);
string EscapeWord(const string& input);
void ProceedProgressBar(unsigned long long line_count,
                        unsigned long long file_line_number,
                        uint32_t elapsed_time_ms);
