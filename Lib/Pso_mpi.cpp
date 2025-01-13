#include "Pso_mpi.h"

PsoMPI::PsoMPI(std::shared_ptr<Task> task,
               int particleSize,
               int particleAmount,
               float c1,
               float c2,
               float c3,
               int maxIter)
    : task_(std::move(task))
    , c1_(c1)
    , c2_(c2)
    , c3_(c3)
    , maxIter_(maxIter)
    , iter_(0)
{
    // MPI rank and size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize_);

    particles_.resize(particleAmount, std::vector<float>(particleSize, 0.0f));
    velocity_.resize(particleAmount, std::vector<float>(particleSize, 0.0f));
    bestLocalParticles_.resize(particleAmount, std::vector<float>(particleSize, 0.0f));
    bestLocalParticlesVals_.resize(particleAmount, std::numeric_limits<float>::infinity());

    bestParticle_.resize(particleSize, 0.0f);
    bestParticleVal_ = std::numeric_limits<float>::infinity();

    oldBestVal_ = std::numeric_limits<float>::infinity();
}

std::vector<float> PsoMPI::findMin(int m,
                                   float eps,
                                   const std::optional<std::vector<float>>& knownBestX,
                                   std::optional<std::reference_wrapper<std::ostream>> outputStream)
{
    initParticles();

    std::uniform_real_distribution<float> distrR(0.0f, 1.0f);
    std::mt19937 gen(std::random_device{}());

    // Particles for each rank
    int totalParticles = static_cast<int>(particles_.size());
    int localCount     = totalParticles / worldSize_;
    int remainder      = totalParticles % worldSize_;

    int startIndex = rank_ * localCount + std::min(rank_, remainder);
    int endIndex   = (rank_ + 1) * localCount + std::min(rank_ + 1, remainder);

    // Main PSO loop
    while (notStopCriterion(m, eps, knownBestX))
    {
        // Update local subset of particles
        for (int i = startIndex; i < endIndex; ++i)
        {
            for (int x = 0; x < (int)particles_[i].size(); ++x)
            {
                float r1 = distrR(gen);
                float r2 = distrR(gen);
                float r3 = distrR(gen);

                velocity_[i][x] = c1_ * r1 * velocity_[i][x]
                                  + c2_ * r2 * (bestLocalParticles_[i][x] - particles_[i][x])
                                  + c3_ * r3 * (bestParticle_[x] - particles_[i][x]);

                particles_[i][x] += velocity_[i][x];
            }

            // Evaluate cost for local subset of particles
            float currentVal = task_->calculateTask(particles_[i]);
            if (currentVal < bestLocalParticlesVals_[i])
            {
                bestLocalParticles_[i]     = particles_[i];
                bestLocalParticlesVals_[i] = currentVal;
            }
        }

        // Finding local best 
        float localBestVal  = bestParticleVal_;
        std::vector<float> localBestPos = bestParticle_;  

        // Checking local subset to see if any local best is better than known global
        for (int i = startIndex; i < endIndex; ++i)
        {
            if (bestLocalParticlesVals_[i] < localBestVal)
            {
                localBestVal = bestLocalParticlesVals_[i];
                localBestPos = bestLocalParticles_[i];
            }
        }

        // Allreduce to find the global best value among ranks 
        struct {
            float val;
            int   rank;
        } inData, outData;

        inData.val = localBestVal;
        inData.rank = rank_;

        MPI_Allreduce(&inData, &outData, 1, MPI_FLOAT_INT, MPI_MINLOC, MPI_COMM_WORLD);

        float globalBestVal = outData.val;

        if (rank_ == outData.rank)
        {
            localBestPos = localBestPos; 
        }

        // Broadcast global best position from the winning rank
        MPI_Bcast(localBestPos.data(), (int)localBestPos.size(), MPI_FLOAT, outData.rank, MPI_COMM_WORLD);

        // Ranks update global best 
        bestParticleVal_ = globalBestVal;
        bestParticle_    = localBestPos;

        // Logging
        if (outputStream.has_value() && rank_ == 0)
        {
            outputStream->get() << "Iteration: " << iter_
                                << ", Global Best Val: " << bestParticleVal_ << std::endl;
        }

        ++iter_;
        bestHistory_.push_back(bestParticleVal_);
    }
    return bestParticle_;
}

void PsoMPI::initParticles()
{
    // Particles per rank
    int totalParticles = static_cast<int>(particles_.size());
    int localCount     = totalParticles / worldSize_;
    int remainder      = totalParticles % worldSize_;

    int startIndex = rank_ * localCount + std::min(rank_, remainder);
    int endIndex   = (rank_ + 1) * localCount + std::min(rank_ + 1, remainder);

    std::pair<float, float> interval = task_->getClosedInterval();
    float minVal = interval.first;
    float maxVal = interval.second;
    float absIntervalDist = std::abs(maxVal - minVal);

    std::uniform_real_distribution<float> distrStartVal(minVal, maxVal);
    std::uniform_real_distribution<float> distrStartVelocity(-absIntervalDist, absIntervalDist);
    std::mt19937 gen(std::random_device{}());

    // Each rank initializes only its local subset
    for (int i = startIndex; i < endIndex; ++i)
    {
        for (int x = 0; x < (int)particles_[i].size(); ++x)
        {
            particles_[i][x] = distrStartVal(gen);
            velocity_[i][x]  = distrStartVelocity(gen);
        }

        float taskValue = task_->calculateTask(particles_[i]);
        bestLocalParticles_[i]     = particles_[i];
        bestLocalParticlesVals_[i] = taskValue;
    }

    // Each rank finds local best
    float localBestVal = std::numeric_limits<float>::infinity();
    std::vector<float> localBestPos( particles_[0].size(), 0.0f );

    for (int i = startIndex; i < endIndex; ++i)
    {
        if (bestLocalParticlesVals_[i] < localBestVal)
        {
            localBestVal = bestLocalParticlesVals_[i];
            localBestPos = bestLocalParticles_[i];
        }
    }

    // Finding global best
    struct {
        float val;
        int   rank;
    } inData, outData;

    inData.val = localBestVal;
    inData.rank = rank_;

    MPI_Allreduce(&inData, &outData, 1, MPI_FLOAT_INT, MPI_MINLOC, MPI_COMM_WORLD);

    // Broadcasting best position from the winning rank

    MPI_Bcast(localBestPos.data(), (int)localBestPos.size(), MPI_FLOAT, outData.rank, MPI_COMM_WORLD);

    //  Ranks update global best
    bestParticleVal_ = outData.val;
    bestParticle_    = localBestPos;
    oldBestVal_      = bestParticleVal_;

    iter_ = 0;
    bestHistory_.clear();
    bestHistory_.push_back(bestParticleVal_);
}

bool PsoMPI::notStopCriterion(int m, float eps, const std::optional<std::vector<float>>& knownBestX)
{
    // maxIter stop
    if (iter_ > maxIter_) {
        return false;
    }

    // Evaluate the stopping condition consistently on every rank.
    bool keepGoing = true;

    // distance stop
    if (knownBestX)
    {
        float sum = std::inner_product(bestParticle_.begin(),
                                       bestParticle_.end(),
                                       knownBestX->begin(),
                                       0.0f,
                                       std::plus<>(),
                                       [](float a, float b) { return (a - b) * (a - b); });

        float dist = std::sqrt(sum);
        keepGoing  = (dist > eps);
    }
    else
    {
        // m values stop
        if (iter_ >= m) {
            oldBestVal_ = bestHistory_[iter_ - m];
            if (std::abs(oldBestVal_ - bestParticleVal_) < eps) {
                keepGoing = false;
            }
        }
    }

    // We want all ranks to agree whether to continue or not
    int localFlag = (keepGoing) ? 1 : 0;
    int globalFlag = 0;

    MPI_Allreduce(&localFlag, &globalFlag, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    // if one rank sets localFlag=0 => globalFlag=0 => stop.

    return (globalFlag == 1);
}
