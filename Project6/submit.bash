#!/bin/bash
#SBATCH -J MatrixMult
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o matrixmul.out
#SBATCH -e matrixmul.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=lichel@oregonstate.edu

printf "ArrayMult\n"
for t in 1 2 4 8
do
  for n in 8 16 32 64 128 256 512
  do
    g++ -o first -DNMB=$t -DLOCAL_SIZE=$n first.cpp /usr/local/apps/cuda/10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
    ./first
  done
done

printf "ArrayMultAdd\n"
for t in 1 2 4 8
do
  for n in 8 16 32 64 128 256 512
  do
    g++ -o second -DNMB=$t -DLOCAL_SIZE=$n second.cpp /usr/local/apps/cuda/10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
    ./second
  done
done

printf "ArrayMultReduce\n"
for t in 1 2 4 8
do
  for n in 32 64 128 256
  do
    g++ -o third -DNMB=$t -DLOCAL_SIZE=$n third.cpp /usr/local/apps/cuda/10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
    ./third
  done
done