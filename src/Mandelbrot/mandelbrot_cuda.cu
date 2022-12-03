/*
  This program is imported from:
  https://gist.github.com/andrejbauer/7919569

  This program is an adaptation of the Mandelbrot program
  from the Programming Rosetta Stone, see
  http://rosettacode.org/wiki/Mandelbrot_set

  See http://www.imagemagick.org/Usage/color_mods/ for what ImageMagick
  can do. It can do a lot.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cuda.h>

__global__ void kernal(int *device_results, int total_pixels, int xres, double xmin, double ymax, double dx, double dy, int maxiter){
    int threadId = blockDim.x * blockIdx.x + threadIdx.x;

    if(threadId >= total_pixels){
        return;
    }

    double x, y; /* Coordinates of the current point in the complex plane. */
    //double u, v; /* Coordinates of the iterated point. */
    /* Pixel counters */

    int i = threadId % xres;
    int j =  threadId / xres;
    int k; /* Iteration counter */
    y = ymax - j * dy;
    double u = 0.0;
    double v= 0.0;
    double u2 = u * u;
    double v2 = v*v;
    x = xmin + i * dx;

    /* iterate the point */
    for (k = 1; k < maxiter && (u2 + v2 < 4.0); k++) {
        v = 2 * u * v + y;
        u = u2 - v2 + x;
        u2 = u * u;
        v2 = v * v;
    };
    device_results[threadId] = k;
}


int main(int argc, char* argv[])
{
    //Check status
    cudaError_t cudaStatus;
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
    }
    int *results_host;
    int *results_device;
    /* Parse the command line arguments. */
    if (argc != 2) {
        printf("Usage:   %s <out.ppm>\n", argv[0]);
        printf("Example: %s pic.ppm\n", argv[0]);
        return -1;
    }

    /* The window in the plane. */
    const double xmin = -2;
    const double xmax = 1;
    const double ymin = -1.5;
    const double ymax = 1.5;

    /* Maximum number of iterations, at most 65535. */
    const uint16_t maxiter = 1000;

    /* Image size, width is given, height is computed. */
    const int xres = 1000;
    const int yres = 1000;
    const int pixels = xres * yres;

    /* Precompute pixel width and height. */
    double dx=(xmax-xmin)/xres;
    double dy=(ymax-ymin)/yres;

    const int blockSize = 32;
    int gridCount = pixels / blockSize;


    if (pixels % blockSize != 0){
        gridCount += 1;
    }

    int pixelsPerBlock = pixels / gridCount;
    if (pixels % gridCount != 0){
        pixelsPerBlock += 1;
    }

    dim3 dimGrid(gridCount, 1, 1);
    dim3 dimBlock(blockSize,1, 1);

    int results_bytes = sizeof(int)*pixels;
    results_host = (int*)malloc(results_bytes);
    cudaMalloc(&results_device, results_bytes);

    kernal<<<dimGrid,dimBlock>>>(results_device, pixels, xres, xmin, ymax, dx, dy, maxiter);
    cudaDeviceSynchronize();

    cudaMemcpy(results_host, results_device, results_bytes, cudaMemcpyDeviceToHost);


    /* The output file name */
    const char* filename = argv[1];

    /* Open the file and write the header. */
    FILE * fp = fopen(filename,"wb");
//char *comment="# Mandelbrot set";/* comment should start with # */

    /*write ASCII header to the file*/
    fprintf(fp,
            "P6\n# Mandelbrot, xmin=%lf, xmax=%lf, ymin=%lf, ymax=%lf, maxiter=%d\n%d\n%d\n%d\n",
            xmin, xmax, ymin, ymax, maxiter, xres, yres, (maxiter < 256 ? 256 : maxiter));

    /* compute  pixel color and write it to file */
    for(int z = 0; z < pixels; z++){
        int k = results_host[z];
        if (k >= maxiter) {
            /* interior */
            const unsigned char black[] = {0, 0, 0, 0, 0, 0};
            fwrite (black, 6, 1, fp);
        }
        else {
            /* exterior */
            unsigned char color[6];
            color[0] = k >> 8;
            color[1] = k & 255;
            color[2] = k >> 8;
            color[3] = k & 255;
            color[4] = k >> 8;
            color[5] = k & 255;
            fwrite(color, 6, 1, fp);
        };
    }





    fclose(fp);
    return 0;
}
