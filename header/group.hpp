#pragma once
#include <string>

namespace isd{

class Group {
public:
    std::string name_;
    std::string specialtyName_;

    Group() = default;
    explicit Group(std::string n): name_(std::move(n)) {}
    ~Group() = default;
};

}
