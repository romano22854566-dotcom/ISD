#pragma once
#include <string>

namespace isd{

class Group {
public:
    std::string name;
    std::string specialtyName;

    Group() = default;
    explicit Group(std::string n): name(std::move(n)) {}
    ~Group() = default;
};

}
