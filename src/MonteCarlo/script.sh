BATCH -J GroupC
#SBATCH -o ./out/output.o%j
#SBATCH -n 1
#SBATCH -N 1
#SBATCH -p classroom
#SBATCH -t 00:60:00

module load gcc/10.2.0
module load openmpi/gcc/64/1.10.7
module load cmake/gcc/3.18.0

rm -rf build
rm -rf ./out/perf/*
mkdir build
cd build
cmake ..
make

touch ../out/perf/monte_perf_out_small
touch ../out/perf/monte_perf_out_med
touch ../out/perf/monte_perf_out_large
for i in {1..28}
do
OMP_NUM_THREADS=$i ./montecarlo 10 >> ../out/perf/monte_perf_out_small
OMP_NUM_THREADS=$i ./montecarlo 1000000 >> ../out/perf/monte_perf_out_med
OMP_NUM_THREADS=$i ./montecarlo 2147483647 >> ../out/perf/monte_perf_out_large
done
