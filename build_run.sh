#!/bin/bash

set -e

mkdir -p build
cd build
cmake ..
cmake --build .
cp ../config.csv .

echo "Running binary"
./modem
