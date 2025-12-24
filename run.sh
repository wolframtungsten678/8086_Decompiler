#!/bin/bash
if [ $# -lt 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi
g++ Decompiler.cpp -o decompiler
./decompiler $1
