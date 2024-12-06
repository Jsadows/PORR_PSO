#include "Lib/Tasks.h"
#include "Lib/Pso.h"
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include "Lib/Testing.h"

int basic_run()
{
    std::shared_ptr<Task> t1 = std::make_unique<Task1>();
    std::shared_ptr<Task> t2 = std::make_unique<Task2>();

    //setup for performance
    int particleSize = 10;
    int particleAmount = 10000;
    auto task = t2;
    std::vector<float> params = {1.0f, 2.0f, 2.0f};
    bool vis = false;

    //setup for visualise
//    int particleSize = 2;
//    int particleAmount = 50;
//    auto task = t2;
//    std::vector<float> params = {1.0f, 2.0f, 2.0f};
//    bool vis = true;

    std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, particleAmount, params[0], params[1], params[2]);

    std::vector<float> odp;
    if(vis){
        assert((void("Wrong particle size for visualise"),particleSize == 2));
        std::ofstream visualiseFile("visualisation.csv");
        visualiseFile << particleAmount << std::endl;
        odp = pso->findMin(200, 0.01, std::nullopt, visualiseFile);
        visualiseFile.close();
    }
    else{
        odp = pso->findMin(200, 0.01);
    }
    std::cout <<  t2->calculateTask(odp) << std::endl;
    std::cout << "wektor" << std::endl;
    for (auto x : odp)
    {
        std::cout<<x << " ";
    }

    return 0;
}

int main(){
    basic_run();
//    testing_run();
    return 0;
}
