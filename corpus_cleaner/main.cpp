/*
#include "corpus_cleaner.cpp"
#include "util.cpp"

int main(void)
{
    string input_path = "../data/";
    string output_path = "../results/urlremover_test/";
    CorpusCleaner corpus_cleaner(input_path,output_path,5,1000);
    // uint32_t min_length=10;
    // uint32_t max_lemgth = 1000;
    // ExcessFilter(input_path,output_path,min_length,max_lemgth);
    corpus_cleaner.CleanPipeline();
    return 0;
}
*/

#include "minhash.cpp"
#include "util.cpp"
#include "../scripts/smhasher/src/MurmurHash3.cpp"

using namespace std;

int main(void)
{
   GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
   vector<string> d1 = generate_dedup_lsh.CalculateLSH(L"私はアイアンマンです。");
   vector<string> d2 = generate_dedup_lsh.CalculateLSH(L"吾輩は猫です。🤗名前はまだ無い");
   vector<string> d3 = generate_dedup_lsh.CalculateLSH(L"吾輩は鳥ですよ。🤗名前はまだ無い。");
   vector<string> d4 = generate_dedup_lsh.CalculateLSH(L"私はアイアンマンですよ。");


   cout<<"d1:"<<endl;
   cout<<"[" <<endl;
   for(auto lsh:d1) cout << lsh <<endl;
   cout<<"]" <<endl;


   cout<<"d2:"<<endl;
   cout<<"[" <<endl;
   for(auto lsh:d2) cout << lsh <<endl;
   cout<<"]" <<endl;

   cout<<"d3:"<<endl;
   cout<<"[" <<endl;
   for(auto lsh:d3) cout << lsh <<endl;
   cout<<"]" <<endl;

   cout<<"d4:"<<endl;
   cout<<"[" <<endl;
   for(auto lsh:d4) cout << lsh <<endl;
   cout<<"]" <<endl;

   LSHDeduplicator deduplicator;
   cout << deduplicator.Apply(&d1) << endl;
   //false
   cout << deduplicator.Apply(&d2) << endl;
   //false
   cout << deduplicator.Apply(&d3) << endl;
   //true
   cout << deduplicator.Apply(&d4) << endl;
   //true

   return 0;
}