#pragma once
#include "student.hpp"
#include "exceptions.hpp"
#include "grade.hpp"
#include "GradeService.hpp"

namespace isd{

class StudentService {
public:
    static const std::string& fullName(const Student& s) { return s.name; }
    static int age(const Student& s) { return s.age; }
    static const std::string& group(const Student& s) { return s.groupName; }

    static const std::map<std::string,SubjectRecord,std::less<>>& records(const Student& s) { return s.records; }
    static std::map<std::string,SubjectRecord,std::less<>>& records(Student& s) { return s.records; }

    static void validate(const Student& s) {
        if (s.name.empty()) throw ISDException("Пустое имя студента");
        if (s.age < 14 || s.age > 120) throw ISDException("Возраст студента некорректен");
        if (s.groupName.empty()) throw ISDException("Пустое имя группы");
    }

    static void ensureSubjectsFromSpecialty(Student& st,const Specialty& sp) {
        for (auto& s : sp.subjects) {
            auto& rec = st.records[s.name];
            for (auto ct : s.classTypes){
                (void)rec.grades[ct];
                if (!rec.absences.contains(ct))
                    rec.absences[ct] = 0;
            }
        }
    }

    static void addGrade(Student& st,const std::string& subj,ClassType ct,int v) {
        GradeService::add(st.records[subj].grades[ct],v);
    }
    static void clearGrades(Student& st,const std::string& subj,ClassType ct){
        GradeService::clear(st.records[subj].grades[ct]);
    }

    static void setAbsence(Student& st,const std::string& subj,ClassType ct,int hours) {
        if (hours < 0) hours = 0;
        st.records[subj].absences[ct] = hours;
    }
};

}
