#! /bin/bash

#SBATCH -J hogan
#SBATCH -o ./output/perf_cannon_output.o
#SBATCH -n 28
#SBATCH -N 1
#SBATCH -p defq
#SBATCH -t 00:45:00


module load gcc/10.2.0
module load cmake/gcc/3.18.0
module load openmpi/gcc/64/1.10.7

cd build
rm -rf *
cmake ..
make

rm ../out/perf/cannons/cannons.txt
touch ../out/perf/cannons/cannons.txt

# 4800 by 4800
echo -ne "size\t4800\tprocs\t25\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 25 ./matrixmatrixcannon  4800  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t4800\tprocs\t16\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 16 ./matrixmatrixcannon  4800  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t4800\tprocs\t9\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 9 ./matrixmatrixcannon  4800  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t4800\tprocs\t4\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 4 ./matrixmatrixcannon  4800  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t4800\tprocs\t1\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 1 ./matrixmatrixcannon  4800  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt

# 240 by 240
echo -ne "size\t240\tprocs\t1\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 1 ./matrixmatrixcannon  240  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t240\tprocs\t4\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 4 ./matrixmatrixcannon  240  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t240\tprocs\t9\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 9 ./matrixmatrixcannon  240  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t240\tprocs\t16\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 16 ./matrixmatrixcannon  240  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t240\tprocs\t25\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 25 ./matrixmatrixcannon  240  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt

# 1600 by 1600
echo -ne "size\t1600\tprocs\t1\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 1 ./matrixmatrixcannon  1600  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t1600\tprocs\t4\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 4 ./matrixmatrixcannon  1600  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t1599\tprocs\t9\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 9 ./matrixmatrixcannon  1599  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t1600\tprocs\t16\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 16 ./matrixmatrixcannon  1600  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
echo -ne "size\t1600\tprocs\t25\t" >> ../out/perf/cannons/cannons.txt
mpirun -n 25 ./matrixmatrixcannon  1600  ../out/cannon_out.mtx >> ../out/perf/cannons/cannons.txt
