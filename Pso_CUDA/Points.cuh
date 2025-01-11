#include "cuda_runtime.h"
#include <device_launch_parameters.h>
#include <curand_kernel.h>
#include <vector>
#include <iostream>

void updateP(int particleSize, int particleAmount, float c1, float c2, float c3, int blockSize, bool taskIs1);

void initGPU(int particleAmount, int particleSize, int blockSize, float intervalMin, float intervalMax, bool taskIs1);

void freeGPU();

void syncResultsToHost(std::vector<float>& bestParticle, float& bestParticleVal);