#include "./Pso.h"
#include "./Task.h"
#include <iostream>
#include <chrono>

int main()
{
    std::cout << "Initializing...\n";
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();
    int particleSize = 100;
    int particleAmount = 100000;
    auto task = t2;
    std::vector<float> params = { 1.3f, 1.5f, 0.8f };
    bool vis = false;
    std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, particleAmount, params[0], params[1], params[2]);
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<float> min = pso -> findMin(100, 1e-6, task==t1);
    auto endTime = std::chrono::high_resolution_clock::now();
    for (float x : min)
    {
        std::cout << x << ", ";
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << std::endl;
    std::cout << "function value: " << task->calculateTask(min) << std::endl;
    std::cout << "time: " << duration;
}
