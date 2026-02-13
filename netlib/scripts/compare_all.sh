#!/bin/bash

NAMES=$(ls $1)
S_DIR=$(dirname "${BASH_SOURCE[0]}")

for n in $NAMES; do
    echo $n
    python $S_DIR/compare.py $1/$n $2/$n
done
