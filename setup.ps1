#!/bin/sh
rm -r -fo build
$env:CC=clang
$env:CXX=clang++
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -T ClangCL -B build
