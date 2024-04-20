#include <gtest/gtest.h>
#include "../corpus_cleaner/corpus_cleaner.hpp"
#include "../corpus_cleaner/util.hpp"
#include "../corpus_cleaner/normalizer.hpp"

// namespace {

class CorpusCleanerTest : public ::testing::Test {
protected:
    // You can freely delete empty functions in the following functions.
    uint32_t min_length=5;
    uint32_t max_length = 1000;
    set<string> accept_language{"__label__ja"};
    Document document;
    string sentence="";
    const string output_path = "../data/output/";
    const string intermediate_path = "../data/intermediate/"; 
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
        mkdir(output_path.c_str(), 0777);
        mkdir(intermediate_path.c_str(), 0777);
    }

    virtual void TearDown() {
        // This code is placed immediately after each test (just before the destructor)
        // will be called.
        RemoveFolder(output_path);
        RemoveFolder(intermediate_path);
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

TEST_F(CorpusCleanerTest, LengthFilter) {
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);

    document.is_rejected=false;
    for(int i=0;i<1001;i++) sentence+="あ";
    document.text = sentence;
    corpus_cleaner.LengthFilter(document);
    ASSERT_TRUE(document.text == sentence);
    ASSERT_TRUE(document.is_rejected==true);

    document.is_rejected=false;
    document.text = "あ。";
    corpus_cleaner.LengthFilter(document);
    ASSERT_TRUE(document.text == "あ。");
    ASSERT_TRUE(document.is_rejected==true);

    document.text = "AAA。";
    document.is_rejected=false;
    corpus_cleaner.LengthFilter(document);
    ASSERT_TRUE(document.text == "AAA。");
    ASSERT_TRUE(document.is_rejected==true);

    document.text = "こんにちは。こんにちは。こんにちは。";
    document.is_rejected=false;
    corpus_cleaner.LengthFilter(document);
    ASSERT_TRUE(document.text == "こんにちは。こんにちは。こんにちは。");
    ASSERT_TRUE(document.is_rejected==false);

    document.text = "こんばんわ。";
    document.is_rejected=false;
    corpus_cleaner.LengthFilter(document);
    ASSERT_TRUE(document.text == "こんばんわ。");
    ASSERT_TRUE(document.is_rejected==false);
}


TEST_F(CorpusCleanerTest, ZeroPunctuationFilter) {
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);

    document.is_rejected=false;
    document.text = "東京　大阪　名古屋　横浜";
    corpus_cleaner.ZeroPunctuationFilter(document);
    ASSERT_TRUE(document.is_rejected==true);

    document.is_rejected=false;
    document.text = "こんにちわ。";
    corpus_cleaner.ZeroPunctuationFilter(document);
    ASSERT_TRUE(document.is_rejected==false);
    
    document.is_rejected=false;
    document.text = "1972年5月：大韓航空がソウル線で就航 ";
    corpus_cleaner.ZeroPunctuationFilter(document);
    ASSERT_TRUE(document.is_rejected==true);    
    
    document.is_rejected=false;
    document.text = "「勝てばよかろうなのだァァァァッ!!」";
    corpus_cleaner.ZeroPunctuationFilter(document);
    ASSERT_TRUE(document.is_rejected==false);

    document.is_rejected=false;
    document.text = "「おまえは……自分が『悪』だと気づいていない…もっともドス黒い『悪』だ」";
    corpus_cleaner.ZeroPunctuationFilter(document);
    ASSERT_TRUE(document.is_rejected==true);    

    document.is_rejected=false;
    document.text = "https://github.com/";
    corpus_cleaner.ZeroPunctuationFilter(document);
    ASSERT_TRUE(document.is_rejected==false);
}

TEST_F(CorpusCleanerTest, URLRemover) {
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    document.is_rejected=false;
    document.text = "https://qiita.com/これはqiitaのURLです";
    corpus_cleaner.URLRemover(document);
    ASSERT_TRUE(document.text == "これはqiitaのURLです");
    ASSERT_TRUE(document.is_rejected==false);
    ASSERT_TRUE(document.metadata.find("URLRemover")!=document.metadata.end());

    document.text = "これはzennのURLですhttps://zenn.dev/";
    corpus_cleaner.URLRemover(document);
    ASSERT_TRUE(document.text == "これはzennのURLです");

    document.text = "https://zenn.dev/https://qiita.com/これはqiitaとzennのURLです";
    corpus_cleaner.URLRemover(document);
    ASSERT_TRUE(document.text == "これはqiitaとzennのURLです");

    document.text = "https://zenn.dev/あhttps://qiita.com/いhttps://huggingface.co/う";
    corpus_cleaner.URLRemover(document);
    ASSERT_TRUE(document.text == "あいう");

    document.text = "URLに日本語が含まれる場合https://www.google.com/search?q=URL+%E6%97%A5%E6%9C%AC%E8%AA%9E&oq=URL+%E6%97%A5%E6%9C%AC%E8%AA%9E&aqs=chrome..69i57.3480j0j7&sourceid=chrome&ie=UTF-8";
    corpus_cleaner.URLRemover(document);
    ASSERT_TRUE(document.text == "URLに日本語が含まれる場合");
}

TEST_F(CorpusCleanerTest, MakeStats) {
    string input_path = "../data/input/test_URLRemover.txt";
    double elapsed_time = 11.56;
    string process_name = "URLRemover";
    Stats stats = MakeStats(process_name,input_path,elapsed_time);
    ASSERT_EQ("URLRemover",stats.process_name);
    ASSERT_EQ("",stats.file_name);
    ASSERT_EQ(elapsed_time,stats.elapsed_time);
    ASSERT_EQ(0,stats.result_file_size);
}

TEST_F(CorpusCleanerTest, SpecialCharacterRemover) {
    uint32_t min_length = 10;
    uint32_t max_length = 1000;
    set<string> accept_language{"__label__ja"};
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
                                 
    Document document;  
    document.text = "☀あ←い⌚う⤲え⭐お🀀";
    cout << document.text << endl;
    document.language = "";
    document.language_score=0;
    corpus_cleaner.SpecialCharacterRemover(document);
    ASSERT_TRUE(document.text == "あいうえお");
    ASSERT_TRUE(document.is_rejected == false);
    ASSERT_TRUE(document.metadata.find("SpecialCharacterRemover") != document.metadata.end());
    ASSERT_TRUE(document.language == "");       
    //ASSERT_TRUE(document.language_score == 0);
    
    document.text = "そろそろ狩るか…♠";
    corpus_cleaner.SpecialCharacterRemover(document);
    ASSERT_TRUE(document.text == "そろそろ狩るか…");
    
    document.text = "RTX4090最高☆☀☁♠";
    corpus_cleaner.SpecialCharacterRemover(document);
    ASSERT_TRUE(document.text == "RTX4090最高");
     
    document.text = "お腹が空きました☹";
    corpus_cleaner.SpecialCharacterRemover(document);
    ASSERT_TRUE(document.text == "お腹が空きました");
     
    document.text = "境界値シリーズ☀⟿←⇿⌀⏿⤀⥿⬀⯿🀀🃿";
    corpus_cleaner.SpecialCharacterRemover(document);
    ASSERT_TRUE(document.text == "境界値シリーズ");
}

TEST_F(CorpusCleanerTest, EmojiRemover) {
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    document.text = "よろしくお願いします😎";
    document.language = "";
    document.language_score=0;
    corpus_cleaner.EmojiRemover(document);
    ASSERT_TRUE(document.text == "よろしくお願いします");
    ASSERT_TRUE(document.is_rejected == false);
    ASSERT_TRUE(document.metadata.find("EmojiRemover") != document.metadata.end());
    ASSERT_TRUE(document.language == "");       
    
    document.text = "こんにちは😗よろしくお願いします🤡🤡";
    corpus_cleaner.EmojiRemover(document);
    ASSERT_TRUE(document.text == "こんにちはよろしくお願いします");
    
    
    document.text = "🤗お🤗は🤗よ🤗う🤗";
    corpus_cleaner.EmojiRemover(document);
    ASSERT_TRUE(document.text == "おはよう");
    
    document.text = "境界値１🌀";
    corpus_cleaner.EmojiRemover(document);
    ASSERT_TRUE(document.text == "境界値１");

    document.text = "境界値２🧿";
    corpus_cleaner.EmojiRemover(document);
    ASSERT_TRUE(document.text == "境界値２"); 
}

TEST_F(CorpusCleanerTest, SentenceSegmenter) {
    string input_folder_path = "../data/input/sentence_segment/";
    string output_folder_path = "../data/output/sentence_segment/";
    string intermediate_folder_path = "../data/output/sentence_segment/intermediate/";
    string answer_folder_path = "../data/answer/sentence_segment/";

    RemoveFolder(intermediate_folder_path);
    RemoveFolder(output_folder_path);
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner(input_folder_path,
                                 output_folder_path,
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);

    // MoveFolder(output_folder_path+"/cleaned/", output_folder_path+"/intermediate/"); 
    // cout << "MoveFolder Completed" << endl;

    corpus_cleaner.SentenceSegmenter( output_folder_path+"/intermediate/",output_folder_path+"/cleaned/");
    cout << "SentenceSegmentation Completed" << endl;
    vector<string> file_list;
    GetFileNameListWithoutExtention(answer_folder_path,&file_list);
    for (int i=0;i<(int)file_list.size();i++){
        ASSERT_TRUE(CompareFiles(output_folder_path+"/cleaned/"+file_list[i]+".jsonl",answer_folder_path+"/"+file_list[i]+".jsonl"));
    }
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

    // cout << minhash<<endl;
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

    LSHDeduplicator deduplicator(true,"",true,5120);
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

    LSHDeduplicator deduplicator(true,"",true,5120);
    ASSERT_TRUE(deduplicator.Apply(&d1)==false);
    ASSERT_TRUE(deduplicator.Apply(&d2)==true);

    //cout << deduplicator.Apply(&d1) << endl;
    //false
    //cout << deduplicator.Apply(&d2) << endl;
    //false
}


TEST_F(CorpusCleanerTest, MinhashDeduplication) {
    string input_folder_path = "../data/input/sentence_deduplicate/";
    string output_folder_path = "../data/output/minhash/";
    string intermediate_folder_path = "../data/output/minhash/intermediate/";
    string answer_folder_path = "../data/answer/minhash";

    RemoveFolder(intermediate_folder_path);
    RemoveFolder(output_folder_path);
    mkdir(output_folder_path.c_str(), 0777);

    GenerateDedupLSH generate_dedup_lsh(4,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/minhash/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner(input_folder_path,
                                 output_folder_path,
                                 min_length,
                                 max_length,
                                 accept_language,
                                 false,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);

    mkdir(intermediate_folder_path.c_str(), 0777);
    // mkdir(intermediate_folder_path.c_str(), 0777);
    // MoveFolder(output_folder_path+"/cleaned/", output_folder_path+"/intermediate/"); 

    vector<string> filename_list;
    GetFileNameListWithoutExtention(output_folder_path+"/intermediate/",&filename_list);
    // Execute the each CorpusCleaner processing on all files in the intermediate folder.
    for (auto filename: filename_list){
        // load data
        ifstream input_file(output_folder_path+"/intermediate/"+"/"+filename+".txt");
        string  output_file_path(output_folder_path+"/cleaned/"+"/"+filename+".jsonl");
        string line="";
        uint64_t line_count=0;
        while (getline(input_file, line)) {
            Document document;
            ConvertTextToDocument(line,filename,to_string(line_count),document);
            // Loop processing as many times as cleaner_list
            corpus_cleaner.MinhashDeduplication(document);
            // dump data
            WriteDocumentToJsonl(document,output_file_path);
            line_count++;
        }
        input_file.close();   
    }

    vector<string> file_list;
    GetFileNameListWithoutExtention(answer_folder_path,&file_list);
    for (int i=0;i<(int)file_list.size();i++){
        ASSERT_TRUE(CompareFiles(output_folder_path+"/cleaned/"+"/"+file_list[i]+".jsonl",answer_folder_path+"/"+file_list[i]+".jsonl"));
    }
}

TEST_F(CorpusCleanerTest,LanguageFilter) 
{
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    document.text = "吾輩は猫である。名前はまだ無い。";
    corpus_cleaner.LanguageFilter(document);
    //document.text isn't changed.
    ASSERT_TRUE(document.text == "吾輩は猫である。名前はまだ無い。"); 
    ASSERT_TRUE(document.is_rejected==false);
    ASSERT_TRUE(document.metadata.find("LanguageFilter")==document.metadata.end());

    document.text = "I am a cat. No name yet.";
    corpus_cleaner.LanguageFilter(document);
    ASSERT_TRUE(document.language=="__label__en");
    ASSERT_TRUE(document.is_rejected==true);
    ASSERT_TRUE(document.metadata.find("LanguageFilter")!=document.metadata.end());
}

TEST_F(CorpusCleanerTest,LanguageFilter2) 
{
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 {"__label__en"},
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    document.text = "I am a cat. No name yet.";
    corpus_cleaner.LanguageFilter(document);
    ASSERT_TRUE(document.language=="__label__en");
    ASSERT_TRUE(document.is_rejected==false);

    //under.threshold
    document.text = "ぎぎgugu";
    corpus_cleaner.LanguageFilter(document);
    ASSERT_TRUE(document.language=="__label__en");
    ASSERT_TRUE(document.language_score<0.3);
    ASSERT_TRUE(document.is_rejected==true);
}

TEST_F(CorpusCleanerTest,QuotesRemover) 
{
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    document.text = "自己教師あり学習または半教師あり学習（英語版）によって訓練が行われる[1]。";
    corpus_cleaner.QuotesRemover(document);
    ASSERT_TRUE(document.text
                =="自己教師あり学習または半教師あり学習（英語版）によって訓練が行われる。");

    document.text = "自己教師あり学習または半教師あり学習（英語版）によって訓練が行われる{2}。";
    corpus_cleaner.QuotesRemover(document);
    ASSERT_TRUE(document.text
                =="自己教師あり学習または半教師あり学習（英語版）によって訓練が行われる。");

    document.text = "これは文献[123]{456}を参照ください。";
    corpus_cleaner.QuotesRemover(document);
    ASSERT_TRUE(document.text
                =="これは文献を参照ください。");

    document.text = "これは文献[a]を参照ください。";
    corpus_cleaner.QuotesRemover(document);
    ASSERT_TRUE(document.text
                =="これは文献[a]を参照ください。");
}

TEST_F(CorpusCleanerTest,KenLMPerplexity) 
{
    vector<wstring> sentence_list;
	sentence_list.push_back(L"東京はッ晴れ");
	sentence_list.push_back(L"東京は元気です");
	sentence_list.push_back(L"吾輩は猫である.名前はまだない.");
	sentence_list.push_back(L"東京は晴れ");
	//sentence_list.push_back(L"東京 大阪 名古屋 秋田 千葉");
	//sentence_list.push_back(L"あああああああ");
	//sentence_list.push_back(L"assdofiuslkあｓｋｄｈｊｋ");

    vector<double> perplexity_list;
    KenLMFilter kenlm_filter;
    for (wstring sentence:sentence_list) {
        perplexity_list.push_back(kenlm_filter.Perplexity(sentence));
        cout << ConvertWstringToUTF8(sentence) << "perplexity:"<<perplexity_list[(int)perplexity_list.size()-1]<<endl;
	}
    // cout << perplexity_list[6]<<endl;
    //ref: https://google.github.io/googletest/reference/assertions.html
    EXPECT_NEAR(perplexity_list[0],21879.531940405483,0.1);//東京はッ晴れ
    EXPECT_NEAR(perplexity_list[1],24128.70574300623,0.1);//東京は元気です
    EXPECT_NEAR(perplexity_list[2],4117.138960278464,0.1);//吾輩は猫である。名前はまだない。
    EXPECT_NEAR(perplexity_list[3],17809.198147089162,0.1);//東京は晴れ
    //ASSERT_TRUE(perplexity_list[4]>15000);//東京 大阪 名古屋 秋田 千葉
    //ASSERT_TRUE(perplexity_list[5]>15000);//あああああああ
    //ASSERT_TRUE(perplexity_list[6]>15000);//assdofiuslkあｓｋｄｈｊｋ

    ASSERT_TRUE(perplexity_list[0]>perplexity_list[3]);//東京はッ晴れ>東京は晴れ
    ASSERT_TRUE(perplexity_list[1]>perplexity_list[3]);//東京は元気です>東京は晴れ
}

TEST_F(CorpusCleanerTest,KenLMPerplexityWithSentencePiece) 
{
    vector<wstring> sentence_list;
	sentence_list.push_back(L"東京はッ晴れ");
	sentence_list.push_back(L"東京は元気です");
	sentence_list.push_back(L"吾輩は猫である. 名前はまだない.");
	sentence_list.push_back(L"東京は晴れ");
	//sentence_list.push_back(L"東京 大阪 名古屋 秋田 千葉");
	//sentence_list.push_back(L"あああああああ");
	//sentence_list.push_back(L"assdofiuslkあｓｋｄｈｊｋ");

    vector<double> perplexity_list;
    KenLMFilter kenlm_filter;
    for (wstring sentence:sentence_list) {
        perplexity_list.push_back(kenlm_filter.PerplexityWithSentencePiece(sentence));
        cout << ConvertWstringToUTF8(sentence) << "perplexity:"<<perplexity_list[(int)perplexity_list.size()-1]<<endl;
	}
    // cout << perplexity_list[6]<<endl;
    //ref: https://google.github.io/googletest/reference/assertions.html
    EXPECT_NEAR(perplexity_list[0],10808.575564708846,0.1);//東京はッ晴れ
    EXPECT_NEAR(perplexity_list[1],14207.90466675276,0.1);//東京は元気です
    EXPECT_NEAR(perplexity_list[2],677.481230499526,0.1);//吾輩は猫である。名前はまだない。
    EXPECT_NEAR(perplexity_list[3],3340.487952615284,0.1);//東京は晴れ
    //ASSERT_TRUE(perplexity_list[4]>15000);//東京 大阪 名古屋 秋田 千葉
    //ASSERT_TRUE(perplexity_list[5]>15000);//あああああああ
    //ASSERT_TRUE(perplexity_list[6]>15000);//assdofiuslkあｓｋｄｈｊｋ

    ASSERT_TRUE(perplexity_list[0]>perplexity_list[3]);//東京はッ晴れ>東京は晴れ
    ASSERT_TRUE(perplexity_list[1]>perplexity_list[3]);//東京は元気です>東京は晴れ
}

TEST_F(CorpusCleanerTest,PerplexityFilter) 
{

    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    Document document;
    document.text = "東京はッ晴れ";
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity<=15000);
    ASSERT_TRUE(document.is_rejected==false);
    ASSERT_TRUE(document.metadata.find("PerplexityFilter")==document.metadata.end());

    document.text = "東京は元気です";
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity<=15000);
    ASSERT_TRUE(document.is_rejected==false);

    document.text = "吾輩は猫である。名前はまだない。";
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity<=15000);
    ASSERT_TRUE(document.is_rejected==false);

    document.text = "東京は晴れ";
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity<=15000);
    ASSERT_TRUE(document.is_rejected==false);

    document.text = "東京 大阪 名古屋 秋田 千葉";
    document.is_rejected=false;
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity>15000);
    ASSERT_TRUE(document.is_rejected==true);
    ASSERT_TRUE(document.metadata.find("PerplexityFilter")!=document.metadata.end());

    document.text = "あああああああ";
    document.is_rejected=false;
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity<15000);
    ASSERT_TRUE(document.is_rejected==false);

    document.text = "assdofiuslkあｓｋｄｈｊｋ";
    document.is_rejected=false;
    corpus_cleaner.PerplexityFilter(document);
    ASSERT_TRUE(document.perplexity<15000);
    ASSERT_TRUE(document.is_rejected==false);


}

TEST_F(CorpusCleanerTest,GetFileNameListWithoutExtention) 
{
    string input_path = "../data/input/sentence_deduplicate";
    vector<string> file_name_list;
    GetFileNameListWithoutExtention(input_path, &file_name_list);
    // cout << file_name_list[0] << endl;
    ASSERT_TRUE(file_name_list[0]=="test_SentenceDeduplication2");
    // cout << file_name_list[1] << endl;
    ASSERT_TRUE(file_name_list[1]=="test_SentenceDeduplication3");
    // cout << file_name_list[2] << endl;
    ASSERT_TRUE(file_name_list[2]=="test_SentenceDeduplication");
}


TEST_F(CorpusCleanerTest,ConvertTextToDocument) 
{
    Document document;
    string sentence = "こんにちは。私はceleryです。";
    string filename = "input";
    string file_line_count = "0";
    ConvertTextToDocument(sentence,filename,file_line_count,document);
    ASSERT_TRUE(document.text==sentence);
    ASSERT_TRUE(document.id=="input_0");
    ASSERT_TRUE(document.is_rejected==false);
    ASSERT_TRUE(document.language=="");
    ASSERT_TRUE(document.language_score==-1);
    ASSERT_TRUE(document.perplexity==-1);
}

TEST_F(CorpusCleanerTest,WriteDocumentToJsonl) 
{
    Document document;
    string sentence = "こんにちは。私はceleryです。";
    string filename = "input";
    string file_line_count = "0";
    ConvertTextToDocument(sentence,filename,file_line_count,document);
    if (fs::exists("../data/output/write_document_to_jsonl.jsonl")) {
        fs::remove("../data/output/write_document_to_jsonl.jsonl");
    }

    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    WriteDocumentToJsonl(document,"../data/output/write_document_to_jsonl.jsonl");
    
    document.text = "いいかい! もっとも『むずかしい事』は! 『自分を乗り越える事』さ!";
    document.id  = "rohan_0"; 
    document.is_rejected = true;
    // document.metadata;
    document.language="__label__ja";
    document.language_score=0.003;
    document.perplexity=1.692;
    WriteDocumentToJsonl(document,"../data/output/write_document_to_jsonl.jsonl");
    
    ifstream input("../data/output/write_document_to_jsonl.jsonl");
    string line;
    getline(input,line);
    // cout << "{\"text\":\"こんにちは。私はceleryです。\",\"id\":\"input_0\",\"is_rejected\":\"0\",\"metadata\":\"\",\"language\":\"\",\"language_score\":\"-1\",\"perplexity\":\"-1\"}" << endl;
    // cout << line << endl;
    ASSERT_TRUE(line==string("{\"text\":\"こんにちは。私はceleryです。\",\"id\":\"input_0\",\"is_rejected\":\"0\",\"metadata\":\"\",\"language\":\"\",\"language_score\":\"-1\",\"perplexity\":\"-1\"}")); 
    getline(input,line);
    ASSERT_TRUE(line=="{\"text\":\"いいかい! もっとも『むずかしい事』は! 『自分を乗り越える事』さ!\",\"id\":\"rohan_0\",\"is_rejected\":\"1\",\"metadata\":\"\",\"language\":\"__label__ja\",\"language_score\":\"0.003\",\"perplexity\":\"1.692\"}" );    
    input.close();
}

TEST_F(CorpusCleanerTest,ReadDocumentFromJsonlOneLine) 
{
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    Document document;
    string jsonl_line = "{\"text\":\"『覚悟』とは!! 暗闇の荒野に!! 進むべき道を切り開く事だッ!\",\"id\":\"jorno_0\",\"is_rejected\":\"1\",\"metadata\":\"\",\"language\":\"__label__ja\",\"language_score\":\"0.003\",\"perplexity\":\"1.692\"}";
    
    ReadDocumentFromJsonlOneLine(document,jsonl_line);
    ASSERT_TRUE(document.text=="『覚悟』とは!! 暗闇の荒野に!! 進むべき道を切り開く事だッ!");
    ASSERT_TRUE(document.id=="jorno_0");
    ASSERT_TRUE(document.is_rejected==true);
    ASSERT_TRUE(document.language=="__label__ja");
    EXPECT_NEAR(document.language_score,0.003,0.0001);
    EXPECT_NEAR(document.perplexity,1.692,0.0001);    
}

TEST_F(CorpusCleanerTest,ExceptionReadDocumentFromJsonlOneLine)
{
    GenerateDedupLSH generate_dedup_lsh(5,200,20,10);
    LSHDeduplicator deduplicator(true,"../data/output/blacklist.txt",true,5120);
    CorpusCleaner corpus_cleaner("../data/input/",
                                 "../data/output/",
                                 min_length,
                                 max_length,
                                 accept_language,
                                 true,
                                 true,
                                 0.3,
                                 15000,
                                 &generate_dedup_lsh,
                                 &deduplicator);
    Document document;
    // jsonl_line without data
    string jsonl_line = "{}";
    EXPECT_ANY_THROW(ReadDocumentFromJsonlOneLine(document,jsonl_line));
}