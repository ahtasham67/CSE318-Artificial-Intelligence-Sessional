#!/bin/bash

g++ -std=c++17 generateReport.cpp -o program
./program set1 result.csv 
