#include "language_filter.hpp"

/**
 * @brief Judge the language of one sentence
 * @details 
 * @example
 * @param string sentence: sentence Judged
 * @param vector<pair<real, string>>& predictions: Pointer for returning judgment results
 *  @param int32_t k: Judge one sentence(1), judge multiple sentences(-1)
 * @param real threshold: threshold for return
 * @return None : minhash
 * @ref https://fasttext.cc/docs/en/supervised-tutorial.html
 * @attention
**/
void LanguageFilter::predictOneLine(string sentence,
                             vector<pair<real, string>>& predictions,
                             int32_t k,
                             real threshold) const
{
    predictions.clear();

    vector<int32_t> words, labels;
    dict_->getStringNoNewline(sentence, words, labels);
    Predictions linePredictions;
    this->predict(k, words, linePredictions, threshold);
    for (const auto& p : linePredictions) {
        predictions.push_back(
        make_pair(exp(p.first), dict_->getLabel(p.second)));
    }
    return;
}

/**
 * @brief Judge the language of one sentence
 * @details 
 * @example
 *  string in = "吾輩は猫である。名前はまだ無い。";
 *  LanguageFilter language_filter;
 *  pair<float, string> score;
 *  score = language_filter.filter(in);
 *  // score.first ==1.00005, score.second ==__label__ja
 * 
 *  string in2 = "I am a cat. No name yet.";
 *  score = language_filter.filter(in2);
 *  // score.first ==0.75237, score.second ==__label__en
 * @param string sentence: sentence judged
 * @return pair<float, string> : float:　Language assessment score, 
 * string: Language determination results
 * @ref https://fasttext.cc/docs/en/supervised-tutorial.html
 * @attention
**/
pair<float, string> LanguageFilter::filter(string sentence) 
{
    int32_t k = 1;
    real threshold = 0.0;
    LanguageFilter fasttext;
    fasttext.loadModel("lid.176.bin");

    vector<pair<real, string>> predictions;
    fasttext.predictOneLine(sentence, predictions, k, threshold);

    return make_pair((float)predictions[0].first,predictions[0].second);
}
