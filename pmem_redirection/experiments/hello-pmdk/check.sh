#!/bin/bash

set -e

n=3
N=(42 1337 777)
WORKFLOW_FILE=numbers.txt

for ((i=0;i<n;i++))
do
	grep "Data\[$i\]: ${N[$i]}" $WORKFLOW_FILE
done

echo "CHECK SUCCESS"
