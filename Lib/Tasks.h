#include <vector>
#include <utility>
#include <cmath>

class Task
{
public:
    virtual float calculateTask(const std::vector<float> &x) = 0;
    virtual  std::pair<float, float> getClosedInterval() = 0;
};


class Task1: public Task
{
public:
    float calculateTask(const std::vector<float> &x);
    virtual  std::pair<float, float> getClosedInterval();

private:
    std::pair<float, float>  calcSumProduct(const std::vector<float>& x);

};


class Task2: public Task
{
public:
    float calculateTask(const std::vector<float> &x);
    virtual  std::pair<float, float> getClosedInterval();
};
