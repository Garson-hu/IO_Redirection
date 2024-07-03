#!/usr/bin/env bash

set -e

if ! make
then
    echo "FAILED to compile"
    exit 1
fi

test()
{
    # ./main | tee actual.txt \
	# && LD_PRELOAD=./myfopen.so ./main | tee -a actual.txt
    LD_PRELOAD=./myfopen.so ./main | tee -a actual.txt
}

if ! test
then
    echo "FAILED to test"
    exit 1
fi

if ! diff expected.txt actual.txt
then
    echo "FAILED files differ"
    exit 1
fi

echo "IO Redirection SUCCESS"
