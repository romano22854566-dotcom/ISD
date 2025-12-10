#pragma once
#include <string>

namespace isd{

struct Subject {
    std::string name;
    Subject() = default;
    explicit Subject(std::string n): name(std::move(n)) {}
    ~Subject() = default;
    friend bool operator==(const Subject& a,const Subject& b) = default;
};

}
