#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <vector>
#ifdef WIN32
#include "../scripts/kenlm/util/getopt.hh"
#else
#include <unistd.h>
#endif
#include <bits/stdc++.h>

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
#include "../scripts/sentencepiece/src/sentencepiece_processor.h"


using namespace std;

class KenLMFilter
{
public:
	sentencepiece::SentencePieceProcessor processor;
		
    KenLMFilter();
    double Score(const wstring sentence);
    double ScoreWithSentencePiece(const wstring sentence);
    double Perplexity(const wstring sentence);
    double PerplexityWithSentencePiece(const wstring sentence);
};