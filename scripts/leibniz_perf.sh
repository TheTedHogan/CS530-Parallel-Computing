#! /bin/bash

#SBATCH -J groupc_leibniz
#SBATCH -o ./output/perf_leibniz_output.o
#SBATCH -n 28
#SBATCH -N 1
#SBATCH -p defq
#SBATCH -t 00:45:00


module load gcc/10.2.0
module load cmake/gcc/3.18.0
module load openmpi/gcc/64/1.10.7

cd build
#rm -rf *
#cmake ..
#make

rm ../out/perf/leibniz/leibniz.txt
touch ../out/perf/leibniz/leibniz.txt

for i in {1..28}
do
echo -ne "size\t57600\tprocs\t" >> ../out/perf/leibniz/leibniz.txt
echo -ne $i  >> ../out/perf/leibniz/leibniz.txt
echo -ne "\t"  >> ../out/perf/leibniz/leibniz.txt
mpirun -n $i ./leibniz 57600  >> ../out/perf/leibniz/leibniz.txt
done

for i in {1..28}
do
echo -ne "size\t2560000\tprocs\t" >> ../out/perf/leibniz/leibniz.txt
echo -ne $i  >> ../out/perf/leibniz/leibniz.txt
echo -ne "\t"  >> ../out/perf/leibniz/leibniz.txt
mpirun -n $i ./leibniz 2560000  >> ../out/perf/leibniz/leibniz.txt
done

for i in {1..28}
do
echo -ne "size\t23040000\tprocs\t" >> ../out/perf/leibniz/leibniz.txt
echo -ne $i  >> ../out/perf/leibniz/leibniz.txt
echo -ne "\t"  >> ../out/perf/leibniz/leibniz.txt
mpirun -n $i ./leibniz 23040000 >> ../out/perf/leibniz/leibniz.txt
done


