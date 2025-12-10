#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include "person.hpp"
#include "subject.hpp"

namespace isd{

class Teacher: public Person {
public:
    std::map<std::string,std::vector<Subject>,std::less<>> groupSubjects_;

    Teacher() = default;
    Teacher(std::string n,int a): Person(std::move(n),a) {}
    ~Teacher() override = default;
};

}
