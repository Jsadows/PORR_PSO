#include "Lib/Tasks.h"
#include <iostream>
#include <vector>
#include <memory>

int main()
{
    std::vector<float> x = {11.0f, 13.4f, 2.0f, 4.9f};
    std::unique_ptr t1 = make_unique<Task1>();
    std::unique_ptr t2 = make_unique<Task2>();
    std::cout<<t1->calculateTask(x)<<std:endl;
    std::cout<<t2->calculateTask(x)<<std:endl;
}
