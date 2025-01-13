#include "Tasks.h"
#include "Pso_mpi.h"
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <chrono>  
#include <algorithm> 
#include <numeric>   

int testing_run(){
    std::shared_ptr<Task> t1 = std::make_shared<Task1>();
    std::shared_ptr<Task> t2 = std::make_shared<Task2>();

    std::vector<std::shared_ptr<Task>> tasks = {t1, t2};

    std::vector<float> params = {1.3f, 0.8f, 0.8f};

    // std::vector<int> particleSizes = {10, 50, 100};
    // int particleAmount = 100000;

    // stopping criterion
    // int m = 200;
    float precision = 1e-6;

    // (particleSize, particleAmount, m)
    std::vector<std::tuple<int, int, int>> scenarioParams = {
        { 10,   10240,  10 },
        { 50,   51200,  50 },
        { 100, 102400, 100 }
    };
       
    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    std::ofstream resultsFile;
    if (rank == 0) {
        resultsFile.open("results_" + std::to_string(worldSize) + ".csv");
        resultsFile << "Task,ParticleSize,KnownBestX,ExecutionTime,ResultValue" << std::endl;
    }

    for (const auto& task : tasks) {
        for (auto& [particleSize, particleAmount, m] : scenarioParams) {
            for (bool stopMethod : { false, true })
            {
                std::optional<std::vector<float>> knownBestX = std::nullopt;
                float localPrecision = precision; 
                // Best X
                if (stopMethod) {
                    if (task == t1) {
                        knownBestX = std::vector<float>(particleSize, 0.0f);
                    } 
                    else if (task == t2) {
                        knownBestX = std::vector<float>(particleSize, 1.0f);
                    }
                    localPrecision = 1e-2f;
                }

                std::unique_ptr<PsoMPI> pso = std::make_unique<PsoMPI>(
                    task, 
                    particleSize, 
                    particleAmount, 
                    params[0], 
                    params[1], 
                    params[2], 
                    /*maxIter=*/500  
                );

                // Measure time
                auto startTime = std::chrono::high_resolution_clock::now();
                std::vector<float> result = pso->findMin(m, localPrecision, knownBestX);
                auto endTime   = std::chrono::high_resolution_clock::now();

                long long localDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

                // Local result value
                float localResultValue = task->calculateTask(result);

                // gather values to rank 0
                std::vector<long long> allDurations; 
                if (rank == 0) {
                    allDurations.resize(worldSize);
                }
                MPI_Gather(&localDuration, 1, MPI_LONG_LONG,
                        (rank == 0 ? allDurations.data() : nullptr), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

                // gather resultValue
                std::vector<float> allResults;
                if (rank == 0) {
                    allResults.resize(worldSize);
                }
                MPI_Gather(&localResultValue, 1, MPI_FLOAT,
                        (rank == 0 ? allResults.data() : nullptr), 1, MPI_FLOAT,
                        0, MPI_COMM_WORLD);

                // Only rank 0 writes
                if (rank == 0) {
                    auto max_time = std::max_element(allDurations.begin(), allDurations.end());
                    auto mean_result = std::accumulate(allResults.begin(), allResults.end(), 0.0);
                    mean_result /= allResults.size();
                    resultsFile 
                    << (task == t1 ? "t1" : "t2") << ","
                    << particleSize << ","
                    << (knownBestX ? "Yes" : "No") << ","
                    <<  *max_time << ","
                    <<  mean_result << ","
                    << std::endl;

                    // data for each rank 
                    // for (int r = 0; r < worldSize; r++) {
                    //     resultsFile 
                    //         << (task == t1 ? "t1" : "t2") << ","
                    //         << particleSize << ","
                    //         << (knownBestX ? "Yes" : "No") << ","
                    //         << r << ","  // which rank's data
                    //         << allDurations[r] << ","
                    //         << allResults[r]
                    //         << std::endl;
                    // }
                }

                // logging
                if (rank == 0) {
                    std::cout << "Task: " << (task == t1 ? "t1" : "t2")
                            << ", ParticleSize: " << particleSize
                            << ", KnownBestX: " << (knownBestX ? "Yes" : "No")
                            << " => data gathered from all ranks." << std::endl;
                }
             }
        }
    }
    if(rank == 0){
        resultsFile.close();
    }
    return 0;
}

int x5_best_test_run()
{
    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    std::ofstream resultsFile;
    if (rank == 0) {
        resultsFile.open("results_x5_best.csv");
        resultsFile << "Task,ParticleSize,KnownBestX,MinTime,MaxTime,AvgTime,VarTime,"
                    << "MinResult,MaxResult,AvgResult,VarResult\n";
    }

    std::shared_ptr<Task> t1 = std::make_shared<Task1>();
    std::shared_ptr<Task> t2 = std::make_shared<Task2>();

    std::vector<std::shared_ptr<Task>> tasks = {t1, t2};
    // std::vector<int> particleSizes = {10, 50, 100};
    // int   particleAmount = 100000;

    // stopping criterion
    // int m  = 200;
    float precision      = 1e-6f;

    // (particleSize, particleAmount, m)
    std::vector<std::tuple<int, int, int>> scenarioParams = {
        { 10,   10240,  10 },
        { 50,   51200,  50 },
        { 100, 102400, 100 }
    };

    
    std::vector<float> params = {1.3f, 0.8f, 0.8f};

    // main loop: tasks, sizes, knownBestX
    for (const auto& task : tasks) 
    {
        for (auto& [particleSize, particleAmount, m] : scenarioParams) {
        {
            for (bool stopMethod : { false, true }) 
            {
                std::optional<std::vector<float>> knownBestX = std::nullopt;
                float localPrecision = precision;
                if (stopMethod) {
                    if (task == t1) {
                        knownBestX = std::vector<float>(particleSize, 0.0f);
                    } else if (task == t2) {
                        knownBestX = std::vector<float>(particleSize, 1.0f);
                    }
                    localPrecision = 1e-2f;
                }

                const int runs = 5;
                std::vector<long long> localDurations(runs);
                std::vector<float>     localResults(runs);

                for (int i = 0; i < runs; ++i) 
                {
                    // new PsoMPI each run
                    std::unique_ptr<PsoMPI> pso = std::make_unique<PsoMPI>(
                        task, 
                        particleSize, 
                        particleAmount, 
                        params[0], 
                        params[1], 
                        params[2],
                        /*maxIter=*/500
                    );

                    // time on each rank
                    auto startTime = std::chrono::high_resolution_clock::now();
                    std::vector<float> result = pso->findMin(m, localPrecision, knownBestX);
                    auto endTime   = std::chrono::high_resolution_clock::now();

                    // local duration
                    long long durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
                    localDurations[i] = durationMs;

                    //  local result value
                    float resultValue = task->calculateTask(result);
                    localResults[i]   = resultValue;
                }

                // gather localDurations and localResults from all ranks
                // gather  *each rank* => total = runs * worldSize.

                std::vector<long long> allDurations;
                std::vector<float>     allResults;

                if (rank == 0) {
                    allDurations.resize(runs * worldSize);
                    allResults.resize(runs * worldSize);
                }

                // gather results
                MPI_Gather(
                    localDurations.data(), runs, MPI_LONG_LONG,
                    rank == 0 ? allDurations.data() : nullptr, runs, MPI_LONG_LONG,
                    0, MPI_COMM_WORLD
                );
                // gather results
                MPI_Gather(
                    localResults.data(), runs, MPI_FLOAT,
                    rank == 0 ? allResults.data() : nullptr, runs, MPI_FLOAT,
                    0, MPI_COMM_WORLD
                );


                if (rank == 0) 
                {
                    // runs*worldSize durations and results in allDurations/allResults
                    auto [minDurationIt, maxDurationIt] = std::minmax_element(allDurations.begin(), allDurations.end());
                    long long minDuration = *minDurationIt;
                    long long maxDuration = *maxDurationIt;

                    double sumDur = std::accumulate(allDurations.begin(), allDurations.end(), 0.0);
                    double avgDuration = sumDur / (allDurations.size());

                    double varDuration = 0.0;
                    for (auto &val : allDurations) {
                        varDuration += (val - avgDuration) * (val - avgDuration);
                    }
                    varDuration /= allDurations.size();

                    auto [minResultIt, maxResultIt] = std::minmax_element(allResults.begin(), allResults.end());
                    float minResultValue = *minResultIt;
                    float maxResultValue = *maxResultIt;

                    // all data results should be the same but just in case
                    double sumRes = std::accumulate(allResults.begin(), allResults.end(), 0.0);
                    double avgResultValue = sumRes / allResults.size();

                    double varResultValue = 0.0;
                    for (auto &val : allResults) {
                        varResultValue += (val - avgResultValue) * (val - avgResultValue);
                    }
                    varResultValue /= allResults.size();


                    std::string taskName = (task == t1 ? "t1" : "t2");
                    std::string knownXStr= (knownBestX ? "Yes" : "No");

                    resultsFile 
                        << taskName       << ","
                        << particleSize   << ","
                        << knownXStr      << ","
                        << minDuration    << ","
                        << maxDuration    << ","
                        << avgDuration    << ","
                        << varDuration    << ","
                        << minResultValue << ","
                        << maxResultValue << ","
                        << avgResultValue << ","
                        << varResultValue
                        << std::endl;

                    std::cout << "Task: "       << taskName
                              << ", ParticleSize: " << particleSize
                              << ", KnownBestX: "   << knownXStr
                              << ", MinTime: "      << minDuration  << "ms"
                              << ", MaxTime: "      << maxDuration  << "ms"
                              << ", AvgTime: "      << avgDuration  << "ms"
                              << ", VarTime: "      << varDuration
                              << ", MinResult: "    << minResultValue
                              << ", MaxResult: "    << maxResultValue
                              << ", AvgResult: "    << avgResultValue
                              << ", VarResult: "    << varResultValue
                              << std::endl;
                }
            }
        }
    }
    }

    if (rank == 0) {
        resultsFile.close();
    }

    return 0;
}

