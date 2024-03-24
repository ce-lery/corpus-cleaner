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

using namespace std;

double KenLMScore(const wstring sentence);
double KenLMPerplexity(const wstring sentence);
