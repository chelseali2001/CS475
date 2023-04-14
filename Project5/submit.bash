#!/bin/bash
#SBATCH -J montecarlo
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --constraint=v100
#SBATCH --gres=gpu:1
#SBATCH -o montecarlo.out
#SBATCH -e montecarlo.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=lichel@oregonstate.edu

for t in 8 32 128
do
  for n in 1024 4096 16384 65536 262144 1048576 2097152 4194304
  do
    /usr/local/apps/cuda/cuda-10.1/bin/nvcc -DBLOCKSIZE=$t -DNUMTRIALS=$n -o montecarlo montecarlo.cu
    ./montecarlo
  done
done