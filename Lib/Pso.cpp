#include "Pso.h"


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
	betterParticles_.resize(particleAmount, std::vector<float>(particleSize));
	betterParticlesVals_.resize(particleAmount);
}

std::vector<float> Pso::findMin()
{
	initParticles();
	// TODO inicjalizacja velocity
	// TODO: obroty algorytmu i wyznaczanie nastêpnych kroków

	return bestParticle_;
}

void Pso::initParticles()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::pair<float, float> interval = task_->getClosedInterval();
	std::uniform_real_distribution<> distrStartVal(interval.first, interval.second);
	bestParticleVal_ = std::numeric_limits<float>::infinity();
	for (int i = 0; i < particles_.size(); ++i)
	{
		for (float& x : particles_[i])
		{
			x = distrStartVal(gen);
		}
		float taskValue = task_->calculateTask(particles_[i]);
		betterParticles_[i] = particles_[i];
		betterParticlesVals_[i] = taskValue;
		if (taskValue < bestParticleVal_)
		{
			bestParticleVal_ = taskValue;
			bestParticle_ = particles_[i];
		}
	}
}
