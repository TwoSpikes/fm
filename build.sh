#!/bin/sh

cmake -B build ./
cmake --build ./build/
mv ./build/fm $PREFIX/bin/
