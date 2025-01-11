#pragma once

#include <memory>
#include <vector>
#include <cmath>
#include <random>
#include <utility>
#include <limits>
#include <optional>
#include <iostream>
#include <fstream>
#include <cassert>
#include <functional>
#include <numeric>
#include <iostream>
#include <curand_kernel.h>
#include "./Task.h"
#include "./Points.cuh"

class Pso
{
public:
	Pso(const std::shared_ptr<Task> task, int particleSize = 50, int particleAmount = 500,
		const float c1 = 1.0f, const float c2 = 1.0f, const float c3 = 3.0f);
	std::vector<float> findMin(int m = 5, float eps = 0.01f, bool taskIs1=true,const std::optional<std::vector<float>>& knownBestX = std::nullopt, std::optional<std::reference_wrapper<std::ostream>> visualiseFile = std::nullopt);
private:
	bool notStopCriterion(int m, float eps, const std::optional<std::vector<float>>& knownBestX);
	float bestParticleVal_;
	const std::shared_ptr<Task> task_;
	std::vector<float> bestParticle_;
	const float c1_;
	const float c2_;
	const float c3_;
	float oldBestVal_;
	int maxIter_;
	std::vector<float> bestHistory_;
	int iter;
	int particleSize_;
	int particleAmount_;
	int blockSize_;
};