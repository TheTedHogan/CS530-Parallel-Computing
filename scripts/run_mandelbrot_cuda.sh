#! /bin/bash

#SBATCH -J hogan
#SBATCH -o ./output/mandelbrot_cuda.o
#SBATCH -N 1
#SBATCH -p gpuq
#SBATCH -t 00:02:00
#SBATCH --gres=gpu:1


module load gcc/9.2.0
module load cmake/gcc/3.18.0
module load openmpi/gcc/64/1.10.7
module load nvidia_hpcsdk

cd build
rm -rf *
cmake ..
make

./mandelbrot_cuda pic.ppm