#include <bits/stdc++.h>

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
public:
    /***constructor***/
    GenerateDedupLSH();
    /***destructor***/
    ~GenerateDedupLSH();
    vector<wstring>  NGramTokenize(wstring text, int32_t n);
    // int32_t HashfuncSigned32FromSeed(int32_t seed);
    int32_t GetMinhash(int32_t seed);
    wstring CalculateLSH(wstring text);
    wstring Apply(wstring document);//TODO
};

class LSHDeduplicator
{
private:

public:
    /***constructor***/
    LSHDeduplicator();
    /***destructor***/
    ~LSHDeduplicator();
    wstring Apply(wstring document);//TODO
};
/*
ドキュメントの重複判定に使用可能なハッシュ値を生成します。
ハッシュ値は20個生成され、類似する(≒編集距離が近い)文章どうしのハッシュが似る性質を持ちます(Locality Sensitive Hashing)。
2つの文書間で少なくとも1つのハッシュ値が一致する場合に重複として判定することができます。
生成されたハッシュは `Document.dedup_lsh` 属性に文字列のリストとして保存されます。
重複処理を実施する場合は、本フィルタを `hojichar.filters.deduplication.LSHDeduplicator` の前に適用します。
*/