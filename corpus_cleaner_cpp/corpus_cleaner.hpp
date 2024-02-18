#include <regex>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>

using namespace std;
namespace fs = filesystem;

class CorpusCleaner
{
private:
    /***member value***/
    string input_path;
    string intermediate_path;
    string output_path;
    uint32_t min_length=5;
    uint32_t max_length=1000;
    //TODO: add vecter of result's file size of each cleaning process. At the end, analysys it.

public:
    /***constructor***/
    CorpusCleaner(string input_path,
                  string output_path,
                  uint32_t min_length,
                  uint32_t max_length);
    /***deconstructor***/
    //TODO: remove intermediate_file
    ~CorpusCleaner();
    /***member function***/
    double ExcessFilter(string input_path, string output_path);
    double URLRemover(string input_path, string output_path);

    double CleanPipeline();
};