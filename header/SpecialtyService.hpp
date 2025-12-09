#pragma once
#include "specialty.hpp"
#include "exceptions.hpp"
#include <algorithm>

namespace isd{

class SpecialtyService {
public:
    static const std::string& name(const Specialty& sp) { return sp.name_; }

    static void validate(const Specialty& sp) {
        if (sp.name_.empty()) throw ISDException("Пустое имя специальности");
    }

    static void addSubject(Specialty& sp,const SpecSubject& s) {
        auto it = std::find_if(sp.subjects_.begin(),sp.subjects_.end(),
                               [&](const SpecSubject& x){return x.name == s.name; });
        if (it != sp.subjects_.end()) throw ISDException("Предмет уже есть в специальности");
        sp.subjects_.push_back(s);
    }

    static void removeSubject(Specialty& sp,const std::string& subj) {
        auto it = std::remove_if(sp.subjects_.begin(),sp.subjects_.end(),
                                 [&](const SpecSubject& x){return x.name == subj; });
        if (it == sp.subjects_.end()) throw ISDException("Предмет не найден");
        sp.subjects_.erase(it,sp.subjects_.end());
    }
};
static void validate(const Specialty& sp) {
    if (sp.name_.empty()) throw ISDException("Пустое имя специальности");
}


}
