#include "minhash.hpp"
#include "util.hpp"
#include "../scripts/smhasher/src/MurmurHash3.h"

/***constructor***/
GenerateDedupLSH::GenerateDedupLSH(uint32_t n_gram=5,
                                   uint32_t n_minhash=200,
                                   uint32_t n_buckets=20,
                                   uint32_t bucket_size=10)
{
    assert(n_minhash==n_buckets*bucket_size);
    this->bucket_size = bucket_size;
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

/**
 * @brief Calculate minhash of tokens list
 * @details 
 * @example
 *   GenerateDedupLSH generate_dedupe_lsh(3);
 *   wstring text = L"おはようございます。";
 *   vector<wstring> tokens = generate_dedupe_lsh.NGramTokenize(text, 3);
 *   uint64_t minhash = generate_dedupe_lsh.GetMinhash(&tokens,0);
 *   //minhash == 2147483647
 * @param vector<wstring> *tokens: tokens list
 * @param uint32_t seed: the seed for murmurminhash3's calculation
 * @return uint64_t : minhash
 * @ref https://github.com/HojiChar/HojiChar/blob/v0.9.0/hojichar/filters/deduplication.py
 * @attention 
**/
uint64_t GenerateDedupLSH::GetMinhash(vector<wstring> *tokens,uint32_t seed)
{
    uint64_t minhash = numeric_limits<int>::max();//INF
    uint64_t out[2] = {};
    for(auto token:*tokens) {
        MurmurHash3_x64_128(token.data(), token.length(), seed, &out);
        minhash = min(minhash,out[0]);
    }
    return minhash;
}

/**
 * @brief Calculate minhash list of text
 * @details 
 * Generates a sequence of hash values ​​for duplicate handling from text.
 * If two documents have the same hash value at most, the documents are considered duplicates.
 * A list of hash values, each hash value is in the format '0+07ad0b7b163f434643387f3f4799a2d466bccd0c',
 * The first two characters represent the hash value.
 * This allows duplicate processing by pooling duplicate processing hashes into a single hash table.
 * 
 * @example
 * @param wstring text: input sentence
 * @return vector<string> : hash list
 * @ref 
 * https://github.com/HojiChar/HojiChar/blob/v0.9.0/hojichar/filters/deduplication.py
 * https://arxiv.org/abs/2107.06499 , Appendix A
 * https://arxiv.org/abs/2107.06499 , Appendix A
 * @attention 
**/
vector<string> GenerateDedupLSH::CalculateLSH(wstring text)
{
    // N_MINHASH 個の mmh3 ハッシュ値 から最終的に N_BUKET 個の重複処理ハッシュを計算する

    // n_minhash個のfingerprints要素ができる
    vector<uint64_t> fingerprints;
    for(uint32_t seed=0;seed<this->n_minhash;seed++){
        // tokenized n-gram
        vector<wstring> tokens = NGramTokenize(text,this->n_gram);
        //calculate minhash
        uint64_t minhash = this->GetMinhash(&tokens,seed);
        fingerprints.push_back(minhash);
    }

    vector<string> lshs;
    for(uint32_t bucket_idx=0;bucket_idx<this->n_buckets;bucket_idx++){
        string hash = "";
        char buffer[200]={};
        for(uint32_t fp_idx=bucket_idx*this->bucket_size;fp_idx<(bucket_idx+1)*this->bucket_size;fp_idx++){
            sprintf(buffer, "%04lx", fingerprints[fp_idx]);
            string temp(buffer);
            //下４桁を削除
            for(int i=0;i<4;i++)    temp.pop_back();
            hash+=temp;
        }
        lshs.push_back(to_string(bucket_idx)+"+"+hash);
    }
    return lshs;
}

// /**
//  * @brief Calculate minhash list of text
//  * @details 
//  * Generates a sequence of hash values ​​for duplicate handling from text.
//  * If two documents have the same hash value at most, the documents are considered duplicates.
//  * A list of hash values, each hash value is in the format '0+07ad0b7b163f434643387f3f4799a2d466bccd0c',
//  * The first two characters represent the hash value.
//  * This allows duplicate processing by pooling duplicate processing hashes into a single hash table.
//  * 
//  * @example
//  * @param wstring text: input sentence
//  * @return vector<string> : hash list
//  * @ref 
//  * https://github.com/HojiChar/HojiChar/blob/v0.9.0/hojichar/filters/deduplication.py
//  * https://arxiv.org/abs/2107.06499 , Appendix A
//  * https://arxiv.org/abs/2107.06499 , Appendix A
//  * @attention 
// **/
// vector<string> GenerateDedupLSH::Apply(wstring text)
// {
// }


/***constructor***/
LSHDeduplicator::LSHDeduplicator(bool online_dedup=true,
                                string blacklist_path="",
                                bool store_blacklist=false)
{
    this->online_dedup = online_dedup;
    this->blacklist_path = blacklist_path;
    string line="";
    if(this->blacklist_path!=""){
        //load blacklist from blacklist_path
        ifstream blacklist_file(this->blacklist_path);
        //insert seen
        while (getline(blacklist_file, line)) {
            Strip(line);
            this->seen.insert(line);
        }
    }
    if(store_blacklist)     this->blacklist=this->seen;
}

/***destructor***/
LSHDeduplicator::~LSHDeduplicator()
{
    if(this->store_blacklist){
        // output blacklist
        ofstream blacklist_file(this->blacklist_path);
        for(auto lsh: this->blacklist)   blacklist_file<<lsh<<endl;
    }
}

/**
 * @brief Calculate minhash list of text
 * @details 
 * Duplication is determined based on the hash value generated by `GenerateDedupLSH`.
 * If the target corpus is approximately 10^6 or less (~several 10 GB is a rough guide), 
 * duplicate processing is possible without prior processing.
 * If you want to perform deduplication without preprocessing (online), set the `online_dedup` flag to `True`. 
 * The guaranteed value for `online_dedup` is `True`.
 * 
 * For larger corpora, it becomes difficult to store hash values ​​of all documents in memory. 
 * Duplicate documents are considered to be a few percent of all documents, so
 * By reading only hash values ​​from a file as a blacklist, it is possible to process a corpus of several 100 GB.
 * 
 * Reads duplicate hash values ​​from the file specified by `blacklist_path`. blacklist files every other line
 * Assume that the hash value generated by `GenerateDedupLSH` is recorded.
 * When the `store_blacklist` flag is set to `True`, 
 * duplicate hash values ​​will be recorded as a set of strings in the `LSHDeduplicator.blacklist` attribute.
 * This option is useful, for example, when creating a blacklist hash value file. 
 * The default value of the `store_blacklist` flag is `False`.
 * @example
 *  GenerateDedupLSH generate_dedup_lsh;
 *  d1 = generate_dedup_lsh.CalculateLSH("Hello, World.");
 *  d2 = generate_dedup_lsh.CalculateLSH("吾輩は猫である。名前はまだ無い。どこで生まれたかとんと見当がつかぬ。");
 *  d3 = generate_dedup_lsh.CalculateLSH("吾輩は鳥である。名前はまだ無い。どこで生まれたかとんと見当がつかぬ。");
 *  LSHDeduplicator deduplicator;
 *  cout << deduplicator.apply(d1) << endl;
 *  //false
 *  cout << deduplicator.apply(d1) << endl;
 *  //false
 *  cout << deduplicator.apply(d1) << endl;
 *  //true
 * @param vector<string> *lshs: lshs list generated by GenerateDedupLSH
 * @return bool : duplication(true), or not duplication(false)
 * @ref 
 * https://github.com/HojiChar/HojiChar/blob/v0.9.0/hojichar/filters/deduplication.py
 * @attention 
**/
bool LSHDeduplicator::Apply(const vector<string> *lshs)
{
    if(lshs->size() == 0){
        cout << "LSHs for deduplication are not caluculated. Filter \
                `GenerateDedupLSH` must be composed before this filter." << endl;
        assert(false);
        return false;
    }

    bool is_duplicated=false;

    for(auto lsh:*lshs){
        // if lsh in this->seen, 
        if (this->seen.find(lsh) != this->seen.end()){
            is_duplicated = true;
            if(this->store_blacklist)   this->blacklist.insert(lsh);
        }
        if(this->online_dedup)  this->seen.insert(lsh);
    }

    return is_duplicated; 

}