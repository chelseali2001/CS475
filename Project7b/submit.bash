#!/bin/bash
#SBATCH -J Proj7b
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH -N 8 # number of nodes
#SBATCH -n 8 # number of tasks
#SBATCH -o proj7b.out
#SBATCH -e proj7b.err
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=lichel@cs.oregonstate.edu
module load openmpi/3.1
mpic++ project7b.cpp -o proj7b -lm

for n in 1 2 4 8 12
do
  mpiexec -mca btl self,tcp -np $n proj7b
  ./proj7b
done