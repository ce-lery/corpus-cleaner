#include "perplexity_filter.hh"


using namespace std;
// using namespace ;
lm::ngram::Model model("ja.arpa.bin"); //TODO: Refactor. Don't use global parameter.	
const lm::ngram::Vocabulary &vocab = model.GetVocabulary();

/**
 * @brief Score sentence by KenLM.
 * @details
 * The step is...  
 * 1. Split sentence into single characters.  
 * 2.   
 * 
 * Example:
 * ```cpp
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";  
 *    cout << KenLMScore(sentence) << endl;  
 *    // -60.5849  
 * ```
 * @param const wstring &sentence: text sentence
 * @return double: score by KenLM
 * @note
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
 * Example:
 * ```cpp
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";  
 *    cout << KenLMScore(sentence) << endl;  
 *    // -60.5849
 * ```
 * @param const wstring &sentence: text sentence
 * @return double: score by KenLM
 * @ref
 * https://github.com/google/sentencepiece/blob/master/doc/api.md
 * https://github.com/google/sentencepiece
 * @note
**/
double KenLMFilter::Score(const wstring sentence)
{
	double total_score=0,score=0;
	
	string word_w ="";

	lm::ngram::State state(model.BeginSentenceState()), out_state;
	for (int i=0;i<(int)sentence.size();i++) {
		// Split sentence into single characters.
		wstring word_w = sentence.substr(i,1);
		string word = ConvertWstringToUTF8(word_w);
		// cout << word << endl;
		score = model.BaseScore(&state, vocab.Index(word), &out_state);
		// cout << score << endl;
		total_score += score;
		state = out_state;
  	}
	//eos
	score = model.BaseScore(&state, vocab.EndSentence(), &out_state);
	total_score += score;

  	return total_score;
}

/**
 * @brief Score sentence by KenLM with SentencePiece Tokenizing.
 * @details
 * The step is...  
 * 1. Split sentence into single characters.  
 * 2.  
 * 
 * Example:
 * ```cpp
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";
 *    cout << KenLMScore(sentence) << endl;
 *	  //
 * ```
 * @param const wstring &sentence: text sentence
 * @return double: score by KenLM
 * @ref
 * https://github.com/google/sentencepiece/blob/master/doc/api.md
 * https://github.com/google/sentencepiece
 * @note
**/
double KenLMFilter::ScoreWithSentencePiece(const wstring sentence)
{

	double total_score=0,score=0;
	
	string word_w ="";
	vector<string> pieces;
	processor.Encode(ConvertWstringToUTF8(sentence), &pieces);

	lm::ngram::State state(model.BeginSentenceState()), out_state;
	for (auto piece:pieces) {
		//string word = pieces[i];
		// cout << piece << endl;
		score = model.BaseScore(&state, vocab.Index(piece), &out_state);
		// cout << score << endl;
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
 * Example:
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";
 *    cout << KenLMPerplexity(sentence) << endl;
 *    // 4117.1
 * @param const string& src: text sentence
 * @return double: score by KenLM
 * @ref 
 * https://github.com/kpu/kenlm/blob/master/python/kenlm.pyx#L209
 * https://zenn.dev/syoyo/articles/529ce949121ca4
 * https://github.com/facebookresearch/cc_net
 * @note
**/
double KenLMFilter::Perplexity(const wstring sentence)
{
	//words = len(as_str(sentence).split()) + 1 // For </s>
	double words = (double)(sentence.size()+1);

	//cout << "sentence.size:"<<words<<endl;
	return pow(10.0,(-this->Score(sentence) / words));
}

/**
 * @brief Perplexity sentence by KenLM with SentencePiece Tokenizing.
 * @details
 * The step is...
 * 	1. Split sentence into token by SentencePiece.
 * 	2. Calculate perplexity value.
 * 
 * The usage is following.
 * 
 *    wstring sentence = L"吾輩は猫である.名前はまだない.";
 *    cout << PerplexityWithSentencePiece(sentence) << endl;
 *    // 677.5
 * @param const string& src: text sentence
 * @return double: score by KenLM
 * @ref 
 * https://github.com/kpu/kenlm/blob/master/python/kenlm.pyx#L209
 * https://zenn.dev/syoyo/articles/529ce949121ca4
 * https://github.com/facebookresearch/cc_net
 * @note
**/
double KenLMFilter::PerplexityWithSentencePiece(const wstring sentence)
{
	vector<string> pieces;
	processor.Encode(ConvertWstringToUTF8(sentence), &pieces);
	string sentence_tokenized = "";
	//for (auto piece:pieces) sentence_tokenized += piece+" ";
	//sentence_tokenized.pop_back();
	//cout << sentence_tokenized << endl;
	
	double words = (double)pieces.size()+1;
	//cout << "sentence.size:"<<words<<endl;
	return pow(10.0,(-this->ScoreWithSentencePiece(sentence) / words));
}

