#include "registry.hpp"
#include "StudentService.hpp"
#include "TeacherService.hpp"
#include "GroupService.hpp"
#include "SpecialtyService.hpp"
#include "GradeService.hpp"
#include "SubjectRecordService.hpp"

#include <fstream>
#include <sstream>

namespace isd{

static std::string esc(const std::string& s){
    std::string o;
    for (char c : s){
        if (c == '|' || c == ';' || c == ',' || c == ':' || c == '\\' || c == '!') o.push_back('\\');
        o.push_back(c);
    }
    return o;
}
static std::string unesc(const std::string& s){
    std::string o; bool e = false;
    for (char c : s){
        if (e){ o.push_back(c); e = false; }
        else if (c == '\\') e = true;
        else o.push_back(c);
    }
    return o;
}
static std::vector<std::string> split(const std::string& l,char sep){
    std::vector<std::string>v; std::string cur; bool e = false;
    for (char c : l){
        if (e){ cur.push_back(c); e = false; }
        else if (c == '\\') e = true;
        else if (c == sep){ v.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    v.push_back(cur);
    return v;
}

static std::string classTypeToTag(ClassType ct){
    switch (ct){
    case ClassType::LK: return "LK";
    case ClassType::PZ: return "PZ";
    case ClassType::LR: return "LR";
    }
    return "LK";
}
static ClassType tagToClassType(const std::string& t){
    if (t == "LK") return ClassType::LK;
    if (t == "PZ") return ClassType::PZ;
    return ClassType::LR;
}

void Registry::save() const {
    {
        std::ofstream f(sf_);
        if (!f) throw ISDException("Не удалось открыть файл студентов для записи");
        for (auto& kv : students_) {
            const auto& s = kv.second;
            std::ostringstream oss;
            oss << esc(StudentService::fullName(s)) << ";"
                << StudentService::age(s) << ";"
                << esc(StudentService::group(s));

            for (auto& recKv : StudentService::records(s)) {
                const auto& subj = recKv.first;
                const auto& rec = recKv.second;

               
                oss << ";" << esc(subj);
                for (ClassType ct : {ClassType::LK,ClassType::PZ,ClassType::LR}) {
                    oss << "|" << classTypeToTag(ct) << ":";
                    const auto& vals = SubjectRecordService::gradesAt(rec,ct).vals_;
                    for (size_t i = 0; i < vals.size(); ++i) {
                        oss << vals[i];
                        if (i + 1 < vals.size()) oss << ",";
                    }
                }
              
                oss << ";!" << esc(subj);
                for (ClassType ct : {ClassType::LK,ClassType::PZ,ClassType::LR}) {
                    oss << "|" << classTypeToTag(ct) << ":"
                        << SubjectRecordService::absenceAt(rec,ct);
                }
            }
            f << oss.str() << "\n";
        }
    }

    {
        std::ofstream f(tf_);
        if (!f) throw ISDException("Не удалось открыть файл преподавателей для записи");
        for (auto& kv : teachers_) {
            const auto& t = kv.second;
            std::ostringstream oss;
            oss << esc(TeacherService::fullName(t)) << ";"
                << TeacherService::age(t);
            for (auto& gr : t.groupSubjects_) {
                oss << ";" << esc(gr.first) << "|";
                for (size_t i = 0; i < gr.second.size(); ++i) {
                    oss << esc(gr.second[i].name);
                    if (i + 1 < gr.second.size()) oss << ",";
                }
            }
            f << oss.str() << "\n";
        }
    }

    {
        std::ofstream f(gf_);
        if (!f) throw ISDException("Не удалось открыть файл групп для записи");
        for (auto& kv : groups_) {
            const auto& g = kv.second;
            std::ostringstream oss;
            oss << esc(GroupService::name(g)) << ";"
                << esc(GroupService::specialty(g));
            f << oss.str() << "\n";
        }
    }

    {
        std::ofstream f(specf_);
        if (!f) throw ISDException("Не удалось открыть файл специальностей для записи");
        for (auto& kv : specialties_) {
            const auto& sp = kv.second;
            std::ostringstream oss;
            oss << esc(SpecialtyService::name(sp));
            for (auto& s : sp.subjects_) {
                char ctl = (s.control == ControlType::Zachet) ? 'Z' : 'E';
                oss << ";" << esc(s.name) << "|" << ctl << "|";
                for (size_t i = 0; i < s.classTypes.size(); ++i) {
                    oss << classTypeToTag(s.classTypes[i]);
                    if (i + 1 < s.classTypes.size()) oss << ",";
                }
            }
            f << oss.str() << "\n";
        }
    }
}
void Registry::load() {
    students_.clear(); teachers_.clear(); groups_.clear(); specialties_.clear();
    nextS_ = nextT_ = nextG_ = nextSpec_ = 1;

    {
        std::ifstream f(specf_);
        std::string line;
        while (std::getline(f,line)) {
            if (line.empty()) continue;
            auto parts = split(line,';');
            Specialty sp(unesc(parts[0]));
            for (size_t i = 1; i < parts.size(); ++i) {
                auto kv = split(parts[i],'|');
                if (kv.size() != 3) continue;
                std::string subj = unesc(kv[0]);
                ControlType ct = (kv[1] == "E") ? ControlType::Exam : ControlType::Zachet;
                std::vector<ClassType> types;
                auto tparts = split(kv[2],',');
                for (auto& tp : tparts) {
                    types.push_back(tagToClassType(tp));
                }
                if (!types.empty()) {
                    SpecialtyService::addSubject(sp,SpecSubject{subj,ct,types});
                }
            }
            addSpecialty(sp);
        }
    }

    {
        std::ifstream f(gf_);
        std::string line;
        while (std::getline(f,line)) {
            if (line.empty()) continue;
            auto parts = split(line,';');
            Group g(unesc(parts[0]));
            if (parts.size() >= 2) GroupService::setSpecialty(g,unesc(parts[1]));
            addGroup(g);
        }
    }

    {
        std::ifstream f(sf_);
        std::string line;
        while (std::getline(f,line)) {
            if (line.empty()) continue;
            auto parts = split(line,';');
            if (parts.size() < 3) continue;
            Student s(unesc(parts[0]),std::stoi(parts[1]),unesc(parts[2]));
            Group* g = findGroup(StudentService::group(s));
            if (g && !GroupService::specialty(*g).empty()) {
                const Specialty* sp = findSpecialty(GroupService::specialty(*g));
                if (sp) StudentService::ensureSubjectsFromSpecialty(s,*sp);
            }
            size_t i = 3;
            while (i < parts.size()) {
                if (parts[i].empty()){ ++i; continue; }
                if (parts[i][0] == '!') {
                    auto blk = parts[i].substr(1);
                    auto kv = split(blk,'|');
                    if (!kv.empty()) {
                        std::string subj = unesc(kv[0]);
                        for (size_t k = 1; k < kv.size(); ++k) {
                            auto ct_pair = split(kv[k],':');
                            if (ct_pair.size() != 2) continue;
                            ClassType ct = tagToClassType(ct_pair[0]);
                            int hrs = ct_pair[1].empty() ? 0 : std::stoi(ct_pair[1]);
                            StudentService::setAbsence(s,subj,ct,hrs);
                        }
                    }
                    ++i;
                }
                else {
                    auto kv = split(parts[i],'|');
                    if (!kv.empty()) {
                        std::string subj = unesc(kv[0]);
                        for (size_t k = 1; k < kv.size(); ++k) {
                            auto ct_pair = split(kv[k],':');
                            if (ct_pair.size() != 2) continue;
                            ClassType ct = tagToClassType(ct_pair[0]);
                            auto nums = split(ct_pair[1],',');
                            StudentService::clearGrades(s,subj,ct);
                            for (auto& n : nums) {
                                if (!n.empty()) StudentService::addGrade(s,subj,ct,std::stoi(n));
                            }
                        }
                    }
                    ++i;
                }
            }
            addStudent(s);
        }
    }

    {
        std::ifstream f(tf_);
        std::string line;
        while (std::getline(f,line)) {
            if (line.empty()) continue;
            auto parts = split(line,';');
            if (parts.size() < 2) continue;

            Teacher t(unesc(parts[0]),std::stoi(parts[1]));
            TeacherService::validate(t);

            for (size_t i = 2; i < parts.size(); ++i) {
                auto kv = split(parts[i],'|');
                if (kv.size() != 2) continue;
                std::string gname = unesc(kv[0]);
                auto subs = split(kv[1],',');
                for (auto& s : subs) {
                    if (!s.empty()) {
                        TeacherService::addSubject(t,gname,Subject{unesc(s)});
                    }
                }
            }
            addTeacher(t);
        }
    }
}
}