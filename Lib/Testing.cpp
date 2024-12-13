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
    int particleAmount = 100000;

    // PSO parameters
//    std::vector<float> params = {1.3f, 1.5f, 0.8f};
    std::vector<float> params = {1.3f, 0.8f, 0.8f};

    // Number of iterations and precision for stopping criterion
    int maxIterations = 100;
    float precision = 1e-6;

    // Output file for results
    std::ofstream resultsFile("results.csv");
    resultsFile << "Task,ParticleSize,KnownBestX,ExecutionTime,ResultValue" << std::endl;

    // Test each combination
    for (const auto& task : tasks) {
        for (int particleSize : particleSizes) {
            for (bool stopMethod : { false, true }){
                std::optional<std::vector<float>> knownBestX = std::nullopt;
                if (stopMethod) {
                    // Determine the known best X based on the task
                    if (task == t1) {
                        knownBestX = std::vector<float>(particleSize, 0.0f); // All 0s for t1
                    } else if (task == t2) {
                        knownBestX = std::vector<float>(particleSize, 1.0f); // All 1s for t2
                    }
                    // different precision when comparing the vector
                    precision = 1e-2;
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
    }

    resultsFile.close();
    return 0;
}


int x5_best_test_run()
{
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();

    // Task combinations
    std::vector<std::shared_ptr<Task>> tasks = {t1, t2};

    // Particle size combinations
    std::vector<int> particleSizes = {10, 50, 100};

    // Number of particles in the swarm
    int particleAmount = 100000;

    // PSO parameters
//    std::vector<float> params = {1.3f, 1.5f, 0.8f};
    std::vector<float> params = {1.3f, 0.8f, 0.8f};

    // Number of iterations and precision for stopping criterion
    int maxIterations = 100;
    float precision = 1e-6;
    // Threads number   
    int threads_nb = 16;
    // Output file for results
    std::ofstream resultsFile("results_x5_best.csv");
    resultsFile << "Threads number: " << threads_nb <<  std::endl;
    resultsFile << "Task,ParticleSize,KnownBestX,MinTime,MaxTime,AvgTime,VarTime,MinResult,MaxResult,AvgResult,VarResult" << std::endl;

    // Test each combination
    for (const auto& task : tasks) {
        for (int particleSize : particleSizes) {
            for (bool stopMethod : { false, true }){

                std::optional<std::vector<float>> knownBestX = std::nullopt;
                if (stopMethod) {
                    // Determine the known best X based on the task
                    if (task == t1) {
                        knownBestX = std::vector<float>(particleSize, 0.0f); // All 0s for t1
                    } else if (task == t2) {
                        knownBestX = std::vector<float>(particleSize, 1.0f); // All 1s for t2
                    }
                    // different precision when comparing the vector
                    precision = 1e-2;
                }

                std::vector<long long> durations;
                std::vector<float> resultValues;

                for (int i = 0; i < 5; ++i) {
                    // Create PSO instance
                    std::unique_ptr<Pso> pso = std::make_unique<Pso>(
                            task, particleSize, particleAmount, params[0], params[1], params[2]);

                    // Measure execution time
                    auto startTime = std::chrono::high_resolution_clock::now();
                    std::vector<float> result = pso->findMin(maxIterations, precision, knownBestX, threads_nb);
                    auto endTime = std::chrono::high_resolution_clock::now();

                    // Calculate execution time in milliseconds
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
                    durations.push_back(duration);

                    // Calculate the task value for the result
                    float resultValue = task->calculateTask(result);
                    resultValues.push_back(resultValue);
                }

                // Calculate statistics
                auto minDuration = *std::min_element(durations.begin(), durations.end());
                auto maxDuration = *std::max_element(durations.begin(), durations.end());
                auto avgDuration = std::accumulate(durations.begin(), durations.end(), 0LL) / durations.size();
                auto varDuration = std::accumulate(durations.begin(), durations.end(), 0.0, 
                    [avgDuration](double sum, long long val) { return sum + (val - avgDuration) * (val - avgDuration); }) / durations.size();

                auto minResultValue = *std::min_element(resultValues.begin(), resultValues.end());
                auto maxResultValue = *std::max_element(resultValues.begin(), resultValues.end());
                auto avgResultValue = std::accumulate(resultValues.begin(), resultValues.end(), 0.0f) / resultValues.size();
                auto varResultValue = std::accumulate(resultValues.begin(), resultValues.end(), 0.0, 
                    [avgResultValue](double sum, float val) { return sum + (val - avgResultValue) * (val - avgResultValue); }) / resultValues.size();

                // Output the results
                resultsFile << (task == t1 ? "t1" : "t2") << ","
                            << particleSize << ","
                            << (knownBestX ? "Yes" : "No") << ","
                            << minDuration << ","
                            << maxDuration << ","
                            << avgDuration << ","
                            << varDuration << ","
                            << minResultValue << ","
                            << maxResultValue << ","
                            << avgResultValue << ","
                            << varResultValue << std::endl;

                std::cout << "Task: " << (task == t1 ? "t1" : "t2")
                          << ", Particle Size: " << particleSize
                          << ", KnownBestX: " << (knownBestX ? "Yes" : "No")
                          << ", Min Time: " << minDuration << "ms"
                          << ", Max Time: " << maxDuration << "ms"
                          << ", Avg Time: " << avgDuration << "ms"
                          << ", Var Time: " << varDuration << "ms"
                          << ", Min Result: " << minResultValue
                          << ", Max Result: " << maxResultValue
                          << ", Avg Result: " << avgResultValue
                          << ", Var Result: " << varResultValue << std::endl;
            }
        }
    }

    resultsFile.close();
    return 0;
}

int threads_test_run()
{
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();

    // Task combinations
    std::vector<std::shared_ptr<Task>> tasks = {t1, t2};

    // Particle size combinations
    std::vector<int> particleSizes = {10, 50, 100};

    // Number of particles in the swarm
    int particleAmount = 100000;

    // PSO parameters
//    std::vector<float> params = {1.3f, 1.5f, 0.8f};
    std::vector<float> params = {1.3f, 0.8f, 0.8f};

    // Number of iterations and precision for stopping criterion
    int maxIterations = 100;
    float precision = 1e-6;

    // Threads numbers to test
    std::vector<int> threadsNumbers = {1, 2, 16, 32};

    // Output file for results
    std::ofstream resultsFile("results_threads.csv");
    resultsFile << "Task,ParticleSize,KnownBestX,Threads,ExecutionTime,ResultValue" << std::endl;

    // Test each combination
    for (const auto& task : tasks) {
        for (int particleSize : particleSizes) {
            for (bool stopMethod : { false, true }) {
                for (int threads_nb : threadsNumbers) {
                    std::optional<std::vector<float>> knownBestX = std::nullopt;
                    if (stopMethod) {
                        // Determine the known best X based on the task
                        if (task == t1) {
                            knownBestX = std::vector<float>(particleSize, 0.0f); // All 0s for t1
                        } else if (task == t2) {
                            knownBestX = std::vector<float>(particleSize, 1.0f); // All 1s for t2
                        }
                        // different precision when comparing the vector
                        precision = 1e-2;
                    }

                    // Create PSO instance
                    std::unique_ptr<Pso> pso = std::make_unique<Pso>(
                            task, particleSize, particleAmount, params[0], params[1], params[2]);

                    // Measure execution time
                    auto startTime = std::chrono::high_resolution_clock::now();
                    std::vector<float> result = pso->findMin(maxIterations, precision, knownBestX, threads_nb);
                    auto endTime = std::chrono::high_resolution_clock::now();

                    // Calculate execution time in milliseconds
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

                    // Calculate the task value for the result
                    float resultValue = task->calculateTask(result);

                    // Output the results
                    resultsFile << (task == t1 ? "t1" : "t2") << ","
                                << particleSize << ","
                                << (knownBestX ? "Yes" : "No") << ","
                                << threads_nb << ","
                                << duration << ","
                                << resultValue << std::endl;

                    std::cout << "Task: " << (task == t1 ? "t1" : "t2")
                              << ", Particle Size: " << particleSize
                              << ", KnownBestX: " << (knownBestX ? "Yes" : "No")
                              << ", Threads: " << threads_nb
                              << ", Time: " << duration << "ms"
                              << ", Result: " << resultValue << std::endl;
                }
            }
        }
    }

    resultsFile.close();
    return 0;
}
