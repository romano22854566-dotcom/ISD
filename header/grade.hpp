#pragma once
#include <vector>

namespace isd{

template<typename T>
class Grade {
public:
    std::vector<T> vals_;
    Grade() = default;
    ~Grade() = default;
};

}
