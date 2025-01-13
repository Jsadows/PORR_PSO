#ifndef PSO_MPI_H
#define PSO_MPI_H

#include <memory>
#include <vector>
#include <cmath>
#include <random>
#include <utility>
#include <limits>
#include <optional>
#include <iostream>
#include <functional>
#include <numeric> 
#include "Tasks.h"
#include "mpi.h"


class PsoMPI
{
public:
    PsoMPI(std::shared_ptr<Task> task,
           int particleSize    = 50,
           int particleAmount  = 500,
           float c1            = 1.0f,
           float c2            = 1.0f,
           float c3            = 3.0f,
           int maxIter         = 500);

    std::vector<float> findMin(int m = 5,
                               float eps = 0.01f,
                               const std::optional<std::vector<float>>& knownBestX = std::nullopt,
                               std::optional<std::reference_wrapper<std::ostream>> outputStream = std::nullopt);

private:
    void initParticles();

    bool notStopCriterion(int m, float eps, const std::optional<std::vector<float>>& knownBestX);

private:
    std::shared_ptr<Task> task_;

    float c1_;
    float c2_;
    float c3_;
    int maxIter_;

    std::vector<std::vector<float>> particles_; 
    std::vector<std::vector<float>> velocity_;  
    std::vector<std::vector<float>> bestLocalParticles_;
    std::vector<float>             bestLocalParticlesVals_;

    // Global best data
    std::vector<float> bestParticle_;
    float              bestParticleVal_;

    float oldBestVal_;
    std::vector<float> bestHistory_;
    int iter_;

    int rank_;
    int worldSize_;

};

#endif 