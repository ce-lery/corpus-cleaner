#include <iomanip>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <vector>
#include <string>

#include "../scripts/fastText/src/args.h"
#include "../scripts/fastText/src/autotune.h"
#include "../scripts/fastText/src/fasttext.h"
#include "../scripts/fastText/src/dictionary.h"
#include "../scripts/fastText/src/real.h"

using namespace fasttext;
using namespace std;
namespace fasttext {

class FastTextEx: public FastText
{
private:
public:
    FastTextEx();
    void predictOneLine(string sentence,
                     vector<pair<real, string>>& predictions,
                     int32_t k,
                     real threshold) const;
};
}
