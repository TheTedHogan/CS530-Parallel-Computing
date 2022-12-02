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

mpirun ./matrixmatrixcannon  12  ../out/cannon_out.mtx