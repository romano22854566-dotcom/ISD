#pragma once
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include "student.hpp"
#include "teacher.hpp"
#include "group.hpp"
#include "specialty.hpp"
#include "StudentService.hpp"
#include "TeacherService.hpp"
#include "GroupService.hpp"
#include "SpecialtyService.hpp"

namespace isd{

using Id = size_t;

class Registry {
    std::map<Id,Student> students_;
    std::map<Id,Teacher> teachers_;
    std::map<Id,Group> groups_;
    std::map<Id,Specialty> specialties_;

    Id nextS_{1};
    Id nextT_{1};
    Id nextG_{1};
    Id nextSpec_{1};

    std::string sf_;
    std::string tf_;
    std::string gf_;
    std::string specf_;

public:
    Registry(std::string s,std::string t,std::string g,std::string spec = "specialties.txt")
        : sf_(std::move(s)),
        tf_(std::move(t)),
        gf_(std::move(g)),
        specf_(std::move(spec)) {
    }

    Id addSpecialty(const Specialty& sp) {
        SpecialtyService::validate(sp);
        Id id = nextSpec_;
        ++nextSpec_;
        specialties_.emplace(id,sp);
        return id;
    }
    bool removeSpecialty(Id id) { return specialties_.erase(id) > 0; }
    Specialty* getSpecialtyMutable(Id id) { return &specialties_.at(id); }
    const Specialty* getSpecialty(Id id) const { return &specialties_.at(id); }
    std::vector<Id> allSpecialtyIds() const {
        std::vector<Id> v;
        v.reserve(specialties_.size());
        for (auto const& [sid,_] : specialties_) v.push_back(sid);
        return v;
    }
    const Specialty* findSpecialty(std::string_view name) const {
        for (auto const& [_,sp] : specialties_) {
            if (sp.name_ == name) return &sp;
        }
        return nullptr;
    }

    Id addStudent(const Student& s) {
        StudentService::validate(s);
        Id id = nextS_;
        ++nextS_;
        students_.emplace(id,s);
        return id;
    }
    bool removeStudent(Id id) { return students_.erase(id) > 0; }
    Student* getStudentMutable(Id id) { return &students_.at(id); }
    const Student* getStudent(Id id) const { return &students_.at(id); }
    std::vector<Id> allStudentIds() const {
        std::vector<Id> v;
        v.reserve(students_.size());
        for (auto const& [sid,_] : students_) v.push_back(sid);
        return v;
    }
    std::vector<Id> findStudentsByGroup(std::string_view g) const {
        std::vector<Id> res;
        for (auto const& [sid,st] : students_) {
            if (st.groupName_ == g) res.push_back(sid);
        }
        return res;
    }

    Id addTeacher(const Teacher& t) {
        TeacherService::validate(t);
        Id id = nextT_;
        ++nextT_;
        teachers_.emplace(id,t);
        return id;
    }
    bool removeTeacher(Id id) { return teachers_.erase(id) > 0; }
    Teacher* getTeacherMutable(Id id) { return &teachers_.at(id); }
    const Teacher* getTeacher(Id id) const { return &teachers_.at(id); }
    std::vector<Id> allTeacherIds() const {
        std::vector<Id> v;
        v.reserve(teachers_.size());
        for (auto const& [tid,_] : teachers_) v.push_back(tid);
        return v;
    }

    Id addGroup(const Group& g) {
        GroupService::validate(g);
        Id id = nextG_;
        ++nextG_;
        groups_.emplace(id,g);
        return id;
    }
    bool removeGroup(Id id) { return groups_.erase(id) > 0; }
    Group* getGroupMutable(Id id) { return &groups_.at(id); }
    const Group* getGroup(Id id) const { return &groups_.at(id); }
    std::vector<Id> allGroupIds() const {
        std::vector<Id> v;
        v.reserve(groups_.size());
        for (auto const& [gid,_] : groups_) v.push_back(gid);
        return v;
    }
    Group* findGroup(std::string_view name) {
        for (auto& [_,g] : groups_) {
            if (g.name_ == name) return &g;
        }
        return nullptr;
    }

    void syncGroupFromSpecialty(const Group& g) {
        if (g.specialtyName_.empty()) return;
        const Specialty* sp = findSpecialty(g.specialtyName_);
        if (!sp) return;
        for (auto& [_,st] : students_) {
            if (st.groupName_ == g.name_) {
                StudentService::ensureSubjectsFromSpecialty(st,*sp);
            }
        }
    }
    Registry(): Registry("students.txt","teachers.txt","groups.txt","specialties.txt") {}
    void save() const;
    void load();
};

}
