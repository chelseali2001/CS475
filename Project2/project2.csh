#!/bin/bash

#number of threads:
for t in 1 2 4 8 12 16 20 24 32 
do
  for n in 100 250 500 750 1000 2500 5000 7500 10000
  do
    g++ -O3   project2.cpp  -DNUMT=$t -DNUMNODES=$n  -o proj2  -lm  -fopenmp
    ./proj2
  done
done