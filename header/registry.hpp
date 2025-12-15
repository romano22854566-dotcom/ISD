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
    std::map<Id,Student> students;
    std::map<Id,Teacher> teachers;
    std::map<Id,Group> groups;
    std::map<Id,Specialty> specialties;

    Id nextS{1};
    Id nextT{1};
    Id nextG{1};
    Id nextSpec{1};

    std::string sf;
    std::string tf;
    std::string gf;
    std::string specf;

public:
    Registry(std::string s,std::string t,std::string g,std::string spec = "specialties.txt")
        : sf(std::move(s)),
        tf(std::move(t)),
        gf(std::move(g)),
        specf(std::move(spec)) {
    }

    Id addSpecialty(const Specialty& sp) {
        SpecialtyService::validate(sp);
        Id id = nextSpec;
        ++nextSpec;
        specialties.try_emplace(id,sp);
        return id;
    }
    bool removeSpecialty(Id id) { return specialties.erase(id) > 0; }
    Specialty* getSpecialtyMutable(Id id) { return &specialties.at(id); }
    const Specialty* getSpecialty(Id id) const { return &specialties.at(id); }
    std::vector<Id> allSpecialtyIds() const {
        std::vector<Id> v;
        v.reserve(specialties.size());
        for (auto const& [sid,_] : specialties) v.push_back(sid);
        return v;
    }
    const Specialty* findSpecialty(std::string_view name) const {
        for (auto const& [_,sp] : specialties) {
            if (sp.name == name) return &sp;
        }
        return nullptr;
    }

    Id addStudent(const Student& s) {
        StudentService::validate(s);
        Id id = nextS;
        ++nextS;
        students.try_emplace(id,s);
        return id;
    }
    bool removeStudent(Id id) { return students.erase(id) > 0; }
    Student* getStudentMutable(Id id) { return &students.at(id); }
    const Student* getStudent(Id id) const { return &students.at(id); }
    std::vector<Id> allStudentIds() const {
        std::vector<Id> v;
        v.reserve(students.size());
        for (auto const& [sid,_] : students) v.push_back(sid);
        return v;
    }
    std::vector<Id> findStudentsByGroup(std::string_view g) const {
        std::vector<Id> res;
        for (auto const& [sid,st] : students) {
            if (st.groupName == g) res.push_back(sid);
        }
        return res;
    }

    Id addTeacher(const Teacher& t) {
        TeacherService::validate(t);
        Id id = nextT;
        ++nextT;
        teachers.try_emplace(id,t);
        return id;
    }
    bool removeTeacher(Id id) { return teachers.erase(id) > 0; }
    Teacher* getTeacherMutable(Id id) { return &teachers.at(id); }
    const Teacher* getTeacher(Id id) const { return &teachers.at(id); }
    std::vector<Id> allTeacherIds() const {
        std::vector<Id> v;
        v.reserve(teachers.size());
        for (auto const& [tid,_] : teachers) v.push_back(tid);
        return v;
    }

    Id addGroup(const Group& g) {
        GroupService::validate(g);
        Id id = nextG;
        ++nextG;
        groups.try_emplace(id,g);
        return id;
    }
    bool removeGroup(Id id) { return groups.erase(id) > 0; }
    Group* getGroupMutable(Id id) { return &groups.at(id); }
    const Group* getGroup(Id id) const { return &groups.at(id); }
    std::vector<Id> allGroupIds() const {
        std::vector<Id> v;
        v.reserve(groups.size());
        for (auto const& [gid,_] : groups) v.push_back(gid);
        return v;
    }
    Group* findGroup(std::string_view name) {
        for (auto& [_,g] : groups) {
            if (g.name == name) return &g;
        }
        return nullptr;
    }

    void syncGroupFromSpecialty(const Group& g) {
        if (g.specialtyName.empty()) return;
        const Specialty* sp = findSpecialty(g.specialtyName);
        if (!sp) return;
        for (auto& [_,st] : students) {
            if (st.groupName == g.name) {
                StudentService::ensureSubjectsFromSpecialty(st,*sp);
            }
        }
    }
    Registry(): Registry("students.txt","teachers.txt","groups.txt","specialties.txt") {}
    void save() const;
    void load();
private:
    void loadSpecialties();
    void loadGroups();
    void loadStudents();
    void loadTeachers();
};

}

