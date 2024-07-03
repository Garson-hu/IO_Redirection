#!/bin/bash

# Compiles and runs hello-ld-preload

# if ! make clean; then
#     echo "Failed to clean"
#     exit 1
# fi

if ! make; then
    echo "Failed to compile"
    exit 1
fi

LD_PRELOAD=./myfopen.so ./hello-ld-preload
