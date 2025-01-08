#include "./Task.h"

float Task1::calculateTask(const std::vector<float>& x)
{
    std::pair<float, float> sumProduct = calcSumProduct(x);
    return sumProduct.first / 40.0f + 1.0f - sumProduct.second;
}

float Task1::calculateTask(const float* x, int size)
{
    float sum = 0.0f, product = 1.0f;
    for (int i = 0; i < size; ++i)
    {
        sum += x[i] * x[i];
        product *= std::cos(x[i] / ((float)i + 1.0f));
    }
    return sum / 40.0f + 1.0f - product;
}


std::pair<float, float>  Task1::calcSumProduct(const std::vector<float>& x)
{
    float sum = 0.0f, product = 1.0f;
    for (int i = 0; i < x.size(); ++i)
    {
        sum += x[i] * x[i];
        product *= std::cos(x[i] / ((float)i + 1.0f));
    }
    std::pair<float, float> sumProduct(sum, product);
    return sumProduct;
}

std::pair<float, float> Task1::getClosedInterval()
{
    return std::pair<float, float>(-40.0f, 40.0f);
}

float Task2::calculateTask(const std::vector<float>& x)
{
    float sum = 0.0f;
    for (int i = 0; i < x.size() - 1; ++i)
    {
        sum += 100.0f * std::pow((x[i + 1] - x[i] * x[i]), 2) + std::pow(1.0f - x[i], 2);
    }
    return sum;
}

float Task2::calculateTask(const float* x, int size)
{
    float sum = 0.0f;
    for (int i = 0; i < size - 1; ++i)
    {
        float diff = x[i + 1] - x[i] * x[i];
        sum += 100.0f * std::pow(diff, 2) + std::pow(1.0f - x[i], 2);
    }

    return sum;
}

std::pair<float, float> Task2::getClosedInterval()
{
    return std::pair<float, float>(-10.0f, 10.0f);
}
