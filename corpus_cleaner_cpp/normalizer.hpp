#include <bits/stdc++.h>

using namespace std;
namespace fs = filesystem;

wstring UnicodeNormalize(wregex word_pattern,wstring sentence_w);
string Strip(const string& sentence);
wstring TranslateToFullwidth(const wstring& sentence_w);
wstring RemoveExtraSpaces(const wstring& sentence);
string NormalizeNeologd(string sentence);
int Normalizer(string input_path,string output_path);