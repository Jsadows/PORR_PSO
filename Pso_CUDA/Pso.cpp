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
    particles_.resize(particleAmount * particleSize);
    velocity_.resize(particleAmount * particleSize);
    bestParticle_.resize(particleSize);
    bestLocalParticles_.resize(particleAmount * particleSize);
    bestLocalParticlesVals_.resize(particleAmount);
    bestParticleVal_ = std::numeric_limits<float>::infinity();
    bestHistory_ = {};
    iter = 0;
    maxIter_ = 500;
    oldBestVal_ = std::numeric_limits<float>::infinity();
    std::cout.precision(3);
}

std::vector<float> Pso::findMin(int m, float eps, const std::optional<std::vector<float>>& knownBestX, int threads_nb, std::optional<std::reference_wrapper<std::ostream>> visualiseFile)
{
    initParticles(threads_nb);
    while (notStopCriterion(m, eps, knownBestX))
    {
        updateP(particles_, velocity_, bestParticle_, bestLocalParticles_, bestLocalParticlesVals_, particleSize_, particleAmount_, c1_, c2_, c3_, 256, true);
        
        for (int i = 0; i < particleAmount_; ++i)
        {
            if (bestParticleVal_ > bestLocalParticlesVals_[i])
            {
                for (int j = 0; j < particleSize_; ++j)
                {
                    bestParticle_[j] = bestLocalParticles_[i * particleSize_ + j];
                }
                //std::cout << bestParticleVal_;
                bestParticleVal_ = bestLocalParticlesVals_[i];
            }
        }
    }
    return bestParticle_;
}



void Pso::initParticles(int threads_nb)
{
    std::pair<float, float> interval = task_->getClosedInterval();
    std::uniform_real_distribution<> distrStartVal(interval.first, interval.second);
    float absIntervalDist = std::abs(interval.second - interval.first);
    std::uniform_real_distribution<> distrStartVelocity(-absIntervalDist, absIntervalDist); //Start speeds could be too high
    bestParticleVal_ = std::numeric_limits<float>::infinity();
    std::mt19937 gen(std::random_device{}());
    for (int i = 0; i < particleAmount_; ++i)
    {
        std::vector<float> particle(particleSize_);
        for (int x = 0; x < particleSize_; ++x)
        {
            particles_[i * particleSize_ + x] = distrStartVal(gen);
            velocity_[i * particleSize_ + x] = distrStartVelocity(gen);
            particle[x] = particles_[i * particleSize_ + x];
        }
        float taskValue = task_->calculateTask(particle);
        std::copy(particle.begin(), particle.end(), bestLocalParticles_.begin() + i * particleSize_);
        bestLocalParticlesVals_[i] = taskValue;
        if (taskValue < bestParticleVal_)
        {
            bestParticleVal_ = taskValue;
            bestParticle_ = particle;
            oldBestVal_ = taskValue;
        }
    }
    iter++;
    bestHistory_.push_back(bestParticleVal_);
    std::cout << bestParticleVal_ << std::endl;
    for (float x : bestParticle_)
    {
        std::cout << x << ", ";
    }
    std::cout << std::endl;
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
