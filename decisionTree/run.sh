#!/bin/bash
g++ -std=c++17 -O2 main.cpp -o main
CRITERIA=("IG" "IGR" "NWIG")
DEPTHS=(0 1 2 3)

for criterion in "${CRITERIA[@]}"; do
    for depth in "${DEPTHS[@]}"; do
        ./main $criterion $depth
    done
done
