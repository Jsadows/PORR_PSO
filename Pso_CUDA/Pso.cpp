#include "./Pso.h"


Pso::Pso(const std::shared_ptr<Task> task, int particleSize, int particleAmount,
    const float c1, const float c2, const float c3)
    : task_(task)
    , c1_(c1)
    , c2_(c2)
    , c3_(c3)
    , particleSize_(particleSize)
    , particleAmount_(particleAmount)
{
    bestParticle_.resize(particleSize);
    bestParticleVal_ = std::numeric_limits<float>::infinity();
    bestHistory_ = {};
    iter = 0;
    maxIter_ = 500;
    oldBestVal_ = std::numeric_limits<float>::infinity();
    std::cout.precision(3);
    blockSize_ = 128;
}

std::vector<float> Pso::findMin(int m, float eps, bool taskIs1, const std::optional<std::vector<float>>& knownBestX, std::optional<std::reference_wrapper<std::ostream>> visualiseFile)
{
    std::pair<float, float> interval = task_->getClosedInterval();
    initGPU(particleAmount_, particleSize_, blockSize_, interval.first, interval.second, taskIs1);


    while (notStopCriterion(m, eps, knownBestX))
    {
        updateP(particleSize_, particleAmount_, c1_, c2_, c3_, blockSize_, taskIs1);
        syncResultsToHost(bestParticle_, bestParticleVal_);
        //std::cout <<"On cpu:"<< bestParticleVal_ << std::endl;
    }

    syncResultsToHost(bestParticle_, bestParticleVal_);

    freeGPU();

    return bestParticle_;
}


bool Pso::notStopCriterion(int m, float eps, const std::optional<std::vector<float>>& knownBestX)
{
    if (iter > maxIter_) return false;

    if (knownBestX)
    {
        float sum = std::inner_product(bestParticle_.begin(), bestParticle_.end(), knownBestX->begin(), 0.0f,
            std::plus<>(),
            [](float a, float b) { return (a - b) * (a - b); }
        );
        iter++;
        return std::sqrt(sum) > eps;
    }
    else
    {
        if (iter >= m) {
            oldBestVal_ = bestHistory_[iter - m];
            if (std::abs(oldBestVal_ - bestParticleVal_) < eps) return false;
        }
        iter++;
        bestHistory_.push_back(bestParticleVal_);
        return true;
    }
}
