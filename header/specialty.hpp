#pragma once
#include <string>
#include <vector>

namespace isd{

enum class ClassType { LK,PZ,LR };
enum class ControlType { Zachet,Exam };

struct SpecSubject {
    std::string name;
    ControlType control{ControlType::Zachet};
    std::vector<ClassType> classTypes;

    SpecSubject() = default;
    SpecSubject(std::string n,ControlType ct,std::vector<ClassType> types)
        : name(std::move(n)),control(ct),classTypes(std::move(types)) {
    }
    ~SpecSubject() = default;
};

class Specialty {
public:
    std::string name;
    std::vector<SpecSubject> subjects;

    Specialty() = default;
    explicit Specialty(std::string n): name(std::move(n)) {}
    ~Specialty() = default;
};

}
