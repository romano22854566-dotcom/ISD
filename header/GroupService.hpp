#pragma once
#include "group.hpp"
#include "exceptions.hpp"
#include <string_view>

namespace isd{

class GroupService {
public:
    static std::string_view name(const Group& g) { return g.name; }
    static std::string_view specialty(const Group& g) { return g.specialtyName; }

    static void validate(const Group& g) {
        if (g.name.empty()) throw ISDException("Пустое имя группы");
        if (g.specialtyName.empty()) throw ISDException("Пустое имя специальности");
    }
    static void setSpecialty(Group& g,std::string_view s) {
        if (s.empty()) throw ISDException("Пустое имя специальности");
        g.specialtyName.assign(s.data(),s.size());
    }
};

}
