#!/usr/bin/env bash

# Install dependencies
sudo apt update -y
sudo apt install build-essential cmake -y

mkdir build
cd build
cmake ..
make
cp ../test_files/* .
./jix_tests
