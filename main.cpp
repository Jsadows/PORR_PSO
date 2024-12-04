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
    std::unique_ptr<Pso> pso = std::make_unique<Pso>(t2, 10, 100000, 1.0f, 2.0f, 2.0f);
//    std::unique_ptr<Pso> pso = std::make_unique<Pso>(t2, 2, 100, 1.0f, 2.0f, 2.0f);
    auto odp = pso->findMin(200, 0.01 );
    std::cout <<  t2->calculateTask(odp) << std::endl;
    std::cout << "wektor" << std::endl;
    for (auto x : odp)
    {
        std::cout<<x << " ";
    }
}
