#include "normalizer.hpp"
#include "util.hpp"
// #include "util.cpp"

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


using namespace std;

/**
 * @brief nfkc normalize sentence by icu::Normalizer2
 * @details
 *  Search for words that match the word_pattern regular expression in the sentence
 *  and perform NFKC normalization using icu::Normalizer2.
 * @example
 *   wstring sentence = L"０１２３４５６７８９";
 *   static wregex word_pattern(L"(([０-９]+))");
 *   wstring normalized_sentence = UnicodeNormalize(word_pattern, sentence)
 *   // normalized_sentence == L"0123456789"
 * @param wregex word_pattern: Regular expression for string to be normalized
 * @param wstring: sentence
 * @return wstring: normalized sentence
 * @ref https://ja.wikipedia.org/wiki/Unicode%E4%B8%80%E8%A6%A7_0000-0FFF
 * @attention
**/
wstring UnicodeNormalize(wregex word_pattern,wstring sentence_w)
{
    static wregex hyphen_pattern(L"－");

	//object for stock the part of matchinng string
    wsmatch matches;
    while (regex_search(sentence_w, matches, word_pattern)) {
        // cout <<"matches.str():"<<ConvertWstringToUTF8(matches.str())<<endl;
		//caution: must initialization of errc
        UErrorCode errc = U_ZERO_ERROR;
        errc = U_ZERO_ERROR;

		// generate NFKC normalizer instance
        const icu::Normalizer2* normalizer = icu::Normalizer2::getNFKCInstance(errc);

		// convert matching part of sentence to UnicodeString
        icu::UnicodeString match(ConvertWstringToUTF8(matches.str()).c_str(), "UTF-8");
		// Normalize the matching part of sentence
        icu::UnicodeString match_morph;
        normalizer->normalize(match,match_morph,errc);

		// convert normalized sentence to string
        string normalizedMatch_temp;
        match_morph.toUTF8String(normalizedMatch_temp);
        // cout <<"normalizedMatch_temp:"<<normalizedMatch_temp<<endl;

        wstring normalizedMatch = ConvertUTF8ToWstring(normalizedMatch_temp);
		// replace original text to normalized text
        sentence_w.replace(matches.position(), matches.length(), normalizedMatch);
    }

    sentence_w = regex_replace(sentence_w,hyphen_pattern,L"-");
    return sentence_w;
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


/**
 * @brief Replace a specific string from half-width to full-width
 * @details
 *  Replace the following full-width symbols with half-width symbols
 *  /！”＃＄％＆’（）＊＋，−．／：；＜＞？＠［￥］＾＿｀｛｜｝
 * @example
 *   wstring sentence= "（）";
 *   sentence  = TranslateToFullwidth(sentence); //"()"
 * @param const string& sentence: text sentence
 * @return wstring: sentence has been processed
 * @attention
**/
wstring TranslateToFullwidth(const wstring& sentence_w)
{
    unordered_map<wchar_t, wchar_t> conversion_map = {
        {u'!', u'！'}, {u'"', u'”'},  {u'#', u'＃'}, {u'$', u'＄'}, {u'%', u'％'},
        {u'&', u'＆'}, {u'\'',u'’'},  {u'(', u'（'}, {u')', u'）'}, {u'*', u'＊'},
        {u'+', u'＋'}, {u',', u'，'}, {u'-', u'－'}, {u'.', u'．'}, {u'/', u'／'},
        {u':', u'：'}, {u';', u'；'}, {u'<', u'＜'}, {u'=', u'＝'}, {u'>', u'＞'},
        {u'?', u'？'}, {u'@', u'＠'}, {u'[', u'［'}, {u']', u'］'}, {u'^', u'＾'},
        {u'_', u'＿'}, {u'`', u'｀'}, {u'{', u'｛'}, {u'|', u'｜'}, {u'}', u'｝'},
        {u'~', u'〜'}
    };

    wstring output;
    for (wchar_t word : sentence_w) {
        if (conversion_map.find(word) != conversion_map.end()) {
            output += conversion_map[word];
        } else {
            output += word;
        }
    }

    return output;
}

/**
 * @brief remove half-width spaces that meet the conditions
 * @details
 *  Replace one or more half-width spaces with one half-width space.
 *  And Remove half-width spaces included in the following conditions.
 *  - Half-width spaces included between "hiragana, full-width katakana, 
 *    half-width katakana, kanji, and full-width symbols"
 *  - Half-width space included between "hiragana, full-width katakana, 
 *    half-width katakana, kanji, 
 *    full-width symbols" and "half-width alphanumeric characters"
 * @example
 *   wstring sentence= "（）";
 *   sentence  = TranslateToFullwidth(sentence); //"()"
 * @param const string& sentence: text sentence
 * @return wstring: sentence has been processed
 * @attention
**/
wstring RemoveExtraSpaces(const wstring& sentence)
{
    wstring result = regex_replace(sentence, wregex(L"[ 　]+"), L" ");

    wstring blocks = LR"(\u4E00-\u9FFF\u3040-\u309F\u30A0-\u30FF\u3000-\u303F\uFF00-\uFFEF)";
    wstring basicLatin = LR"(\u0000-\u007F)";

    auto removeSpaceBetween = [](const wstring& cls1, const wstring& cls2, wstring str) {
        wregex pattern(L"([" + cls1 + L"]) ([" + cls2 + L"])");
        while (regex_search(str, pattern)) {
            str = regex_replace(str, pattern, L"$1$2");
        }
        return str;
    };

    result = removeSpaceBetween(blocks, blocks, result);
    result = removeSpaceBetween(blocks, basicLatin, result);
    result = removeSpaceBetween(basicLatin, blocks, result);

    return result;
}

/**
 * @brief Neologd Normalized function
 * @details
 * Perform the normalization process described in the link below.
 *  https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja
 * @example
 *   string sentence= "検索 エンジン 自作 入門 を 買い ました!!!";
 *   sentence = NormalizeNeologd(sentence); //"検索エンジン自作入門を買いました"
 * @param const string& sentence: text sentence
 * @return wstring: sentence has been processed
 * @attention
 *  This process is for Japanese text. Do not use English text or code in your corpus.
 *  For example, in English text, spaces between words will be removed.
**/
string NormalizeNeologd(string sentence)
{
    static wregex word_pattern(L"(([０-９Ａ-Ｚａ-ｚ｡-ﾟ]+))");

	//Remove leadingand trailing white space
    sentence = Strip(sentence);

	// Normalize full-width alphanumeric characters
	wstring sentence_w = ConvertUTF8ToWstring(sentence);
    sentence_w = UnicodeNormalize(word_pattern,sentence_w);

	//Normalize hyphens
	static wregex small_hyhpen_pattern(L"([˗֊‐‑‒–⁃⁻₋−]+)");
    sentence_w =  regex_replace(sentence_w,small_hyhpen_pattern,L"-");
	static wregex large_hyhpen_pattern(L"([﹣－—―─━ーｰ]+)");
    sentence_w =  regex_replace(sentence_w,large_hyhpen_pattern,L"ー");

	// Remove tilde words
	static wregex tilde_pattern(L"[~∼∾〜〰～]");
	sentence_w =  regex_replace(sentence_w,tilde_pattern,L"");

    sentence_w =TranslateToFullwidth(sentence_w);
    sentence_w = RemoveExtraSpaces(sentence_w);

    static wregex special_word_pattern(L"(([！＃＄％＆（）＊＋，－．／：；＜＞？＠［￥］＾＿｀｛｜｝〜]+))");

    sentence_w = UnicodeNormalize(special_word_pattern,sentence_w);
    static wregex quotation_pattern(L"[’]");
    sentence_w =  regex_replace(sentence_w,quotation_pattern,L"\'");
    static wregex wquotation_pattern(L"[”]");
    sentence_w =  regex_replace(sentence_w,wquotation_pattern,L"\"");
    static wregex equal_pattern(L"[=]");
    sentence_w =  regex_replace(sentence_w,equal_pattern,L"＝");

    sentence = ConvertWstringToUTF8(sentence_w);
    sentence = Strip(sentence);
    return sentence;
}



/*
int main(void)
{
    string input_path = "../data/wiki_test.txt";
    string output_path = "../results/wiki_test_NormalizeNeologd.txt";

    // Normalizer(input_path,output_path);

    //original
    assert("Hello,C++!" == NormalizeNeologd("   Hello, C++!   "));// TODO: Write the comment that this normalizer is don't applied for English text. Because spaces are removed.
    assert("-" == NormalizeNeologd("˗֊‐‑‒–⁃⁻₋−"));
    assert("-" == NormalizeNeologd("－"));
    assert("ー" == NormalizeNeologd("﹣—―─━ーｰ"));
    assert("＝" == NormalizeNeologd("="));

    assert("0123456789" == NormalizeNeologd("０１２３４５６７８９"));
    assert("ABCDEFGHIJKLMNOPQRSTUVWXYZ" == NormalizeNeologd("ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"));
    assert("abcdefghijklmnopqrstuvwxyz" == NormalizeNeologd("ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"));
    assert("!\"#$%&\'()*+,-./:;<>?@[¥]^_`{|}" == NormalizeNeologd("！”＃＄％＆’（）＊＋，－．／：；＜＞？＠［￥］＾＿｀｛｜｝"));
    assert("＝。、・「」" == NormalizeNeologd("＝。、・「」"));
    assert("ハンカク" == NormalizeNeologd("ﾊﾝｶｸ"));
    assert("o-o" == NormalizeNeologd("o₋o"));
    assert("majikaー" == NormalizeNeologd("majika━"));
    assert("わい" == NormalizeNeologd("わ〰い"));
    assert("スーパー" == NormalizeNeologd("スーパーーーー"));
    assert("!#" == NormalizeNeologd("!#"));
    assert("ゼンカクスペース" == NormalizeNeologd("ゼンカク　スペース"));
    assert("おお" == NormalizeNeologd("お             お"));
    assert("おお" == NormalizeNeologd("      おお"));
    assert("おお" == NormalizeNeologd("おお      "));
    assert("検索エンジン自作入門を買いました!!!" ==NormalizeNeologd("検索 エンジン 自作 入門 を 買い ました!!!"));
    assert("アルゴリズムC" == NormalizeNeologd("アルゴリズム C"));
    assert("PRML副読本" == NormalizeNeologd("　　　ＰＲＭＬ　　副　読　本　　　"));
    assert("Coding the Matrix" == NormalizeNeologd("Coding the Matrix"));
    assert("南アルプスの天然水Sparking Lemonレモン一絞り" == NormalizeNeologd("南アルプスの　天然水　Ｓｐａｒｋｉｎｇ　Ｌｅｍｏｎ　レモン一絞り"));
    assert("南アルプスの天然水-Sparking*Lemon+レモン一絞り" == NormalizeNeologd("南アルプスの　天然水-　Ｓｐａｒｋｉｎｇ*　Ｌｅｍｏｎ+　レモン一絞り"));
	cout << "Normalizing Text is completed." << endl;
    return 0;
}
*/