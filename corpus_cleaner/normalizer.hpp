#include <bits/stdc++.h>
#include <unicode/datefmt.h>
#include <unicode/dtfmtsym.h>
#include <unicode/gregocal.h>
#include <unicode/timezone.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/dtptngen.h>
#include <unicode/dtitvfmt.h>
#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <unicode/uclean.h> 

using namespace std;
namespace fs = filesystem;

class StringNormalizer
{
private:
    const icu::Normalizer2* normalizer;
public:
    /***constructor***/
    StringNormalizer();
    /***destructor**/
    ~StringNormalizer();
    wstring UnicodeNormalize(wregex word_pattern,wstring sentence_w);
    wstring TranslateToFullwidth(const wstring& sentence_w);
    wstring RemoveExtraSpaces(const wstring& sentence);
    string NormalizeNeologd(string sentence);
    int Normalizer(string input_path,string output_path);
};
