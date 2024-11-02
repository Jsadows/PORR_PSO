#include "Lib/Tasks.h"
#include <iostream>
#include <vector>
#include <memory>

int main()
{
    std::vector<float> x = {1.0f, 1.0f, 1.0f, 1.0f};
    std::unique_ptr<Task> t1 = std::make_unique<Task1>();
    std::unique_ptr<Task> t2 = std::make_unique<Task2>();
    std::cout<<t1->calculateTask(x)<<std::endl;
    std::cout<<t2->calculateTask(x)<<std::endl;
}
