#pragma once
#include <vector>

namespace isd{

template<typename T>
class Grade {
public:
    std::vector<T> vals;
    Grade() = default;
    ~Grade() = default;
};

}
