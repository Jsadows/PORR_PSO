#include "cuda_runtime.h"
#include <curand_kernel.h>
#include <vector>
#include <iostream>

void updateP(std::vector<float>& particles, std::vector<float>& velocity, std::vector<float>& bestParticle,
    std::vector<float>& bestLocalParticles, std::vector<float>& bestLocalVals, int particleSize, int particleAmount, float c1, float c2, float c3, int blockSize, bool task1);