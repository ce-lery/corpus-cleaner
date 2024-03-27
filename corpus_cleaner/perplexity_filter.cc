#include "perplexity_filter.hh"


using namespace std;
// using namespace ;
lm::ngram::Model model("ja.arpa.bin"); //TODO: Refactor. Don't use global parameter.	



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
 * 
 * @attention
**/
KenLMFilter::KenLMFilter()
{

	const auto status = processor.Load("ja.sp.model");
	if (!status.ok()) {
   		std::cerr << status.ToString() << std::endl;
   		// error
	}
}


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
 * @ref
 * https://github.com/google/sentencepiece/blob/master/doc/api.md
 * https://github.com/google/sentencepiece
 * @attention
**/
double KenLMFilter::Score(const wstring sentence)
{

	double total_score=0,score=0;
	
	string word_w ="";
	// cout << ConvertWstringToUTF8(sentence) << endl;
	
	// vector<string> pieces;
	// processor.Encode(ConvertWstringToUTF8(sentence), &pieces);
	// string sentence_tokenized = "";
	// for (auto piece:pieces) sentence_tokenized += " "+piece;
	// cout << sentence_tokenized << endl;
	// wstring sentence_w = ConvertUTF8ToWstring(sentence_tokenized);
	wstring sentence_w = sentence;
	lm::ngram::State state(model.BeginSentenceState()), out_state;
	const lm::ngram::Vocabulary &vocab = model.GetVocabulary();
	for (int i=0;i<(int)sentence_w.size();i++) {
		// Split sentence into single characters.
		wstring word_w=sentence_w.substr(i,1);
		string word=ConvertWstringToUTF8(word_w);
		cout << word << endl;
		score=model.BaseScore(&state, vocab.Index(word), &out_state);
		cout << score << endl;
		total_score += score;
		state = out_state;
  	}
	//eos
	score=model.BaseScore(&state, vocab.EndSentence(), &out_state);
	total_score += score;

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
double KenLMFilter::Perplexity(const wstring sentence)
{
	// vector<string> pieces;
	// processor.Encode(ConvertWstringToUTF8(sentence), &pieces);
	// string sentence_tokenized = "";
	// for (auto piece:pieces) sentence_tokenized += piece+" ";
	// sentence_tokenized.pop_back();
	// cout << sentence_tokenized << endl;
	// wstring sentence_w = ConvertUTF8ToWstring(sentence_tokenized);
	wstring sentence_w = sentence;

	// words = len(as_str(sentence).split()) + 1 // For </s>
	double words = (double)(sentence_w.size()+1);
	cout << "sentence.size:"<<sentence_w.size()<<endl;
	return pow(10.0,(-this->Score(sentence_w) / words));
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