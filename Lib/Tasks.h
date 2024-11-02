#include <vector>
#include <utility>
#include <cmath>

class Task
{
private:
    
public:
    //Tasks(/* args */);
    virtual float calculateTask(const std::vector<float> &x) = 0;
};


class Task1: public Task
{
private:
    std::pair<float, float>  calcSumProduct(const std::vector<float> &x);
    
public:
    float calculateTask(const std::vector<float> &x);
};


class Task2: public Task
{
private:
    
public:
    float calculateTask(const std::vector<float> &x);
};
