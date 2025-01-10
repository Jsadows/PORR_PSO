#include "cuda_runtime.h"
#include <device_launch_parameters.h>
#include <curand_kernel.h>
#include <vector>
#include <iostream>

void updateP(int particleSize, int particleAmount, float c1, float c2, float c3, int blockSize, bool taskIs1);

void initGPU(const std::vector<float>& particles, const std::vector<float>& velocity,
    const std::vector<float>& bestParticle, const float& bestParticleVal, const std::vector<float>& bestLocalParticles,
    const std::vector<float>& bestLocalVals, int particleAmount, int particleSize, int blockSize);

void freeGPU();

void syncResultsToHost(std::vector<float>& bestParticle, float& bestParticleVal);