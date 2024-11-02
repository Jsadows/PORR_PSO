#include "Tasks.h"

float Task1::calculateTask(const std::vector<float> &x)
{
    std::pair<float, float> sumProduct = calcSumProduct(x);
    return = sumProduct.first/40.0f + 1.0f - sumProduct.second;
}

std:pair<float, float>  Task1::calcSumProduct(const std::vector<float> &x)
{
    std::pair<float, float> sumProduct (0.0f, 0.0f);
    for (i=0; i < x.size(); ++i)
    {
        sumProduct.first +=  x[i]*x[i];
        sumProduct.second *= std:cos(x[i]/i);
    }
    return sumProduct;
}

float Task2::calculateTask(const std::vector<float> &x)
{
    float sum = 0.0f;
    for (i=0; i < x.size()-1; ++i)
    {
        sum += 100.0f*std::pow((x[i+1] -x[i]*x[i]), 2) + std::pow(1-x[i], 2)
    }
    return = sum;
}