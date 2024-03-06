#include <gtest/gtest.h>
#include "../corpus_cleaner_cpp/corpus_cleaner.hpp"
#include "../corpus_cleaner_cpp/util.hpp"
#include "../corpus_cleaner_cpp/normalizer.hpp"
#include "../corpus_cleaner_cpp/minhash.hpp"

// namespace {

class CorpusCleanerTest : public ::testing::Test {
protected:
    // You can freely delete empty functions in the following functions.

    CorpusCleanerTest() {
        // Write the setup that will be executed for each test here.
    }

    virtual ~CorpusCleanerTest() {
        // Write here the clean-up that will be executed 
        //after each test and will not throw an exception.
    }

    // When constructor and destructor are not enough.
    // You can define the following methods:

    virtual void SetUp() {
        // This code is right after the constructor (just before each test)
        // will be called.
        string output_path = "../data/output/";
        mkdir(output_path.c_str(), 0777);
    }

    virtual void TearDown() {
        // This code is placed immediately after each test (just before the destructor)
        // will be called.
        // string output_path = "../data/output/";
        // RemoveFolder(output_path);
    }

    // Objects declared here are available to all tests within the test case.
    //string a;
    //uin32_t b;
};

// } //namespace

bool CompareFiles(const string& file1, const string& file2) {
    ifstream f1(file1, ios::binary);
    ifstream f2(file2, ios::binary);

    //compare file size
    if(filesystem::file_size(file1)!=filesystem::file_size(file2))  return false;

    // compare file contents
    if (f1 && f2) {
        char c1, c2;
        while (f1.get(c1) && f2.get(c2)) {
            if (c1 != c2)   return false; //file1!=file2
        }
        return true; //file1==file2
    } 
    else    return false; //can't open file, or can't find file.
}

TEST_F(CorpusCleanerTest, Excessfilter) {
    string input_path = "../data/input/test_ExcessFilter.txt";
    string output_path = "../data/output/test_ExcessFilter.txt";
    string answer_path = "../data/answer/test_ExcessFilter.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.ExcessFilter(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}


TEST_F(CorpusCleanerTest, URLRemover) {
    string input_path = "../data/input/test_URLRemover.txt";
    string output_path = "../data/output/test_URLRemover.txt";
    string answer_path = "../data/answer/test_URLRemover.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.URLRemover(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}

TEST_F(CorpusCleanerTest, MakeStats) {
    string input_path = "../data/input/test_URLRemover.txt";
    double elapsed_time = 11.56;
    string process_name = "URLRemover";
    Stats stats = MakeStats(process_name,input_path,elapsed_time);
    ASSERT_EQ("URLRemover",stats.process_name);
    ASSERT_EQ("test_URLRemover.txt",stats.file_name);
    ASSERT_EQ(elapsed_time,stats.elapsed_time);
    ASSERT_EQ(filesystem::file_size(input_path),stats.result_file_size);
}

TEST_F(CorpusCleanerTest, SpecialCharacterRemover) {
    string input_path = "../data/input/test_SpecialCharacterRemover.txt";
    string output_path = "../data/output/test_SpecialCharacterRemover.txt";
    string answer_path = "../data/answer/test_SpecialCharacterRemover.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.SpecialCharacterRemover(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}

TEST_F(CorpusCleanerTest, EmojiRemover) {
    string input_path = "../data/input/test_EmojiRemover.txt";
    string output_path = "../data/output/test_EmojiRemover.txt";
    string answer_path = "../data/answer/test_EmojiRemover.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.EmojiRemover(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}

TEST_F(CorpusCleanerTest, SentenceDeduplication) {
    string input_folder_path = "../data/input/sentence_deduplicate";
    string output_folder_path = "../data/output/sentence_deduplicate";
    string answer_folder_path = "../data/answer/sentence_deduplicate";

    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner(input_folder_path,output_folder_path,min_length,max_length);
    corpus_cleaner.SentenceDeduplication(input_folder_path,output_folder_path);

    vector<string> file_list;
    GetFileList(answer_folder_path,&file_list);
    for (int i=0;i<(int)file_list.size();i++){
        ASSERT_TRUE(CompareFiles(output_folder_path+"/"+file_list[i],answer_folder_path+"/"+file_list[i]));
    }

}

TEST_F(CorpusCleanerTest, SentenceSegmenter) {
    string input_path = "../data/input/test_SentenceSegmentater.txt";
    string output_path = "../data/output/test_SentenceSegmentater.txt";
    string answer_path = "../data/answer/test_SentenceSegmentater.txt";
    uint32_t min_length=10;
    uint32_t max_length = 1000;
    CorpusCleaner corpus_cleaner("../data/input/","../data/output/",min_length,max_length);
    corpus_cleaner.SentenceSegmenter(input_path,output_path);
    ASSERT_TRUE(CompareFiles(output_path,answer_path));
}

TEST_F(CorpusCleanerTest, Normalizer) {
    //original
    ASSERT_TRUE("Hello,C++!" == NormalizeNeologd("   Hello, C++!   "));// TODO: Write the comment that this normalizer is don't applied for English text. Because spaces are removed.
    ASSERT_TRUE("-" == NormalizeNeologd("˗֊‐‑‒–⁃⁻₋−"));
    ASSERT_TRUE("-" == NormalizeNeologd("－"));
    ASSERT_TRUE("ー" == NormalizeNeologd("﹣—―─━ーｰ"));
    ASSERT_TRUE("＝" == NormalizeNeologd("="));
    ASSERT_TRUE("0123456789" == NormalizeNeologd("０１２３４５６７８９"));
    ASSERT_TRUE("ABCDEFGHIJKLMNOPQRSTUVWXYZ" == NormalizeNeologd("ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"));
    ASSERT_TRUE("abcdefghijklmnopqrstuvwxyz" == NormalizeNeologd("ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"));
    ASSERT_TRUE("!\"#$%&\'()*+,-./:;<>?@[¥]^_`{|}" == NormalizeNeologd("！”＃＄％＆’（）＊＋，－．／：；＜＞？＠［￥］＾＿｀｛｜｝"));
    ASSERT_TRUE("＝。、・「」" == NormalizeNeologd("＝。、・「」"));
    ASSERT_TRUE("ハンカク" == NormalizeNeologd("ﾊﾝｶｸ"));
    ASSERT_TRUE("o-o" == NormalizeNeologd("o₋o"));
    ASSERT_TRUE("majikaー" == NormalizeNeologd("majika━"));
    ASSERT_TRUE("わい" == NormalizeNeologd("わ〰い"));
    ASSERT_TRUE("スーパー" == NormalizeNeologd("スーパーーーー"));
    ASSERT_TRUE("!#" == NormalizeNeologd("!#"));
    ASSERT_TRUE("ゼンカクスペース" == NormalizeNeologd("ゼンカク　スペース"));
    ASSERT_TRUE("おお" == NormalizeNeologd("お             お"));
    ASSERT_TRUE("おお" == NormalizeNeologd("      おお"));
    ASSERT_TRUE("おお" == NormalizeNeologd("おお      "));
    ASSERT_TRUE("検索エンジン自作入門を買いました!!!" ==NormalizeNeologd("検索 エンジン 自作 入門 を 買い ました!!!"));
    ASSERT_TRUE("アルゴリズムC" == NormalizeNeologd("アルゴリズム C"));
    ASSERT_TRUE("PRML副読本" == NormalizeNeologd("　　　ＰＲＭＬ　　副　読　本　　　"));
    ASSERT_TRUE("Coding the Matrix" == NormalizeNeologd("Coding the Matrix"));
    ASSERT_TRUE("南アルプスの天然水Sparking Lemonレモン一絞り" == NormalizeNeologd("南アルプスの　天然水　Ｓｐａｒｋｉｎｇ　Ｌｅｍｏｎ　レモン一絞り"));
    ASSERT_TRUE("南アルプスの天然水-Sparking*Lemon+レモン一絞り" == NormalizeNeologd("南アルプスの　天然水-　Ｓｐａｒｋｉｎｇ*　Ｌｅｍｏｎ+　レモン一絞り"));
	// cout << "Normalizing Text is completed." << endl;
}

TEST_F(CorpusCleanerTest, NGramTokenize) {
    GenerateDedupLSH generate_dedupe_lsh(3,200,20,10);

    wstring text = L"おはようございます。";
    vector<wstring> ret = generate_dedupe_lsh.NGramTokenize(text, 3);
    for(int i=0;i<(int)ret.size()-3+1;i++)  ASSERT_TRUE(ret[i]==text.substr(i,3));
    ASSERT_TRUE((int)ret.size()==8);

    text = L"おはよ";
    ret = generate_dedupe_lsh.NGramTokenize(text, 5);
    ASSERT_TRUE(ret[0]==L"おはよ");
    ASSERT_TRUE((int)ret.size()==1);

    text = L"おはよう🤗ございます。Huggingface";
    ret = generate_dedupe_lsh.NGramTokenize(text, 3);
    for(int i=0;i<(int)ret.size()-3+1;i++)  ASSERT_TRUE(ret[i]==text.substr(i,3));
    ASSERT_TRUE((int)ret.size()==20);
}

TEST_F(CorpusCleanerTest, GetMinhash) {
    GenerateDedupLSH generate_dedupe_lsh(3,200,20,10);

    wstring text = L"おはようございます。";
    vector<wstring> tokens = generate_dedupe_lsh.NGramTokenize(text, 3);
    uint64_t minhash = generate_dedupe_lsh.GetMinhash(&tokens,0);

    cout << minhash<<endl;
    ASSERT_TRUE(minhash==5643264886837621032);
}


TEST_F(CorpusCleanerTest, LSHDeduplicator1) {
    // refer: https://github.com/HojiChar/HojiChar/blob/v0.9.0/tests/filters/test_lsh_deduplication.py

    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    vector<string> d1 = generate_dedup_lsh.CalculateLSH(L"吾輩は猫である。名前はまだ無い。どこで生まれたかとんと見当がつかぬ。");
    vector<string> d2 = generate_dedup_lsh.CalculateLSH(L"吾輩は鳥である。名前はまだ無い。どこで生まれたかとんと見当がつかぬ。");
    vector<string> d3 = generate_dedup_lsh.CalculateLSH(L"祇園精舎の鐘の声、諸行無常の響きあり。");

    // cout<<"d1:"<<endl;
    // cout<<"[" <<endl;
    // for(auto lsh:d1) cout << lsh <<endl;
    // cout<<"]" <<endl;

    // cout<<"d2:"<<endl;
    // cout<<"[" <<endl;
    // for(auto lsh:d2) cout << lsh <<endl;
    // cout<<"]" <<endl;

    // cout<<"d3:"<<endl;
    // cout<<"[" <<endl;
    // for(auto lsh:d3) cout << lsh <<endl;
    // cout<<"]" <<endl;

    LSHDeduplicator deduplicator(true,"",true);
    ASSERT_TRUE(deduplicator.Apply(&d1)==false);
    ASSERT_TRUE(deduplicator.Apply(&d2)==true);
    ASSERT_TRUE(deduplicator.Apply(&d3)==false);

}

TEST_F(CorpusCleanerTest, LSHDeduplicator2) {
    // refer: https://github.com/HojiChar/HojiChar/blob/v0.9.0/tests/filters/test_lsh_deduplication.py
    
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    vector<string> d1 = generate_dedup_lsh.CalculateLSH(L"吾輩は猫です。🤗名前はまだ無い。");
    vector<string> d2 = generate_dedup_lsh.CalculateLSH(L"吾輩は猫です。🤗名前はまだ無いです。");

    // cout<<"d1:"<<endl;
    // cout<<"[" <<endl;
    // for(auto lsh:d1) cout << lsh <<endl;
    // cout<<"]" <<endl;

    // cout<<"d2:"<<endl;
    // cout<<"[" <<endl;
    // for(auto lsh:d2) cout << lsh <<endl;
    // cout<<"]" <<endl;

    LSHDeduplicator deduplicator(true,"",true);
    ASSERT_TRUE(deduplicator.Apply(&d1)==false);
    ASSERT_TRUE(deduplicator.Apply(&d2)==true);

    //cout << deduplicator.Apply(&d1) << endl;
    //false
    //cout << deduplicator.Apply(&d2) << endl;
    //false

}


