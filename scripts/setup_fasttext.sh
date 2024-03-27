cd scripts
git clone https://github.com/facebookresearch/fastText.git
cd fastText
mkdir build && cd build && cmake ..
make && make install
cd ../../../

cd corpus_cleaner
wget https://dl.fbaipublicfiles.com/fasttext/supervised-models/lid.176.bin

mkdir -p ../tests/build/
cp lid.176.bin ../tests/build/

cd ../