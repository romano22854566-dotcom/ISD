#pragma once
#include "group.hpp"
#include "exceptions.hpp"

namespace isd{

class GroupService {
public:
    static const std::string& name(const Group& g) { return g.name_; }
    static const std::string& specialty(const Group& g) { return g.specialtyName_; }

    static void validate(const Group& g) {
        if (g.name_.empty()) throw ISDException("Пустое имя группы");
        if (g.specialtyName_.empty()) throw ISDException("Пустое имя специальности");
    }
    static void setSpecialty(Group& g,std::string s) {
        if (s.empty()) throw ISDException("Пустое имя специальности");
        g.specialtyName_ = std::move(s);
    }
};

}
