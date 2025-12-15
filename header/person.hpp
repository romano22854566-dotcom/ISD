#pragma once
#include <string>
#include <memory>

namespace isd{

class Person {
private:
    std::string name;
    int age_{0};
public:
    Person() = default;
    Person(std::string n,int a): name(std::move(n)),age_(a) {}
    virtual ~Person() = default;

    const std::string& fullName() const { return name; }
    int age() const { return age_; }
};

}