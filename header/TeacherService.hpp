#pragma once
#include "teacher.hpp"
#include "exceptions.hpp"
#include <algorithm>

namespace isd{

class TeacherService {
public:
    static const std::string& fullName(const Teacher& t) { return t.fullName(); }
    static int age(const Teacher& t) { return t.age(); }

    static void validate(const Teacher& t) {
        if (t.fullName().empty()) throw ISDException("Пустое имя преподавателя");
        if (t.age() < 18 || t.age() > 120) throw ISDException("Возраст преподавателя некорректен");
    }

    static void addSubject(Teacher& t,const std::string& group,const Subject& s) {
        auto& vec = t.groupSubjects_[group];
        if (std::find(vec.begin(),vec.end(),s) != vec.end())
            throw ISDException("Предмет уже добавлен");
        vec.push_back(s);
    }

    static void removeSubject(Teacher& t,const std::string& group,const std::string& subjName) {
        auto& vec = t.groupSubjects_[group];
        std::erase_if(vec,[&subjName](const Subject& s){ return s.name == subjName; });


    }
};

}
