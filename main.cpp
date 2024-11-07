#include "Lib/Tasks.h"
#include "Lib/Pso.h"
#include <iostream>
#include <vector>
#include <memory>

int main()
{
    std::vector<float> x = {1.0f, 1.0f, 1.0f, 1.0f};
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();
    std::cout<<t1->calculateTask(x)<<std::endl;
    std::cout<<t2->calculateTask(x)<<std::endl;
    std::unique_ptr<Pso> pso = std::make_unique<Pso>(t2, 3, 500);
    auto odp = pso->findMin(20, 0.01, x );
    std::cout <<  t2->calculateTask(odp) << std::endl;
    std::cout << "wektor" << std::endl;
    for (auto x : odp)
    {
        std::cout<<x << " ";
    }
}
