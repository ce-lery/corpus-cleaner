#include "normalizer.hpp"
#include "util.hpp"
// #include "util.cpp"

using namespace std;


/***constructor***/
StringNormalizer::StringNormalizer()
{
    UErrorCode errc = U_ZERO_ERROR;
    errc = U_ZERO_ERROR;

	// generate NFKC normalizer instance
    this->normalizer = icu::Normalizer2::getNFKCInstance(errc);   
}

/***destructor***/
StringNormalizer::~StringNormalizer()
{
    //u_cleanup();
}


/**
 * @brief nfkc normalize sentence by icu::Normalizer2
 * @details
 *  Search for words that match the word_pattern regular expression in the sentence
 *  and perform NFKC normalization using icu::Normalizer2.
 * 
 * Example:
 * ```cpp
 *   wstring sentence = L"０１２３４５６７８９";
 *   static wregex word_pattern(L"(([０-９]+))");
 *   wstring normalized_sentence = UnicodeNormalize(word_pattern, sentence)
 *   // normalized_sentence == L"0123456789"
 * ```
 * @param wregex word_pattern: Regular expression for string to be normalized
 * @param wstring: sentence
 * @return wstring: normalized sentence
 * @ref https://ja.wikipedia.org/wiki/Unicode%E4%B8%80%E8%A6%A7_0000-0FFF
 * https://www.nslabs.jp/icu-normalization.rhtml
 * @note
 * regex_search sometimes causes segmentation fault when the sentence if over 15kB.
 * So, in this function, split sentence to chunk and apply regex_search to every chunk.
 * https://stackoverflow.com/questions/56333295/regex-segmentation-fault
 * https://stackoverflow.com/questions/56905134/c-stdregex-segmentation-fault
**/
wstring StringNormalizer::UnicodeNormalize(wregex word_pattern,wstring sentence_w)
{
    static wregex hyphen_pattern(L"－");
    const size_t chunk_size = 500;
    const size_t overlap = 100; 
    
    // if sentence_w length is less than chunk_size, do usually normalization.
    if (sentence_w.length() <= chunk_size) \
        return NormalizeChunk(word_pattern, sentence_w, hyphen_pattern);
    
    wstring result;
    size_t pos = 0;
    
    while (pos < sentence_w.length()) {
        // calculate the termination of chunk
        size_t end = min(pos + chunk_size, sentence_w.length());
        wstring chunk = sentence_w.substr(pos, end - pos);
        
        // If the chunk isn't the last, add overlap
        if (end < sentence_w.length() && end + overlap <= sentence_w.length()) \
            chunk += sentence_w.substr(end, overlap);
        
        // normalize per chunk
        wstring normalizedChunk = NormalizeChunk(word_pattern, chunk, hyphen_pattern);
        
        // add chuk to result(without overlap part)
        if (pos == 0) {
            if (end < sentence_w.length())  result = normalizedChunk.substr(0, chunk_size);
            else                            result = normalizedChunk;
        } else {
            size_t actualChunkSize = min(chunk_size, sentence_w.length() - pos);
            if (end < sentence_w.length())  result += normalizedChunk.substr(0, actualChunkSize);
            else                            result += normalizedChunk;
        }
        pos += chunk_size;
    }
    return result;
}


wstring StringNormalizer::NormalizeChunk(const wregex& word_pattern, wstring chunk, const wregex& hyphen_pattern)
{
    wsmatch matches;
    while (regex_search(chunk, matches, word_pattern)) {
        // cout <<"matches.str():"<<ConvertWstringToUTF8(matches.str())<<endl;
		//caution: must initialization of errc
        UErrorCode errc = U_ZERO_ERROR;

        // convert matching part of sentence to UnicodeString
        icu::UnicodeString match(ConvertWstringToUTF8(matches.str()).c_str(), "UTF-8");
        // Normalize the matching part of sentence
        icu::UnicodeString match_morph;
        this->normalizer->normalize(match,match_morph,errc);

        // add error check
        if (U_FAILURE(errc))    continue;

        // convert normalized sentence to string
        string normalizedMatch_temp;
        match_morph.toUTF8String(normalizedMatch_temp);
        // cout <<"normalizedMatch_temp:"<<normalizedMatch_temp<<endl;

        wstring normalizedMatch = ConvertUTF8ToWstring(normalizedMatch_temp);
        // replace original text to normalized text
        chunk.replace(matches.position(), matches.length(), normalizedMatch);
        
        match.remove();
        match_morph.remove();
    }

    chunk = regex_replace(chunk,hyphen_pattern,L"-");

    return chunk;
}




/**
 * @brief Replace a specific string from half-width to full-width
 * @details
 *  Replace the following full-width symbols with half-width symbols
 *  /！”＃＄％＆’（）＊＋，−．／：；＜＞？＠［￥］＾＿｀｛｜｝
 * 
 * Example:  
 * ```cpp
 *   wstring sentence= "（）";
 *   sentence  = TranslateToFullwidth(sentence); //"()"
 * ```
 * @param const string& sentence: text sentence
 * @return wstring: sentence has been processed
 * @note
**/
wstring StringNormalizer::TranslateToFullwidth(const wstring& sentence_w)
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
 * 
 * Example:  
 * ```cpp
 *   wstring sentence= "（）";
 *   sentence  = TranslateToFullwidth(sentence); //"()"
 * ```
 * @param const string& sentence: text sentence
 * @return wstring: sentence has been processed
 * @note
**/
wstring StringNormalizer::RemoveExtraSpaces(const wstring& sentence)
{
    const size_t CHUNK_SIZE = 500;
    const size_t OVERLAP = 10;
    
    if (sentence.length() <= CHUNK_SIZE) {
        return RemoveExtraSpacesChunk(sentence);
    }
    
    wstring result;
    result.reserve(sentence.length());
    
    for (size_t pos = 0; pos < sentence.length(); pos += CHUNK_SIZE) {
        size_t end = min(pos + CHUNK_SIZE + OVERLAP, sentence.length());
        wstring chunk = sentence.substr(pos, end - pos);
        
        wstring processedChunk = RemoveExtraSpacesChunk(chunk);
        
        // treal with overlap
        size_t appendSize = min(CHUNK_SIZE, processedChunk.length());
        if (pos + CHUNK_SIZE >= sentence.length()) {
            appendSize = processedChunk.length();
        }
        
        result.append(processedChunk, 0, appendSize);
    }
    
    return FinalSpaceCleanup(result);
}

wstring StringNormalizer::RemoveExtraSpacesChunk(const wstring& chunk)
{
    static const wregex spacePattern(L"[ 　]+");
    static const wstring blocks = LR"(\u4E00-\u9FFF\u3040-\u309F\u30A0-\u30FF\u3000-\u303F\uFF00-\uFFEF)";
    static const wstring basicLatin = LR"(\u0000-\u007F)";
    static const wregex blockToBlockPattern(L"([" + blocks + L"]) ([" + blocks + L"])");
    static const wregex blockToLatinPattern(L"([" + blocks + L"]) ([" + basicLatin + L"])");
    static const wregex latinToBlockPattern(L"([" + basicLatin + L"]) ([" + blocks + L"])");
    
    // convert consecutive spaces to single spaces
    wstring result = regex_replace(chunk, spacePattern, L" ");
    
    // remove spaces between each character class
    auto removeSpaceBetweenClasses = [&](const wregex& pattern) {
        while (regex_search(result, pattern)) {
            result = regex_replace(result, pattern, L"$1$2");
        }
    };
    
    removeSpaceBetweenClasses(blockToBlockPattern);
    removeSpaceBetweenClasses(blockToLatinPattern);
    removeSpaceBetweenClasses(latinToBlockPattern);
    
    return result;
}

wstring StringNormalizer::FinalSpaceCleanup(const wstring& text)
{
    static const wstring blocks = LR"(\u4E00-\u9FFF\u3040-\u309F\u30A0-\u30FF\u3000-\u303F\uFF00-\uFFEF)";
    static const wstring basicLatin = LR"(\u0000-\u007F)";
    static const wregex blockToBlockPattern(L"([" + blocks + L"]) ([" + blocks + L"])");
    static const wregex blockToLatinPattern(L"([" + blocks + L"]) ([" + basicLatin + L"])");
    static const wregex latinToBlockPattern(L"([" + basicLatin + L"]) ([" + blocks + L"])");
    
    wstring result = text;
    
    auto removeSpaceBetweenClasses = [&](const wregex& pattern) {
        while (regex_search(result, pattern)) {
            result = regex_replace(result, pattern, L"$1$2");
        }
    };
    
    removeSpaceBetweenClasses(blockToBlockPattern);
    removeSpaceBetweenClasses(blockToLatinPattern);
    removeSpaceBetweenClasses(latinToBlockPattern);
    
    return result;
}
/**
 * @brief Neologd Normalized function
 * @details
 * Perform the normalization process described in the link below.
 * https://github.com/neologd/mecab-ipadic-neologd/wiki/Regexp.ja  
 * 
 * Example:
 * ```cpp
 *   string sentence= "検索 エンジン 自作 入門 を 買い ました!!!";
 *   sentence = NormalizeNeologd(sentence); //"検索エンジン自作入門を買いました"
 * ```
 * @param const string& sentence: text sentence
 * @return wstring: sentence has been processed
 * @attention
 *  This process is for Japanese text. Do not use English text or code in your corpus.
 *  For example, in English text, spaces between words will be removed.
**/
string StringNormalizer::NormalizeNeologd(string sentence)
{
    static wregex word_pattern(L"(([０-９Ａ-Ｚａ-ｚ｡-ﾟ]+))");

	//Remove leadingand trailing white space
    sentence = Strip(sentence);

	// Normalize full-width alphanumeric characters
	wstring sentence_w = ConvertUTF8ToWstring(sentence);
    sentence_w = this->UnicodeNormalize(word_pattern,sentence_w);

	//Normalize hyphens
	static wregex small_hyhpen_pattern(L"([˗֊‐‑‒–⁃⁻₋−]+)");
    sentence_w =  regex_replace(sentence_w,small_hyhpen_pattern,L"-");
	static wregex large_hyhpen_pattern(L"([﹣－—―─━ーｰ]+)");
    sentence_w =  regex_replace(sentence_w,large_hyhpen_pattern,L"ー");

	// Remove tilde words
	static wregex tilde_pattern(L"[~∼∾〜〰～]");
	sentence_w =  regex_replace(sentence_w,tilde_pattern,L"");

    sentence_w = this->TranslateToFullwidth(sentence_w);
    sentence_w = this->RemoveExtraSpaces(sentence_w);

    static wregex special_word_pattern(L"(([！＃＄％＆（）＊＋，－．／：；＜＞？＠［￥］＾＿｀｛｜｝〜]+))");

    sentence_w = this->UnicodeNormalize(special_word_pattern,sentence_w);
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