#include "./Points.cuh"

// Device pointers
float* d_particles;
float* d_velocities;
float* d_bestParticle;
float* d_bestParticleVal;
float* d_bestLocalParticles;
float* d_bestLocalVals; 
curandState* d_state;
float* d_globalBestValCandidate;
int* d_globalBestIdxCandidate;

__device__ float calculateTask1(const float* x, int size, int startId)
{
    float sum = 0.0f, product = 1.0f;
    for (int i = 0; i < size; ++i)
    {
        sum += x[startId*size + i] * x[startId*size + i];
        product *= cosf(x[startId*size + i] / ((float)i + 1.0f));
    }
    return sum / 40.0f + 1.0f - product;
}

__device__ float calculateTask2(const float* x, int size, int startId)
{
    float sum = 0.0f;
    for (int i = 0; i < size - 1; ++i)
    {
        float diff = x[startId*size + i + 1] - (x[startId*size + i] * x[startId*size + i]);
        float param2 = 1.0f - x[startId*size + i];
        sum += 100.0f * diff * diff + param2 * param2;
    }
    return sum;
}

__device__ __forceinline__ float atomicMinFloat(float* addr, float value) {
    float old;
    old = !signbit(value) ? __int_as_float(atomicMin((int*)addr, __float_as_int(value))) :
        __uint_as_float(atomicMax((unsigned int*)addr, __float_as_uint(value)));

    return old;
}

__global__ void kernelUpdateParticles(float* d_particles, float* d_velocities, float* d_bestParticle, float* d_bestParticleVal, float* d_bestLocalParticles,
    float* d_bestLocalVals, int particleSize, int particleAmount, float c1, float c2, float c3,
    curandState* state,int blockSize, float* d_globalBestValCandidate, int* d_globalBestIdxCandidate, bool taskIs1=true)
{
    extern __shared__ float sharedMemory[];
    float* sharedBestVal = sharedMemory;
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < particleAmount) {

        curandState localState = state[idx];

        if (threadIdx.x == 0) {
            *sharedBestVal = FLT_MAX;
        }
        __syncthreads();

        for (int i = 0; i < particleSize; ++i) {
            float r1 = curand_uniform(&localState);
            float r2 = curand_uniform(&localState);
            float r3 = curand_uniform(&localState);
            d_velocities[idx * particleSize + i] = c1 * r1 * d_velocities[idx * particleSize + i] +
                c2 * r2 * (d_bestLocalParticles[idx * particleSize + i] - d_particles[idx * particleSize + i]) +
                c3 * r3 * (d_bestParticle[i] - d_particles[idx * particleSize + i]);

            d_particles[idx * particleSize + i] += d_velocities[idx * particleSize + i];
        }
        float newVal = FLT_MAX; 
        if (taskIs1)
        {
            newVal = calculateTask1(d_particles, particleSize, idx);
        }
        else
        {
            newVal = calculateTask2(d_particles, particleSize, idx);
        }
        if (d_bestLocalVals[idx] > newVal)
        {
            d_bestLocalVals[idx] = newVal;
            for (int i = 0; i < particleSize; ++i) {
                d_bestLocalParticles[idx* particleSize + i] = d_particles[idx* particleSize + i ];
            }
            
            atomicMinFloat(sharedBestVal, newVal);
        }

        __syncthreads();

        // Update block-level candidate for the global best
        if (threadIdx.x == 0 && *sharedBestVal < d_globalBestValCandidate[blockIdx.x]) {
            int bestIdxInBlock = blockIdx.x * blockDim.x;
            float testBestVal = FLT_MAX;
            for (int idx_i = blockIdx.x * blockDim.x; idx_i < blockIdx.x * blockDim.x + blockSize; idx_i++) {   //particleAmount must be multiple of blockSize
                if (d_bestLocalVals[idx_i] < testBestVal) {
                    testBestVal = d_bestLocalVals[idx_i];
                    bestIdxInBlock = idx_i;
                }
            }
            
            d_globalBestValCandidate[blockIdx.x] = d_bestLocalVals[bestIdxInBlock];
            d_globalBestIdxCandidate[blockIdx.x] = bestIdxInBlock;
        }
        state[idx] = localState;
    }

}

__global__ void updateGlobalBestParticle(float* d_bestParticle, float* d_bestParticleVal, float* d_bestLocalParticles, float* d_globalBestValCandidate, int* d_globalBestIdxCandidate, int particleSize, int blocksPerGrid) {
    int bestBlock = -1;
    float testBestVal = *d_bestParticleVal;
    for (int blck_i = 0; blck_i < blocksPerGrid; blck_i++) {
        if (d_globalBestValCandidate[blck_i] < testBestVal) {
            bestBlock = blck_i;
            testBestVal = d_globalBestValCandidate[blck_i];
        }
    }

    //printf("best block %d \n", bestBlock);

    if (bestBlock != -1) {
        *d_bestParticleVal = d_globalBestValCandidate[bestBlock];
        for (int i = 0; i < particleSize; i++) {
            d_bestParticle[i] = d_bestLocalParticles[d_globalBestIdxCandidate[bestBlock]*particleSize + i];   //could be parallelised for rewriting simultanously
        }
    }
}

__global__ void initCurand(curandState* state, unsigned long seed, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        curand_init(seed, idx, 0, &state[idx]);
    }
}

__global__ void initParticlesOnGPU(float* d_particles, float* d_velocities, float* d_bestLocalParticles,
    float* d_bestLocalVals, float* d_bestParticle, float* d_globalBestValCandidate, int* d_globalBestIdxCandidate, int particleSize, int particleAmount, curandState* state, float intervalMin, float intervalMax, bool taskIs1)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < particleAmount) {
        curandState localState = state[idx];

        float range = intervalMax - intervalMin;
        for (int i = 0; i < particleSize; ++i) {
            float randomVal = intervalMin + curand_uniform(&localState) * range;
            float randomVelocity = -range + curand_uniform(&localState) * (2.0f * range);

            d_particles[idx * particleSize + i] = randomVal;
            d_velocities[idx * particleSize + i] = randomVelocity;
            d_bestLocalParticles[idx * particleSize + i] = randomVal;
            if (idx == 0) d_bestParticle[i] = randomVal;   //not perfect initialization - to the first particle not the best particle
        }

        float initialVal;
        if (taskIs1) initialVal = calculateTask1(d_particles, particleSize, idx);
        else initialVal = calculateTask2(d_particles, particleSize, idx);
        d_bestLocalVals[idx] = initialVal;

        if (threadIdx.x == 0) {
            d_globalBestValCandidate[blockIdx.x] = d_bestLocalVals[idx];
            d_globalBestIdxCandidate[blockIdx.x] = idx;
        }

        state[idx] = localState;
    }
}

__host__ void initGPU(int particleAmount, int particleSize, int blockSize, float intervalMin, float intervalMax, bool taskIs1)
{
    cudaMalloc(&d_particles, particleAmount * particleSize * sizeof(float));
    cudaMalloc(&d_velocities, particleAmount * particleSize * sizeof(float));
    cudaMalloc(&d_bestParticle, particleSize * sizeof(float));
    cudaMalloc(&d_bestParticleVal, sizeof(float));
    cudaMalloc(&d_bestLocalParticles, particleAmount * particleSize * sizeof(float));
    cudaMalloc(&d_bestLocalVals, particleAmount * sizeof(float));
    cudaMalloc(&d_state, particleAmount * sizeof(curandState));

    int threadsPerBlock = blockSize;
    int blocksPerGrid = (particleAmount + threadsPerBlock - 1) / threadsPerBlock;

    cudaMalloc(&d_globalBestValCandidate, blocksPerGrid * sizeof(float));
    cudaMalloc(&d_globalBestIdxCandidate, blocksPerGrid * sizeof(int));

    initCurand << <blocksPerGrid, threadsPerBlock >> > (d_state, time(0), particleAmount);
    cudaDeviceSynchronize();

    initParticlesOnGPU << <blocksPerGrid, threadsPerBlock >> > (d_particles, d_velocities, d_bestLocalParticles,
        d_bestLocalVals, d_bestParticle, d_globalBestValCandidate, d_globalBestIdxCandidate, particleSize, particleAmount, d_state, intervalMin, intervalMax, taskIs1);
    cudaDeviceSynchronize();

    float initBestParticleVal = FLT_MAX;
    cudaMemcpy(d_bestParticleVal, &initBestParticleVal, sizeof(float), cudaMemcpyHostToDevice);
}


__host__ void freeGPU() 
{
    cudaFree(d_particles);
    cudaFree(d_velocities);
    cudaFree(d_bestParticle);
    cudaFree(d_bestParticleVal);
    cudaFree(d_bestLocalParticles);
    cudaFree(d_bestLocalVals);
    cudaFree(d_state);
    cudaFree(d_globalBestValCandidate);
    cudaFree(d_globalBestIdxCandidate);
}

void copyAndPrintGlobalBest(int* d_globalBestIdxCandidate, float* d_globalBestValCandidate, int N) {
    // Allocate memory on the host to receive the values
    float* h_globalBestVal = new float[N];
    int* h_globalBestIdx = new int[N];

    // Copy the entire vectors from device to host
    cudaMemcpy(h_globalBestVal, d_globalBestValCandidate, N * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_globalBestIdx, d_globalBestIdxCandidate, N * sizeof(int), cudaMemcpyDeviceToHost);

    // Print the values
    std::cout << "Global Best Value " << std::endl;
    for (int i = 0; i < N; ++i) {
        std::cout << h_globalBestVal[i] <<", "; 
    }
    std::cout << std::endl<< "Global Best Index " << std::endl;
    for (int i = 0; i < N; ++i) {
        std::cout << h_globalBestIdx[i] << ", ";
    }
    std::cout << std::endl;

    // Clean up the allocated memory
    delete[] h_globalBestVal;
    delete[] h_globalBestIdx;
}

__host__ void updateP(int particleSize, int particleAmount, float c1, float c2, float c3, int blockSize, bool taskIs1)
{
    int threadsPerBlock = blockSize;
    int blocksPerGrid = (particleAmount + threadsPerBlock - 1) / threadsPerBlock;
    kernelUpdateParticles <<<blocksPerGrid, threadsPerBlock>>> (d_particles, d_velocities, d_bestParticle, d_bestParticleVal, d_bestLocalParticles,
        d_bestLocalVals, particleSize, particleAmount, c1, c2, c3, d_state, blockSize, d_globalBestValCandidate, d_globalBestIdxCandidate, taskIs1);
    cudaDeviceSynchronize();

    //copyAndPrintGlobalBest(d_globalBestIdxCandidate, d_globalBestValCandidate, blocksPerGrid);

    updateGlobalBestParticle << <1, 1 >> > (d_bestParticle, d_bestParticleVal, d_bestLocalParticles, d_globalBestValCandidate, d_globalBestIdxCandidate, particleSize, blocksPerGrid);
    cudaDeviceSynchronize();
}

__host__ void syncResultsToHost(std::vector<float>& bestParticle, float& bestParticleVal) {
    cudaMemcpy(bestParticle.data(), d_bestParticle, bestParticle.size() * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&bestParticleVal, d_bestParticleVal, sizeof(float), cudaMemcpyDeviceToHost);
}