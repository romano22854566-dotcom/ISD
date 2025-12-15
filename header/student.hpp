#pragma once
#include <string>
#include <map>
#include <functional>
#include "grade.hpp"
#include "specialty.hpp"

namespace isd{

struct SubjectRecord {
    std::map<ClassType,Grade<int>> grades;
    std::map<ClassType,int> absences;

    SubjectRecord() = default;
    ~SubjectRecord() = default;
};

class Student {
public:
    std::string name;
    int age{0};
    std::string groupName;
    std::map<std::string,SubjectRecord,std::less<>> records;

    Student() = default;
    Student(std::string n,int a,std::string g)
        : name(std::move(n)),age(a),groupName(std::move(g)) {
    }
    ~Student() = default;
};

}
