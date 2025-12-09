#pragma once
#include <map>
#include <vector>
#include <string>
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

    Id nextS_{1},nextT_{1},nextG_{1},nextSpec_{1};

    std::string sf_,tf_,gf_,specf_;

public:
    Registry(std::string s,std::string t,std::string g,std::string spec = "specialties.txt")
        : sf_(std::move(s)),tf_(std::move(t)),gf_(std::move(g)),specf_(std::move(spec)) {
    }

    Id addSpecialty(Specialty sp) {
        SpecialtyService::validate(sp);
        Id id = nextSpec_++;
        specialties_[id] = std::move(sp);
        return id;
    }
    bool removeSpecialty(Id id) { return specialties_.erase(id) > 0; }
    Specialty* getSpecialtyMutable(Id id) { return &specialties_.at(id); }
    const Specialty* getSpecialty(Id id) const { return &specialties_.at(id); }
    std::vector<Id> allSpecialtyIds() const {
        std::vector<Id> v;
        for (auto& kv : specialties_) v.push_back(kv.first);
        return v;
    }
    const Specialty* findSpecialty(const std::string& name) const {
        for (auto& kv : specialties_) {
            if (kv.second.name_ == name) return &kv.second;
        }
        return nullptr;
    }

    Id addStudent(Student s) {
        StudentService::validate(s);
        Id id = nextS_++;
        students_[id] = std::move(s);
        return id;
    }
    bool removeStudent(Id id) { return students_.erase(id) > 0; }
    Student* getStudentMutable(Id id) { return &students_.at(id); }
    const Student* getStudent(Id id) const { return &students_.at(id); }
    std::vector<Id> allStudentIds() const {
        std::vector<Id> v;
        for (auto& kv : students_) v.push_back(kv.first);
        return v;
    }
    std::vector<Id> findStudentsByGroup(const std::string& g) const {
        std::vector<Id> res;
        for (auto& kv : students_) {
            if (kv.second.groupName_ == g) res.push_back(kv.first);
        }
        return res;
    }

    Id addTeacher(Teacher t) {
        TeacherService::validate(t);
        Id id = nextT_++;
        teachers_[id] = std::move(t);
        return id;
    }
    bool removeTeacher(Id id) { return teachers_.erase(id) > 0; }
    Teacher* getTeacherMutable(Id id) { return &teachers_.at(id); }
    const Teacher* getTeacher(Id id) const { return &teachers_.at(id); }
    std::vector<Id> allTeacherIds() const {
        std::vector<Id> v;
        for (auto& kv : teachers_) v.push_back(kv.first);
        return v;
    }

    Id addGroup(Group g) {
        GroupService::validate(g);
        Id id = nextG_++;
        groups_[id] = std::move(g);
        return id;
    }
    bool removeGroup(Id id) { return groups_.erase(id) > 0; }
    Group* getGroupMutable(Id id) { return &groups_.at(id); }
    const Group* getGroup(Id id) const { return &groups_.at(id); }
    std::vector<Id> allGroupIds() const {
        std::vector<Id> v;
        for (auto& kv : groups_) v.push_back(kv.first);
        return v;
    }
    Group* findGroup(const std::string& name) {
        for (auto& kv : groups_) {
            if (kv.second.name_ == name) return &kv.second;
        }
        return nullptr;
    }

    void syncGroupFromSpecialty(const Group& g) {
        if (g.specialtyName_.empty()) return;
        const Specialty* sp = findSpecialty(g.specialtyName_);
        if (!sp) return;
        for (auto& kv : students_) {
            if (kv.second.groupName_ == g.name_) {
                StudentService::ensureSubjectsFromSpecialty(kv.second,*sp);
            }
        }
    }

    void save() const;
    void load();
};

}
