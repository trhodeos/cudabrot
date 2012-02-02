#include <stdio.h>

#define SCALE 1/240.0f
#define LIMIT 192
/*
  __global__ void cudabrot_kernel(unsigned char* buffer, unsigned int width, unsigned int height,
  float scale, float limit) {

  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int x = index % width;
  int y = index / width;

  if (x < width && y < height) {
  buffer[4 * (y * width + x) + 0] =  0x00;
  buffer[4 * (y * width + x) + 1] =  0x00;
  buffer[4 * (y * width + x) + 2] =  0x00;
  buffer[4 * (y * width + x) + 3] =  0xff;
  }
  }*/

__global__ void cudabrot_kernel(unsigned char* buffer, unsigned int width, unsigned int height,
                                float scale, float limit) {


  __shared__ float sCenter[2];

  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int x = index % width;
  int y = index / width;

  if (threadIdx.x == 0) {
    sCenter[0] = width / 2.0f;
    sCenter[1] = height / 2.0f;
  }

  __syncthreads();
  if (index < width * height) {
    float ax =  (x - sCenter[0]) * scale;
    float ay =  (y - sCenter[1]) * scale;
    float a1 = ax, b1 = ay;
    float a2, b2;
    int lp = 0;
    while (!(lp > 255 || ((a1*a1 + b1*b1) > limit))) {
      lp++;
      a2 = a1 * a1 - b1 * b1 + ax;
      b2 = 2 * a1 * b1 + ay;
      a1 = a2;
      b1 = b2;
    }
    if (lp > 255) {
      lp = 0;
    }
    __syncthreads();
    // NOTE: OpenGL likes col-major

    buffer[4 * (y * width + x) + 0] = (2*lp) % 256;
    buffer[4 * (y * width + x) + 1] = (3*lp) % 256;
    buffer[4 * (y * width + x) + 2] = (5*lp) % 256;
    buffer[4 * (y * width + x) + 3] = 0xff;
  }
}

void launch_cudabrot_kernel(unsigned char* buffer, unsigned int width, unsigned int height) {
  int num_threads = 256;
  int total_threads = width * height;
  int num_blocks = total_threads / num_threads;
  num_blocks += ((total_threads % num_threads) > 0) ? 1 : 0;
  cudabrot_kernel<<<num_blocks, num_threads>>>(buffer, width, height, SCALE, LIMIT);
  cudaError e = cudaGetLastError();
  if (e != cudaSuccess) {
    printf("%s\n", cudaGetErrorString(e));
  }
}
