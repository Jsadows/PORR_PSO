#include "Pso.h"


Pso::Pso(const std::shared_ptr<Task> task, int particleSize, int particleAmount,
	const float c1, const float c2, const float c3)
	: task_(task)
	, c1_(c1)
	, c2_(c2)
	, c3_(c3)
	, gen_(std::random_device{}())
{
	particles_.resize(particleAmount, std::vector<float>(particleSize));
	velocity_.resize(particleAmount, std::vector<float>(particleSize));
	bestParticle_.resize(particleSize);
	betterParticles_.resize(particleAmount, std::vector<float>(particleSize));
	betterParticlesVals_.resize(particleAmount);
}

std::vector<float> Pso::findMin(int m, float eps, const std::optional<std::vector<float>>& knownBestX)
{
	initParticles();
	// TODO: obroty algorytmu i wyznaczanie nast�pnych krok�w
	std::uniform_real_distribution<> distrR(0, 1);
	while (notStopCriterion(m, eps, knownBestX))
	{
		for (int i = 0; i < particles_.size(); ++i)
		{
			for (int x=0; x <  particles_[i].size(); ++i)
			{
				float r1 = distrR(gen_);
				float r2 = distrR(gen_);
				float r3 = distrR(gen_);
				velocity_[i][x] = c1_ *r1 * velocity_[i][x] + 
					c2_ * r2 * (betterParticles_[i][x] -  particles_[i][x]) +
					c3_ * r3 * (bestParticle_[x] - particles_[i][x]);
				particles_[i][x] += velocity_[i][x];
			}
			float currentVal = task_->calculateTask(particles_[i]);
			if (currentVal > betterParticlesVals_[i])
			{
				betterParticles_[i] = particles_[i];
				bestParticleVal_ = currentVal;
				if (currentVal > bestParticleVal_ )
				{
					bestParticleVal_ = currentVal;
					bestParticle_ = particles_[i];
				}
			}
		}
	
	}
	return bestParticle_;
}

void Pso::initParticles()
{
	iterNoBetter_ = -1;
	std::pair<float, float> interval = task_->getClosedInterval();
	std::uniform_real_distribution<> distrStartVal(interval.first, interval.second);
	float absIntervalDist = std::abs(interval.second - interval.first);
	std::uniform_real_distribution<> distrStartVelocity(-absIntervalDist, absIntervalDist);
	bestParticleVal_ = std::numeric_limits<float>::infinity();
	for (int i = 0; i < particles_.size(); ++i)
	{
		for (int x=0; x <  particles_[i].size(); ++i)
		{
			particles_[i][x] = distrStartVal(gen_);
			velocity_[i][x] = distrStartVelocity(gen_);
		}
		float taskValue = task_->calculateTask(particles_[i]);
		betterParticles_[i] = particles_[i];
		betterParticlesVals_[i] = taskValue;
		if (taskValue < bestParticleVal_)
		{
			bestParticleVal_ = taskValue;
			bestParticle_ = particles_[i];
			oldBestVal_ = taskValue;
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
		if (std::abs(oldBestVal_ - bestParticleVal_) < eps) ++m;
		oldBestVal_ = bestParticleVal_;
		bool isMSmaller = m < iterNoBetter_;
		if (isMSmaller) m = 0;
		return isMSmaller;
	}
}