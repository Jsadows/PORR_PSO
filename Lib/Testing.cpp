#include "Tasks.h"
#include "Pso.h"
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <chrono>  // For measuring execution time

int testing_run()
{
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();

    // Task combinations
    std::vector<std::shared_ptr<Task>> tasks = {t1, t2};

    // Particle size combinations
    std::vector<int> particleSizes = {10, 50, 100};

    // Number of particles in the swarm
    int particleAmount = 10000;

    // PSO parameters
    std::vector<float> params = {1.0f, 2.0f, 2.0f};

    // Number of iterations and precision for stopping criterion
    int maxIterations = 200;
    float precision = 0.01f;

    // Output file for results
    std::ofstream resultsFile("results.csv");
    resultsFile << "Task,ParticleSize,KnownBestX,ExecutionTime,ResultValue" << std::endl;

    // Test each combination
    for (const auto& task : tasks) {
        for (int particleSize : particleSizes) {
            // Determine the known best X based on the task
            std::optional<std::vector<float>> knownBestX = std::nullopt;
            if (task == t1) {
                knownBestX = std::vector<float>(particleSize, 0.0f); // All 0s for t1
            } else if (task == t2) {
                knownBestX = std::vector<float>(particleSize, 1.0f); // All 1s for t2
            }

            // Create PSO instance
            std::unique_ptr<Pso> pso = std::make_unique<Pso>(
                    task, particleSize, particleAmount, params[0], params[1], params[2]);

            // Measure execution time
            auto startTime = std::chrono::high_resolution_clock::now();
            std::vector<float> result = pso->findMin(maxIterations, precision, knownBestX);
            auto endTime = std::chrono::high_resolution_clock::now();

            // Calculate execution time in milliseconds
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

            // Calculate the task value for the result
            float resultValue = task->calculateTask(result);

            // Output the results
            resultsFile << (task == t1 ? "t1" : "t2") << ","
                        << particleSize << ","
                        << (knownBestX ? "Yes" : "No") << ","
                        << duration << ","
                        << resultValue << std::endl;

            std::cout << "Task: " << (task == t1 ? "t1" : "t2")
                      << ", Particle Size: " << particleSize
                      << ", KnownBestX: " << (knownBestX ? "Yes" : "No")
                      << ", Time: " << duration << "ms"
                      << ", Result: " << resultValue << std::endl;
        }
    }

    resultsFile.close();
    return 0;
}
