#include "Tasks.h"
#include <memory>
#include <vector>
#include <cmath>
#include <random>
#include <utility>
#include <limits>

class Pso
{
public:
	Pso(const std::shared_ptr<Task> task, int particleSize=50, int particleAmount=500, 
		const float c1 = 1.0f,const float c2=1.0f, const float c3=3.0f);
	std::vector<float> findMin();
private:
	void initParticles();
	const std::shared_ptr<Task> task_;
	float bestParticleVal_;
	std::vector<float> bestParticle_;
	std::vector<std::vector<float>> betterParticles_;
	std::vector<float> betterParticlesVals_;
	std::vector<std::vector<float>> velocity_;
	std::vector<std::vector<float>> particles_;
	const float c1_;
	const float c2_;
	const float c3_;
};