#include "Tasks.h"

float Task1::calculateTask(const std::vector<float> &x)
{
    std::pair<float, float> sumProduct = calcSumProduct(x);
    return sumProduct.first/40.0f + 1.0f - sumProduct.second;
}

std::pair<float, float>  Task1::calcSumProduct(const std::vector<float> &x)
{
    std::pair<float, float> sumProduct (0.0f, 1.0f);
    for (size_t i=0; i < x.size(); ++i)
    {
        sumProduct.first +=  x[i]*x[i];
        sumProduct.second *= std::cos(x[i]/((float)i+1.0f));
    }
    return sumProduct;
}

std::pair<float, float> Task1::getClosedInterval()
{
    return std::pair<float, float>(-40.0f, 40.0f);
}

float Task2::calculateTask(const std::vector<float> &x)
{
    float sum = 0.0f;
    for (size_t i=0; i + 1 < x.size(); ++i)
    {
        sum += 100.0f*std::pow((x[i+1] -x[i]*x[i]), 2) + std::pow(1.0f-x[i], 2);
    }
    return sum;
}

std::pair<float, float> Task2::getClosedInterval()
{
    return std::pair<float, float>(-10.0f, 10.0f);
}
