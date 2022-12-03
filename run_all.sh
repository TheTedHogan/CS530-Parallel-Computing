#! /bin/bash
#SBATCH -J GroupC
#SBATCH -o ./out/output.o%j
#SBATCH -n 1
#SBATCH -N 1
#SBATCH -p classroom
#SBATCH -t 00:60:00

export OMP_NUM_THREADS=1

module load cmake/gcc/3.18.0
module load openmpi/gcc/64/1.10.7
module load nvidia_hpcsdk
module load gcc/9.2.0


rm -rf build
rm -rf ./out/perf/*
mkdir build
cd build
cmake ..
make

touch ../out/perf/mm_perf_out_small
for i in {1..28}
do
OMP_NUM_THREADS=$i ./matrixmatrix ../etc/2by3matrix.mtx ../etc/3by2matrix.mtx ../etc/r2testoutmmsmall.mtx >> ../out/perf/mm_perf_out_small
OMP_NUM_THREADS=$i ./matrixmatrix ../etc/50by30matrix.mtx ../etc/30by20matrix.mtx ../etc/r2testoutmmmedium.mtx >> ../out/perf/mm_perf_out_med
OMP_NUM_THREADS=$i ./matrixmatrix ../etc/1000by1000matrix.mtx ../etc/1000by1000matrix2.mtx ..etc/r2testoutmmlarge.mtx >> ../out/perf/mm_perf_out_large
done

#
#touch ../out/perf/mv_perf_out_small
#for i in {1..28}
#do
#OMP_NUM_THREADS=$i ./matrixvector ../etc/ck104.mtx ../etc/vector104.mtx ../etc/r2testoutmv.mtx >> ../out/perf/mv_perf_out_small
#done
#
#touch ../out/perf/monte_perf_out_small
#touch ../out/perf/monte_perf_out_med
#touch ../out/perf/monte_perf_out_large
#for i in {1..28}
#do
#OMP_NUM_THREADS=$i ./montecarlo 10 >> ../out/perf/monte_perf_out_small
#OMP_NUM_THREADS=$i ./montecarlo 1000000 >> ../out/perf/monte_perf_out_med
#OMP_NUM_THREADS=$i ./montecarlo 2147483647 >> ../out/perf/monte_perf_out_large
#done
#
#touch ../out/perf/fibb_perf_out_small
#touch ../out/perf/fibb_perf_out_med
#touch ../out/perf/fibb_perf_out_large
#for i in {1..28}
#do
#OMP_NUM_THREADS=$i ./fibonacci_omp 10 s >> ../out/perf/fibb_perf_out_small
#OMP_NUM_THREADS=$i ./fibonacci_omp 1000000 s >> ../out/perf/fibb_perf_out_med
#OMP_NUM_THREADS=$i ./fibonacci_omp 2147483647 s >> ../out/perf/fibb_perf_out_large
#done
#
#touch ../out/perf/leibniz_perf_out_small
#touch ../out/perf/leibniz_perf_out_med
#touch ../out/perf/leibniz_perf_out_large
#for i in {1..28}
#do
#OMP_NUM_THREADS=$i ./leibniz 10 >> ../out/perf/leibniz_perf_out_small
#OMP_NUM_THREADS=$i ./leibniz 1000000 >> ../out/perf/leibniz_perf_out_medium
#OMP_NUM_THREADS=$i ./leibniz 2147483647 >> ../out/perf/leibniz_perf_out_large
#done

