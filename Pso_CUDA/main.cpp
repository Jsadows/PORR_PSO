#include "./Pso.h"
#include "./Task.h"
#include <iostream>

int main()
{
    std::cout << "Hello World!\n";
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();
    int particleSize = 2;
    int particleAmount = 50;
    int theads_nb = 8;
    auto task = t1;
    std::vector<float> params = { 1.3f, 1.5f, 0.8f };
    bool vis = false;
    std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, particleAmount, params[0], params[1], params[2]);
    std::vector<float> min = pso -> findMin(10, 0.01f);
    for (float x : min)
    {
        std::cout << x << ", ";
    }
    std::cout << std::endl;
    std::cout << t1->calculateTask(min);
}
