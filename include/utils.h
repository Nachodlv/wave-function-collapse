#pragma once
#include <cassert>
#include <cstdlib>

class Utils
{
public:
    static float get_random_number(const int low, const int high)
    {
        if (low == high) return low;
        return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (high - low)));
    }
};
