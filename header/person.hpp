#pragma once
#include <string>
#include <memory>

namespace isd{

class Person {
private:
    std::string name_;
    int age_{0};
public:
    Person() = default;
    Person(std::string n,int a): name_(std::move(n)),age_(a) {}
    virtual ~Person() = default;

    const std::string& fullName() const { return name_; }
    int age() const { return age_; }
};

}
