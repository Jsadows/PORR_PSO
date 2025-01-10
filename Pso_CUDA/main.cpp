#include "./Pso.h"
#include "./Task.h"
#include <iostream>
#include <chrono>

int main()
{
    std::cout << "Hello World!\n";
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();
    int particleSize = 50;
    int particleAmount = 100000;
    int theads_nb = 8;
    auto task = t2;
    std::vector<float> params = { 1.3f, 1.5f, 0.8f };
    bool vis = false;
    std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, particleAmount, params[0], params[1], params[2]);
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<float> min = pso -> findMin(10, 0.001f,false);
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
