#pragma once
#include <string>
#include <map>
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
    std::string name_;
    int age_{0};
    std::string groupName_;
    std::map<std::string,SubjectRecord> records_;

    Student() = default;
    Student(std::string n,int a,std::string g)
        : name_(std::move(n)),age_(a),groupName_(std::move(g)) {
    }
    ~Student() = default;
};

}
