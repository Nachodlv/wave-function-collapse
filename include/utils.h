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

    template<typename T>
    static void from_shared_to_raw_pointer(std::vector<std::vector<std::shared_ptr<T>>>& shared_pointers, std::vector<std::vector<const T*>>& raw_pointers)
    {
        for (auto shared_pointer_row : shared_pointers)
        {
            std::vector<const T*> raw_pointer_row;
            for (auto shared_pointer : shared_pointer_row)
            {
                raw_pointer_row.push_back(shared_pointer.get());
            }
            raw_pointers.push_back(raw_pointer_row);
        }
    }
};
