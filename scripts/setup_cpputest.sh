#!/bin/bash

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

# install cpputest
cd tests
wget https://github.com/cpputest/cpputest/releases/download/v4.0/cpputest-4.0.tar.gz
tar xvfzp cpputest-4.0.tar.gz
rm -r cpputest-4.0.tar.gz
cd cpputest-4.0
cd build
cmake ..
make 
make install
cd ../scripts
chmod +x NewProject.sh
cd ../../
# 
echo "export CPPUTEST_HOME=~/corpus-cleaner-cpp/tests/cpputest-4.0" >> ~/.bash_profile
echo "export PATH=$PATH:~/corpus-cleaner-cpp/tests/cpputest-4.0/scripts" >> ~/.bash_profile

source ~/.bash_profile

mkdir -p ~/corpus-cleaner-cpp/tests/cpputest-4.0/lib/
cp /usr/local/lib/libCppUTest.a ~/corpus-cleaner-cpp/tests/cpputest-4.0/lib/
cp /usr/local/lib/libCppUTestExt.a ~/corpus-cleaner-cpp/tests/cpputest-4.0/lib/

cd ../

