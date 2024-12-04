#include "Pso.h"

#define USE_OMP

Pso::Pso(const std::shared_ptr<Task> task, int particleSize, int particleAmount,
	const float c1, const float c2, const float c3)
	: task_(task)
	, c1_(c1)
	, c2_(c2)
	, c3_(c3)
{
	particles_.resize(particleAmount, std::vector<float>(particleSize));
	velocity_.resize(particleAmount, std::vector<float>(particleSize));
	bestParticle_.resize(particleSize);
	bestLocalParticles_.resize(particleAmount, std::vector<float>(particleSize));
	bestLocalParticlesVals_.resize(particleAmount);
	bestParticleVal_ = std::numeric_limits<float>::infinity();
	iterNoBetter_ = -1;
	oldBestVal_ = std::numeric_limits<float>::infinity();
}

std::vector<float> Pso::findMin(int m, float eps, const std::optional<std::vector<float>>& knownBestX)
{
	initParticles();
	std::uniform_real_distribution<> distrR(0, 1);
	std::mt19937 gen(std::random_device{}());
	while (notStopCriterion(m, eps, knownBestX))
	{
        #ifdef USE_OMP
		#pragma omp parallel for schedule(dynamic) default(none) firstprivate(distrR, gen)
        #endif
		for (int i = 0; i < particles_.size(); ++i)
		{
			for (int x=0; x < particles_[i].size(); ++x)
			{
				float r1 = distrR(gen);
				float r2 = distrR(gen);
				float r3 = distrR(gen);
				velocity_[i][x] = c1_ *r1 * velocity_[i][x] + 
					c2_ * r2 * (bestLocalParticles_[i][x] -  particles_[i][x]) +
					c3_ * r3 * (bestParticle_[x] - particles_[i][x]);
				particles_[i][x] += velocity_[i][x];
			}
			float currentVal = task_->calculateTask(particles_[i]);
			if (currentVal < bestLocalParticlesVals_[i])
			{
				bestLocalParticles_[i] = particles_[i];
				bestLocalParticlesVals_[i] = currentVal;
                #ifdef USE_OMP
                #pragma omp critical
                #endif
                {
                    if (currentVal < bestParticleVal_) {
                        bestParticleVal_ = currentVal;
                        bestParticle_ = particles_[i];
                    }
                }
			}
		}
		std::cout<< bestParticleVal_ <<std::endl;
	}
	return bestParticle_;
}

void Pso::initParticles()
{
	iterNoBetter_ = -1;
	std::pair<float, float> interval = task_->getClosedInterval();
	std::uniform_real_distribution<> distrStartVal(interval.first, interval.second);
	float absIntervalDist = std::abs(interval.second - interval.first);
	std::uniform_real_distribution<> distrStartVelocity(-absIntervalDist, absIntervalDist); //Start speeds could be too high
	bestParticleVal_ = std::numeric_limits<float>::infinity();
	std::mt19937 gen(std::random_device{}());
    #ifdef USE_OMP
	#pragma omp parallel for schedule(dynamic) default(none) firstprivate(distrStartVal, distrStartVelocity, gen)
	#endif
    for (int i = 0; i < particles_.size(); ++i)
	{
		for (int x=0; x <  particles_[i].size(); ++x)
		{
			particles_[i][x] = distrStartVal(gen);
			velocity_[i][x] = distrStartVelocity(gen);
		}
		float taskValue = task_->calculateTask(particles_[i]);
		bestLocalParticles_[i] = particles_[i];
		bestLocalParticlesVals_[i] = taskValue;
        if (taskValue < bestParticleVal_)
        {
            #ifdef USE_OMP
            #pragma omp critical
            #endif
            {
                bestParticleVal_ = taskValue;
                bestParticle_ = particles_[i];
                oldBestVal_ = taskValue;
            }
        }
	}
}

bool Pso::notStopCriterion(int m, float eps, const std::optional<std::vector<float>>& knownBestX)
{
	if(knownBestX)
	{
		return std::abs(bestParticleVal_ - task_->calculateTask(*knownBestX)) > eps;
	}
	else
	{
		if (std::abs(oldBestVal_ - bestParticleVal_) < eps)
		{
			++iterNoBetter_;
		}
		else
		{
			iterNoBetter_ = 0;
			oldBestVal_ = bestParticleVal_;
		}
		return iterNoBetter_ < m;
	}
}