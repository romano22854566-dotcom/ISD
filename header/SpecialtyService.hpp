#pragma once
#include "specialty.hpp"
#include "exceptions.hpp"
#include <algorithm>

namespace isd{

class SpecialtyService {
public:
    static const std::string& name(const Specialty& sp) { return sp.name; }

    static void validate(const Specialty& sp) {
        if (sp.name.empty()) throw ISDException("Пустое имя специальности");
    }

    static void addSubject(Specialty& sp,const SpecSubject& s) {
        if (auto it = std::find_if(sp.subjects.begin(),sp.subjects.end(),
            [&](const SpecSubject& x){ return x.name == s.name; });
            it != sp.subjects.end()) {
            throw ISDException("Предмет уже есть в специальности");
        }
        sp.subjects.push_back(s);
    }

    static void removeSubject(Specialty& sp,const std::string& subj) {
        if (auto it = std::remove_if(sp.subjects.begin(),sp.subjects.end(),
            [&](const SpecSubject& x){ return x.name == subj; });
            it == sp.subjects.end()) {
            throw ISDException("Предмет не найден");
        }
        else {
            sp.subjects.erase(it,sp.subjects.end());
        }
    }
};

static void validate(const Specialty& sp) {
    if (sp.name.empty()) throw ISDException("Пустое имя специальности");
}

}
