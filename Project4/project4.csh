#!/bin/bash

for t in 1000 10000 100000 1000000 2000000 3000000 4000000 5000000 6000000 7000000 8000000
do
  g++ all04.cpp  -DARRAYSIZE=$t -o proj4  -lm  -fopenmp
  ./proj4
done