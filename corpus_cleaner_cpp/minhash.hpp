#include <bits/stdc++.h>

#define INF 1L<<30;
using namespace std;

/**
 * @brief Deduplication with Minhash LSH
 * @details 
 * @example
 *   string input_path = "../data/input/test_URLRemover.txt";
 *   double elapsed_time = 11.56;
 *   string process_name = "URLRemover";
 *   Stats stats = MakeStats(process_name,input_path,elapsed_time);
 * @param string process_name: Cleaning filter name.
 * @param string output_path: Path of file for statistics.
 * @param double elapsed_time: elapsed process time.
 * @return Stats: statistics
 * @ref https://github.com/HojiChar/HojiChar/blob/v0.9.0/hojichar/filters/deduplication.py
 * @attention 
**/
class GenerateDedupLSH
{
private:
    uint32_t n_gram = 5;
    uint32_t n_minhash = 200;
    uint32_t n_buckets=20;
    uint32_t bucket_size=10;
public:
    /***constructor***/
    GenerateDedupLSH(uint32_t n_gram,
                     uint32_t n_minhash,
                     uint32_t n_buckets,
                     uint32_t bucket_size);
    /***destructor***/
    ~GenerateDedupLSH();
    vector<wstring>  NGramTokenize(wstring text, int32_t n);
    // int32_t HashfuncSigned32FromSeed(int32_t seed);
    uint64_t GetMinhash(vector<wstring> *tokens,uint32_t seed);
    vector<string> CalculateLSH(wstring text);
    // wstring Apply(wstring document);//TODO
};

class LSHDeduplicator
{
private:
    bool online_dedup;
    string blacklist_path;
    bool store_blacklist;
    set<string> seen,blacklist; // TODO: unordered_set is faster than set.
public:
    /***constructor***/
    LSHDeduplicator(bool onlin_dedupe,
                    string blacklist_path,
                    bool store_blacklist);
    /***destructor***/
    ~LSHDeduplicator();
    bool Apply(const vector<string> *lshs);//TODO
};
/*
ドキュメントの重複判定に使用可能なハッシュ値を生成します。
ハッシュ値は20個生成され、類似する(≒編集距離が近い)文章どうしのハッシュが似る性質を持ちます(Locality Sensitive Hashing)。
2つの文書間で少なくとも1つのハッシュ値が一致する場合に重複として判定することができます。
生成されたハッシュは `Document.dedup_lsh` 属性に文字列のリストとして保存されます。
重複処理を実施する場合は、本フィルタを `hojichar.filters.deduplication.LSHDeduplicator` の前に適用します。
*/