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
touch ../out/perf/mm_perf_out_small
mpirun ./cmatrixmatrixcannon  12  ../out/cannon_out.mtx
#mpirun -n 2 ./matrixmatrixcannon 240 ../out/cannon_out.mtx >> ../out/perf/mm_perf_out_small
#mpirun -n 4 ./matrixmatrixcannon  240 ../out/cannon_out.mtx >> ../out/perf/mm_perf_out_small
#
#mpirun -n 16 ./matrixmatrixcannon  240 ../out/cannon_out.mtx >> ../out/perf/mm_perf_out_small
#mpirun -n 25 ./matrixmatrixcannon  225 ../out/cannon_out.mtx >> ../out/perf/mm_perf_out_small
#mpirun -n 36 ./matrixmatrixcannon  216 ../out/cannon_out.mtx >> ../out/perf/mm_perf_out_small
