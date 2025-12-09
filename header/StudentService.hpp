#pragma once
#include "student.hpp"
#include "exceptions.hpp"
#include "grade.hpp"
#include "GradeService.hpp"

namespace isd{

class StudentService {
public:
    static const std::string& fullName(const Student& s) { return s.name_; }
    static int age(const Student& s) { return s.age_; }
    static const std::string& group(const Student& s) { return s.groupName_; }

    static const std::map<std::string,SubjectRecord>& records(const Student& s) { return s.records_; }
    static std::map<std::string,SubjectRecord>& records(Student& s) { return s.records_; }

    static void validate(const Student& s) {
        if (s.name_.empty()) throw ISDException("Пустое имя студента");
        if (s.age_ < 14 || s.age_ > 120) throw ISDException("Возраст студента некорректен");
        if (s.groupName_.empty()) throw ISDException("Пустое имя группы");
    }

    static void ensureSubjectsFromSpecialty(Student& st,const Specialty& sp) {
        for (auto& s : sp.subjects_) {
            auto& rec = st.records_[s.name];
            for (auto ct : s.classTypes){
                (void)rec.grades[ct];
                if (rec.absences.find(ct) == rec.absences.end())
                    rec.absences[ct] = 0;
            }
        }
    }

    static void addGrade(Student& st,const std::string& subj,ClassType ct,int v) {
        GradeService::add(st.records_[subj].grades[ct],v);
    }
    static void clearGrades(Student& st,const std::string& subj,ClassType ct){
        GradeService::clear(st.records_[subj].grades[ct]);
    }

    static void setAbsence(Student& st,const std::string& subj,ClassType ct,int hours) {
        if (hours < 0) hours = 0;
        st.records_[subj].absences[ct] = hours;
    }
};

}
