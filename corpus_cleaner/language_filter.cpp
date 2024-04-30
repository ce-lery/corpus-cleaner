#include "language_filter.hpp"


FastTextEx::FastTextEx()
{
    //FastTextEx fasttextex; fasttextex.loadModel("lid.176.bin");
    this->loadModel("lid.176.bin");
}

/**
 * @brief Judge the language of one sentence
 * @details 
 * Example:
 * @param string sentence: sentence Judged
 * @param vector<pair<real, string>>& predictions: Pointer for returning judgment results
 *  @param int32_t k: Judge one sentence(1), judge multiple sentences(-1)
 * @param real threshold: threshold for return
 * @return None : minhash
 * @ref https://fasttext.cc/docs/en/supervised-tutorial.html
 * @attention
**/
void FastTextEx::predictOneLine(string sentence,
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
