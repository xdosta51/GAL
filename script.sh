#!/bin/bash

# bash script.sh GRAF_SIZE_MIN GRAF_SIZE_MAX GRAF_VARIANT_MIN GRAF_VARIANT_MAX COUNT THREADS ALGORITHMS PATH

for j in $(seq $3 $4)
do
    for i in $(seq $1 $2)
    do
        bin/GAL -i "${8}${i}_${j}_Graph" -c $5 -n $6 -o -$7
    done
done