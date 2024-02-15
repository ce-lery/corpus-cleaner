#include <bits/stdc++.h>
using namespace std;

/**
 * @brief Remove too long sentence and too short sentence.
 * @details 
 *  Remove too long sentence that is length is more thanand too short sentence.
 *  The length of too long sentence is more than "max_length".
 *  The length of too short sentence is lesser than "min_length".
 * @example
 *   string input_path="wiki_train.txt";
 *   string output_path = "wiki_train2.txt";
 *   uint32_t min_length=10;
 *   uint32_t max_lemgth = 1000;
 *   ExcessFilter(input_path,output_path,min_length,max_lemgth);
 * @param string input_path: The path of filterd file.
 * @param string output_path: The output path of results file.
 * @param uint32_t min_length=5: Minimum threshold to count as 1 sentence.
 * @param uint32_t max_length=1000: Maximum threshold to count as 1 sentence.
 * @return None
 * @attention 
**/
void ExcessFilter(string input_path, 
                  string output_path, 
                  uint32_t min_length=5, 
                  uint32_t max_length=1000)
{
    ifstream input_file(input_path);
    ofstream output_file(output_path);
    string line="";

    while (getline(input_file, line)) {
        uint32_t line_length = line.size();
        if (min_length <= line_length && line_length <= max_length) {
            output_file << line << endl;
        }
    }
    input_file.close();
    output_file.close();
    cout << "Excess filtering is completed." << endl;
}

int main(void)
{
    string input_path="wiki_train.txt";
    string output_path = "wiki_train2.txt";
    uint32_t min_length=10;
    uint32_t max_lemgth = 1000;
    ExcessFilter(input_path,output_path,min_length,max_lemgth);
    return 0;
}