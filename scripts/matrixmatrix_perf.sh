#! /bin/bash

#SBATCH -J groupc_mm
#SBATCH -o ./output/perf_mm_output.o
#SBATCH -n 28
#SBATCH -N 1
#SBATCH -p defq
#SBATCH -t 03:45:00


module load gcc/10.2.0
module load cmake/gcc/3.18.0
module load openmpi/gcc/64/1.10.7

cd build
#rm -rf *
#cmake ..
#make

rm ../out/perf/matrixmatrix/matrixmatrix.txt
touch ../out/perf/matrixmatrix/matrixmatrix.txt

#for i in {2..28}
#do
#echo -ne "size\t240\tprocs\t" >> ../out/perf/matrixmatrix/matrixmatrix.txt
#echo -ne $i  >> ../out/perf/matrixmatrix/matrixmatrix.txt
#echo -ne "\t"  >> ../out/perf/matrixmatrix/matrixmatrix.txt
#mpirun -n $i ./matrixmatrix ../etc/240by240.mtx ../etc/240by240.mtx ../out/240mmout.mtx >> ../out/perf/matrixmatrix/matrixmatrix.txt
#done
#
#for i in {2..28}
#do
#echo -ne "size\t1600\tprocs\t" >> ../out/perf/matrixmatrix/matrixmatrix.txt
#echo -ne $i  >> ../out/perf/matrixmatrix/matrixmatrix.txt
#echo -ne "\t"  >> ../out/perf/matrixmatrix/matrixmatrix.txt
#mpirun -n $i ./matrixmatrix ../etc/1600by1600.mtx ../etc/1600by1600.mtx ../out/1600mmout.mtx >> ../out/perf/matrixmatrix/matrixmatrix.txt
#done

for i in {3..28}
do
echo -ne "size\t4800\tprocs\t" >> ../out/perf/matrixmatrix/matrixmatrix.txt
echo -ne $i  >> ../out/perf/matrixmatrix/matrixmatrix.txt
echo -ne "\t"  >> ../out/perf/matrixmatrix/matrixmatrix.txt
mpirun -n $i ./matrixmatrix ../etc/4800by4800.mtx ../etc/4800by4800.mtx ../out/4800mmout.mtx >> ../out/perf/matrixmatrix/matrixmatrix.txt
done

