#! /bin/bash

#SBATCH -J hogan
#SBATCH -o ./out/output.o%j
#SBATCH -n 1
#SBATCH -N 1
#SBATCH -p shortq
#SBATCH -t 00:02:00

export OMP_NUM_THREADS=1
module load gcc/10.2.0
module load cmake/gcc/3.18.0
rm -rf build
mkdir build
cd build
cmake ..
make
pwd
./matrixmatrix ../etc/2by3matrix.mtx ../etc/3by2matrix.mtx ../etc/r2testoutmm.mtx
./matrixvector ../etc/ck104.mtx ../etc/vector104.mtx ../etc/r2testoutmv.mtx
./montecarlo 1000
./fibonacci 10 s
./leibniz 1000
