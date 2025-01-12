#include "./Pso.h"
#include "./Task.h"
#include <iostream>
#include <optional>
#include <chrono>

void calculateStatistics(const std::vector<float>& data) {

    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end());
    float minValue = *minIt;
    float maxValue = *maxIt;

    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();

    float variance = 0.0f;
    for (float value : data) {
        variance += std::pow(value - mean, 2);
    }
    variance /= data.size();

    std::cout <<  minValue << ", ";
    std::cout  << maxValue << ", ";
    std::cout  << mean << ", ";
    std::cout  << variance << ", ";
}

void makeTest(std::vector<float> params, std::shared_ptr<Task> task, std::shared_ptr<Task> t1, int particleSize, std::optional<std::vector<float>> knownBestX, std::string t)
{
    std::vector<float> durr1;
    std::vector<float> mins1;
    std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, 102400, params[0], params[1], params[2]);
    for (size_t i = 0; i < 5; i++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<float> min = pso->findMin(100, 1e-6, task == t1);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        durr1.push_back(duration);
        mins1.push_back(task->calculateTask(min));
    }
    std::string isKn = knownBestX ? ", Yes, " : ", No, ";
    std::cout << t+", "<<particleSize<< isKn;
    calculateStatistics(durr1);
    calculateStatistics(mins1);
    std::cout << std::endl;
}

int main()
{
    std::cout << "Initializing...\n";
    std::shared_ptr<Task> t1 = std::make_shared<Task1>();
    std::shared_ptr<Task> t2 = std::make_shared<Task2>();
    int particleSize = 100;
    int particleAmount = 102400;    //particleAmount must be multiple of blockSize
    auto task = t2;
    std::vector<float> params = { 1.3f, 0.8f, 0.8f };
    // T1 tests
    std::vector<float> vec1(10, 0.0f);
    makeTest(params, t1, t1, 10, std::nullopt, "t1");
    makeTest(params, t1, t1, 10, vec1, "t1");
    std::vector<float> vec2(50, 0.0f);
    makeTest(params, t1, t1, 50, std::nullopt, "t1");
    makeTest(params, t1, t1, 50, vec2, "t1");
    std::vector<float> vec3(100, 0.0f);
    makeTest(params, t1, t1, 100, std::nullopt, "t1");
    makeTest(params, t1, t1, 100, vec3, "t1");
    // T2 tests
    std::vector<float> vec4(10, 1.0f);
    makeTest(params, t2, t1, 10, std::nullopt, "t2");
    makeTest(params, t2, t1, 10, vec4, "t2");
    std::vector<float> vec5(50, 1.0f);
    makeTest(params, t2, t1, 50, std::nullopt, "t2");
    makeTest(params, t2, t1, 50, vec5, "t2");
    std::vector<float> vec6(100, 1.0f);
    makeTest(params, t2, t1, 100, std::nullopt, "t2");
    makeTest(params, t2, t1, 100, vec6, "t2");



    /*std::unique_ptr<Pso> pso = std::make_unique<Pso>(task, particleSize, particleAmount, params[0], params[1], params[2]);
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<float> min = pso->findMin(100, 1e-6, task == t1);
    auto endTime = std::chrono::high_resolution_clock::now();
    for (float x : min)
    {
        std::cout << x << ", ";
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << std::endl;
    std::cout << "function value: " << task->calculateTask(min) << std::endl;
    std::cout << "time: " << duration;
    */
}
