#include "minhash.hpp"
#include "util.hpp"

/***constructor***/
GenerateDedupLSH::GenerateDedupLSH()
{
}

/***destructor***/
GenerateDedupLSH::~GenerateDedupLSH()
{
}

/**
 * @brief Tokenize a string into n-gram tokens.
 * @details 
 * @example
 *   GenerateDedupLSH generate_dedupe_lsh;
 *   generate_dedupe_lsh.n_gram_tokenize(L"おはようございます。", 3);
 *   // {"おはよ", "はよう", "ようご", "うござ", "ござい", "ざいま", "います", "ます。"}
 * @param wstring text: input text
 * @param string n: the n number of n_gram
 * @return ector<wstring> : n_gram tokenized text
 * @ref https://github.com/HojiChar/HojiChar/blob/v0.9.0/hojichar/filters/deduplication.py
 * @attention 
**/
vector<wstring> GenerateDedupLSH::NGramTokenize(wstring text, int32_t n)
{
    vector<wstring> tokenized_text;

    if ((int32_t)text.size() < n){
        tokenized_text.push_back(text);
        return tokenized_text;
    }
    else{
        for(int32_t i=0;i<(int32_t)text.size();i++){
            if(i+n-1>=(int32_t)text.size()){
                break;
            }
            else{
                tokenized_text.push_back(text.substr(i,n));
            }
        }
        return tokenized_text;
    }
}

// int main(void)
// {
//     GenerateDedupLSH generate_dedupe_lsh;
//     vector<wstring> ret = generate_dedupe_lsh.NGramTokenize(L"おはよう🤗ございます。Huggingface", 3);
//     for(int i=0;i<(int)ret.size();i++)  cout << ConvertWstringToUTF8(ret[i]) <<" ";
//     cout <<endl;
// }