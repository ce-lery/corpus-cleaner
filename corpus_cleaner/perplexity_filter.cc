#include "perplexity_filter.hh"

#include "../scripts/kenlm/lm/model.hh"
#include "../scripts/kenlm/lm/sizes.hh"
#include "../scripts/kenlm/util/file_piece.hh"
#include "../scripts/kenlm/util/usage.hh"

#ifdef WIN32
#include "../scripts/kenlm/util/getopt.hh"
#else
#include <unistd.h>
#endif
#include <omp.h>
#include "util.hpp"

using namespace std;
using namespace lm::ngram;
const static Model model("ja.arpa.bin"); //TODO: Refactor. Don't use global parameter.

/**
 * @brief Score sentence by KenLM.
 * @details
 * The step is...
 * 1. Split sentence into single characters.
 * 2. 
 * 
 * The usage is following.
 * 
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";
 *    cout << KenLMScore(sentence) << endl;
 *    // -60.5849
 * @param const wstring &sentence: text sentence
 * @return double: score by KenLM
 * @attention
**/
double KenLMScore(const wstring sentence)
{

  double total_score=0,score=0;
  
  wstring word_w =L"";
  // cout << ConvertWstringToUTF8(sentence) << endl;
	State state(model.BeginSentenceState()), out_state;
	const Vocabulary &vocab = model.GetVocabulary();
  for (int i=0;i<(int)sentence.size();i++) {
    // Split sentence into single characters.
    word_w=sentence.substr(i,1);
    string word=ConvertWstringToUTF8(word_w);
    // cout << word << endl;
    score=model.Score(state, vocab.Index(word), out_state);
    // cout << score << endl;
    total_score += score;
    state = out_state;
  }
  return total_score;
}

/**
 * @brief Perplexity sentence by KenLM.
 * @details
 * The step is...
 * 1. Split sentence into single characters.
 * 2. 
 * 
 * The usage is following.
 * 
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";
 *    cout << KenLMPerplexity(sentence) << endl;
 *    // -60.5849
 * @param const string& src: text sentence
 * @return double: score by KenLM
 * @ref 
 * https://github.com/kpu/kenlm/blob/master/python/kenlm.pyx#L209
 * https://zenn.dev/syoyo/articles/529ce949121ca4
 * https://github.com/facebookresearch/cc_net
 * @attention
**/
double KenLMPerplexity(const wstring sentence)
{
	// words = len(as_str(sentence).split()) + 1 // For </s>
	double words = (double)(sentence.size()+1);
	// cout << "sentence.size:"<<sentence.size()<<endl;
	return pow(10.0,(-KenLMScore(sentence) / words));
}

/*
int main() {
	vector<wstring> sentence_list;
	sentence_list.push_back(L"東京はッ晴れ");
	sentence_list.push_back(L"東京は元気です");
	sentence_list.push_back(L"吾輩は猫である。名前はまだない。");
	sentence_list.push_back(L"東京は晴れ");
	sentence_list.push_back(L"東京 大阪 名古屋 秋田 千葉");
	sentence_list.push_back(L"あああああああ");
	chrono::system_clock::time_point start, end;
	start = chrono::system_clock::now();

	#pragma omp parallel
	{
	#pragma omp for nowait ordered
	for (wstring sentence:sentence_list) {
		#pragma omp ordered 
		{
		// cout << ConvertWstringToUTF8(sentence) <<endl;
		// cout << ConvertWstringToUTF8(sentence) << KenLMScore(sentence) <<endl;
		
		cout << ConvertWstringToUTF8(sentence) <<" perplexity:"<<KenLMPerplexity(sentence)<<endl;}
	}
	}

	end = chrono::system_clock::now(); 
	double elapsed = chrono::duration_cast<chrono::duration<double>>(end - start).count(); 
	cout << "passed time[s]"<< elapsed <<endl;

	return 0;
}
*/
