#! /bin/bash

#SBATCH -J groupc
#SBATCH -o ./output/leibniz_output.o
#SBATCH -n 9
#SBATCH -N 1
#SBATCH -p defq
#SBATCH -t 00:02:00


module load gcc/10.2.0
module load cmake/gcc/3.18.0
module load openmpi/gcc/64/1.10.7

cd build
rm -rf *
cmake ..
make

mpirun ./leibniz 1000