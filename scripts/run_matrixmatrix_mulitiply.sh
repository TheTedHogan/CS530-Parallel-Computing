#! /bin/bash

#SBATCH -J hogan
#SBATCH -o ./output/cannon_output.o
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

mpirun ./matrixmatrixcannon  ../etc/3by2matrix.mtx ../etc/2by3matrix.mtx  ../out/matrixmatrix__out.mtx