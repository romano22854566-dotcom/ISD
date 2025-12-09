#pragma once
#include "student.hpp"

namespace isd{

class SubjectRecordService {
public:
    static Grade<int>& gradesAt(SubjectRecord& rec,ClassType ct) {
        return rec.grades[ct];
    }
    static const Grade<int>& gradesAt(const SubjectRecord& rec,ClassType ct) {
        auto it = rec.grades.find(ct);
        if (it == rec.grades.end()) {
            static Grade<int> empty;
            return empty;
        }
        return it->second;
    }
    static int absenceAt(const SubjectRecord& rec,ClassType ct) {
        auto it = rec.absences.find(ct);
        return (it == rec.absences.end()) ? 0 : it->second;
    }
};

}
