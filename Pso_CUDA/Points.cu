#include "./Points.cuh"

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

__global__ void kernelUpdateParticles(float* d_particles, float* d_velocities, float* d_bestParticle, float* d_bestLocalParticles,
    float* d_bestLocalVals, int particleSize, int particleAmount, float c1, float c2, float c3,
    curandState* state,int blockSize, bool task1=true)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < particleAmount) {
        curandState localState = state[idx];

        for (int i = 0; i < particleSize; ++i) {
            float r1 = curand_uniform(&localState);
            float r2 = curand_uniform(&localState);
            float r3 = curand_uniform(&localState);
           d_velocities[idx * particleSize + i] = c1 * r1 * d_velocities[idx * particleSize + i] +
                c2 * r2 * (d_bestLocalParticles[idx * particleSize + i] - d_particles[idx * particleSize + i]) +
                c3 * r3 * (d_bestParticle[i] - d_particles[idx * particleSize + i]);

            d_particles[idx * particleSize + i] += d_velocities[idx * particleSize + i];
        }
        float newVal; 
        if (task1)
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
            
        }
        state[idx] = localState;
    }

}

__global__ void initCurand(curandState* state, unsigned long seed, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        curand_init(seed, idx, 0, &state[idx]);
    }
}

void updateP(std::vector<float>& particles, std::vector<float>& velocity, std::vector<float>& bestParticle,
    std::vector<float>& bestLocalParticles, std::vector<float>& bestLocalVals, int particleSize, int particleAmount, float c1, float c2, float c3, int blockSize, bool task1)
{
    float* d_particles;
    float* d_velocities;
    float* d_bestParticle;
    float* d_bestLocalParticles;
    float* d_bestLocalVals;
    curandState* d_state;
   
    cudaMalloc(&d_particles, particles.size() * sizeof(float));
    cudaMalloc(&d_velocities, velocity.size()  * sizeof(float));
    cudaMalloc(&d_bestParticle, bestParticle.size() * sizeof(float));
    cudaMalloc(&d_bestLocalParticles, bestLocalParticles.size() * sizeof(float));
    cudaMalloc(&d_bestLocalVals, bestLocalVals.size() * sizeof(float));
    cudaMalloc(&d_state, particleAmount * sizeof(curandState));

    cudaMemcpy(d_particles, particles.data(), particles.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_velocities, velocity.data(), velocity.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_bestParticle, bestParticle.data(), bestParticle.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_bestLocalParticles, bestLocalParticles.data(), bestLocalParticles.size()  * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_bestLocalVals, bestLocalVals.data(), bestLocalVals.size() * sizeof(float), cudaMemcpyHostToDevice);

    int threadsPerBlock = blockSize;
    int blocksPerGrid = (particleAmount + threadsPerBlock - 1) / threadsPerBlock;
    initCurand << <blocksPerGrid, threadsPerBlock >> > (d_state, time(0), particleAmount);
    kernelUpdateParticles << <blocksPerGrid, threadsPerBlock >> > (d_particles, d_velocities, d_bestParticle, d_bestLocalParticles,
        d_bestLocalVals, particleSize, particleAmount, c1, c2, c3, d_state, blockSize, task1);
    cudaDeviceSynchronize();

    // Kopiowanie wyników z powrotem na CPU
    cudaMemcpy(velocity.data(), d_velocities, velocity.size() * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(particles.data(), d_particles, particles.size() * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(bestLocalVals.data(), d_bestLocalVals, bestLocalVals.size() * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(bestLocalParticles.data(), d_bestLocalParticles, bestLocalParticles.size() * sizeof(float), cudaMemcpyDeviceToHost);

    // Zwolnienie pamiêci GPU
    cudaFree(d_particles);
    cudaFree(d_velocities);
    cudaFree(d_bestParticle);
    cudaFree(d_bestLocalParticles);
    cudaFree(d_bestLocalVals);
    cudaFree(d_state);
}
