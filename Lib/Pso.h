#ifndef PSO_H
#define PSO_H

#include <memory>
#include <vector>
#include <cmath>
#include <random>
#include <utility>
#include <limits>
#include <optional>
#include <iostream>
#include <omp.h>
#include "Tasks.h"

class Pso
{
public:
	Pso(const std::shared_ptr<Task> task, int particleSize=50, int particleAmount=500, 
		const float c1 = 1.0f,const float c2=1.0f, const float c3=3.0f);
	std::vector<float> findMin(int m=5, float eps=0.01f, const std::optional<std::vector<float>>& knownBestX= std::nullopt);
private:
	void initParticles();
	bool notStopCriterion(int m, float eps, const std::optional<std::vector<float>>& knownBestX);
	const std::shared_ptr<Task> task_;
	float bestParticleVal_;
	std::vector<float> bestParticle_;
	std::vector<std::vector<float>> bestLocalParticles_;
	std::vector<float> bestLocalParticlesVals_;
	std::vector<std::vector<float>> velocity_;
	std::vector<std::vector<float>> particles_;
	const float c1_;
	const float c2_;
	const float c3_;
	float oldBestVal_;
	int iterNoBetter_;
};

#endif