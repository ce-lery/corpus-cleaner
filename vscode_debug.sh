
ICUPATH=$PWD/scripts/icu/usr/local
echo $ICUPATH
export C_INCLUDE_PATH=$ICUPATH/include
export CPLUS_INCLUDE_PATH=$ICUPATH/include
export LIBRARY_PATH=$ICUPATH/lib
export LD_LIBRARY_PATH=$ICUPATH/lib

cmake -S ./corpus_cleaner -B ./corpus_cleaner/build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./corpus_cleaner/build