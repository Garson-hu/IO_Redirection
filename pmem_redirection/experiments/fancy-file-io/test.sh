#!/bin/bash

# Compiles and runs fancy-file-io

# if ! make clean; then
#     echo "Failed to clean"
#     exit 1
# fi

if ! make; then
    echo "Failed to compile"
    exit 1
fi

rm -f hi
echo "libc:"
time {
    ./fancy-file-io;
}

rm -f hi
echo -e "\nLD_PRELOAD:"
time {
    LD_PRELOAD=./myio.so ./fancy-file-io;
}
