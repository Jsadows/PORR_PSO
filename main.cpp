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
/*
    //setup for performance
    int particleSize = 100;
    int particleAmount = 100000;
    auto task = t2;
//    std::vector<float> params = {1.3f, 1.5f, 0.8f};
    std::vector<float> params = {1.3f, 0.8f, 0.8f};   //better for big problems
    bool vis = false;
*/
    //setup for visualise
   int particleSize = 2;
   int particleAmount = 50;
   int theads_nb = 8;
   auto task = t2;
   std::vector<float> params = {1.3f, 1.5f, 0.8f};
   bool vis = true;

    std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, particleAmount, params[0], params[1], params[2]);

    std::vector<float> odp;
    if(vis){
        assert((void("Wrong particle size for visualise"),particleSize == 2));
        std::ofstream visualiseFile("visualisation.csv");
        visualiseFile << particleAmount << std::endl;
        odp = pso->findMin(200, 0.01, std::nullopt, theads_nb, visualiseFile);
        visualiseFile.close();
    }
    else{
        odp = pso->findMin(100, 1e-6);
//        odp = pso->findMin(100, 1e-2,std::vector<float> (particleSize, 0.0));
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
//    basic_run();
    threads_test_run();
    // x5_best_test_run();
    // testing_run();
    return 0;
}
