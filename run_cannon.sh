#! /bin/bash

#SBATCH -J hogan
#SBATCH -o ./output/cannon_output.o%j
#SBATCH -n 25
#SBATCH -N 1
#SBATCH -p defq
#SBATCH -t 00:02:00

module load gcc/10.2.0
module load openmpi/gcc/64/1.10.7

cd build
rm -rf *
cmake ..
make

mpirun ./matrixmatrixcannon 75 ../out/cannon_out.mtx

