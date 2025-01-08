#include "./Points.cuh"

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

        state[idx] = localState;
    }

}

__global__ void initCurand(curandState* state, unsigned long seed, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        curand_init(seed, idx, 0, &state[idx]);
    }
}

void updateP(std::vector<std::vector<float>> particles, std::vector<std::vector<float>> velocity, std::vector<float> bestParticle,
    std::vector<std::vector<float>> bestLocalParticle, std::vector<float> bestLocalVals, int particleSize, int particleAmount, float c1, float c2, float c3, int blockSize, bool task1)
{
    float* d_particles;
    float* d_velocities;
    float* d_bestParticle;
    float* d_bestLocalParticles;
    float* d_bestLocalVals;
    curandState* d_state;

    cudaMalloc(&d_particles, particles.size() * particles[0].size() * sizeof(float));
    cudaMalloc(&d_velocities, velocity.size() * velocity[0].size() * sizeof(float));
    cudaMalloc(&d_bestParticle, bestParticle.size() * sizeof(float));
    cudaMalloc(&d_bestLocalParticles, bestLocalParticle.size() * sizeof(float));
    cudaMalloc(&d_bestLocalVals, bestLocalVals.size() * sizeof(float));
    cudaMalloc(&d_state, particles.size() / particles[0].size() * sizeof(curandState));

    cudaMemcpy(d_particles, particles.data(), particles.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_velocities, velocity.data(), velocity.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_bestParticle, bestParticle.data(), bestParticle.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_bestLocalParticles, bestLocalParticle.data(), particles.size() * particles[0].size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_bestLocalVals, bestLocalVals.data(), velocity.size() * sizeof(float), cudaMemcpyHostToDevice);

    int threadsPerBlock = blockSize;
    int blocksPerGrid = (particleAmount + threadsPerBlock - 1) / threadsPerBlock;
    initCurand << <blocksPerGrid, threadsPerBlock >> > (d_state, time(0), particleAmount);
    //kernelUpdateParticles << <blocksPerGrid, threadsPerBlock >> > (d_particles, d_velocities, d_bestParticle, d_bestLocalParticles,
       // d_bestLocalVals, particleSize, particleAmount, c1, c2, c3, d_state, blockSize, task1); coœ siê pierdoli
    cudaDeviceSynchronize();

    // Kopiowanie wyników z powrotem na CPU
    cudaMemcpy(velocity.data(), d_velocities, velocity.size() * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(particles.data(), d_particles, particles.size() * sizeof(float), cudaMemcpyDeviceToHost);

    // Zwolnienie pamiêci GPU
    cudaFree(d_particles);
    cudaFree(d_velocities);
    cudaFree(d_bestParticle);
    cudaFree(d_bestLocalParticles);
    cudaFree(d_bestLocalVals);
    cudaFree(d_state);
}
