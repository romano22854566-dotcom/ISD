#include "registry.hpp"
#include "StudentService.hpp"
#include "TeacherService.hpp"
#include "GroupService.hpp"
#include "SpecialtyService.hpp"
#include "GradeService.hpp"
#include "SubjectRecordService.hpp"

#include <fstream>
#include <sstream>
#include <string>


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
    std::vector<std::string> v; std::string cur; bool e = false;
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
    using enum ClassType;
    switch (ct){
    case LK: return "LK";
    case PZ: return "PZ";
    case LR: return "LR";
    }
    return "LK";
}

static ClassType tagToClassType(std::string_view t){
    using enum ClassType;
    if (t == "LK") return LK;
    if (t == "PZ") return PZ;
    return LR;
}


static void writeGrades(std::ostringstream& oss,const SubjectRecord& rec) {
    using enum ClassType;
    for (ClassType ct : {LK,PZ,LR}) {
        oss << "|" << classTypeToTag(ct) << ":";
        const auto& vals = SubjectRecordService::gradesAt(rec,ct).vals_;
        bool first = true;
        for (int v : vals) {
            if (!first) oss << ",";
            oss << v;
            first = false;
        }
    }
}

static void writeStudent(std::ostream& f,const Student& s) {
    std::ostringstream oss;
    oss << esc(StudentService::fullName(s)) << ";"
        << StudentService::age(s) << ";"
        << esc(StudentService::group(s));

    for (const auto& [subj,rec] : StudentService::records(s)) {
        oss << ";" << esc(subj);
        writeGrades(oss,rec);   

        oss << ";!" << esc(subj);
        using enum ClassType;
        for (ClassType ct : {LK,PZ,LR}) {
            oss << "|" << classTypeToTag(ct) << ":"
                << SubjectRecordService::absenceAt(rec,ct);
        }
    }
    f << oss.str() << "\n";
}

static void writeTeacher(std::ostream& f,const Teacher& t) {
    std::ostringstream oss;
    oss << esc(TeacherService::fullName(t)) << ";"
        << TeacherService::age(t);
    for (const auto& [group,subjects] : t.groupSubjects_) {
        oss << ";" << esc(group) << "|";
        for (size_t i = 0; i < subjects.size(); ++i) {
            oss << esc(subjects[i].name);
            if (i + 1 < subjects.size()) oss << ",";
        }
    }
    f << oss.str() << "\n";
}

static void writeGroup(std::ostream& f,const Group& g) {
    std::ostringstream oss;
    oss << std::string(GroupService::name(g)) << ";"
        << std::string(GroupService::specialty(g));
    f << oss.str() << "\n";
}

static void writeSpecialty(std::ostream& f,const Specialty& sp) {
    std::ostringstream oss;
    oss << esc(SpecialtyService::name(sp));
    for (const auto& s : sp.subjects_) {
        const char ctl = (s.control == ControlType::Zachet) ? 'Z' : 'E';
        oss << ";" << esc(s.name) << "|" << ctl << "|";
        for (size_t i = 0; i < s.classTypes.size(); ++i) {
            oss << classTypeToTag(s.classTypes[i]);
            if (i + 1 < s.classTypes.size()) oss << ",";
        }
    }
    f << oss.str() << "\n";
}

void Registry::save() const {
    {
        std::ofstream f(sf_);
        if (!f) throw ISDException("Не удалось открыть файл студентов для записи");
        for (const auto& [_,s] : students_) {
            writeStudent(f,s);
        }
    }

    {
        std::ofstream f(tf_);
        if (!f) throw ISDException("Не удалось открыть файл преподавателей для записи");
        for (const auto& [_,t] : teachers_) {
            writeTeacher(f,t);
        }
    }

    {
        std::ofstream f(gf_);
        if (!f) throw ISDException("Не удалось открыть файл групп для записи");
        for (const auto& [_,g] : groups_) {
            writeGroup(f,g);
        }
    }

    {
        std::ofstream f(specf_);
        if (!f) throw ISDException("Не удалось открыть файл специальностей для записи");
        for (const auto& [_,sp] : specialties_) {
            writeSpecialty(f,sp);
        }
    }
}

void Registry::load() {
    students_.clear(); teachers_.clear(); groups_.clear(); specialties_.clear();
    nextS_ = nextT_ = nextG_ = nextSpec_ = 1;

    loadSpecialties();
    loadGroups();
    loadStudents();
    loadTeachers();
}


static void applyAbsBlock(Student& s,const std::string& blk) {
    const auto kv = split(blk,'|');
    if (kv.empty()) return;
    const std::string subj = unesc(kv[0]);
    for (size_t k = 1; k < kv.size(); ++k) {
        const auto ct_pair = split(kv[k],':');
        if (ct_pair.size() != 2) continue;
        const ClassType ct = tagToClassType(ct_pair[0]);
        const int hrs = ct_pair[1].empty() ? 0 : std::stoi(ct_pair[1]);
        StudentService::setAbsence(s,subj,ct,hrs);
    }
}

static void applyGradeBlock(Student& s,const std::string& part) {
    const auto kv = split(part,'|');
    if (kv.empty()) return;
    const std::string subj = unesc(kv[0]);
    for (size_t k = 1; k < kv.size(); ++k) {
        const auto ct_pair = split(kv[k],':');
        if (ct_pair.size() != 2) continue;
        const ClassType ct = tagToClassType(ct_pair[0]);
        const auto nums = split(ct_pair[1],',');
        StudentService::clearGrades(s,subj,ct);
        for (const auto& n : nums) {
            if (!n.empty()) StudentService::addGrade(s,subj,ct,std::stoi(n));
        }
    }
}

static void addSubjectsToTeacher(Teacher& t,const std::string& gname,const std::string& csv) {
    const auto subs = split(csv,',');
    for (const auto& one : subs) {
        if (one.empty()) continue;
        TeacherService::addSubject(t,gname,Subject{unesc(one)});
    }
}

void Registry::loadSpecialties() {
    std::ifstream f(specf_);
    std::string line;
    while (std::getline(f,line)) {
        if (line.empty()) continue;
        const auto parts = split(line,';');
        Specialty sp(unesc(parts[0]));
        for (size_t i = 1; i < parts.size(); ++i) {
            const auto kv = split(parts[i],'|');
            if (kv.size() != 3) continue;
            const std::string subj = unesc(kv[0]);
            const ControlType ct = (kv[1] == "E") ? ControlType::Exam : ControlType::Zachet;
            std::vector<ClassType> types;
            const auto tparts = split(kv[2],',');
            for (const auto& tp : tparts) types.push_back(tagToClassType(tp));
            if (!types.empty()) SpecialtyService::addSubject(sp,SpecSubject{subj, ct, types});
        }
        addSpecialty(sp);
    }
}
void Registry::loadGroups() {
    std::ifstream f(gf_);
    std::string line;
    while (std::getline(f,line)) {
        if (line.empty()) continue;
        const auto parts = split(line,';');
        Group g(unesc(parts[0]));
        if (parts.size() >= 2) GroupService::setSpecialty(g,unesc(parts[1]));
        addGroup(g);
    }
}




void Registry::loadStudents() {
    std::ifstream f(sf_);
    std::string line;
    while (std::getline(f,line)) {
        if (line.empty()) continue;
        const auto parts = split(line,';');
        if (parts.size() < 3) continue;

        Student s(unesc(parts[0]),std::stoi(parts[1]),unesc(parts[2]));

        if (const Group* g = findGroup(StudentService::group(s));
            g && !GroupService::specialty(*g).empty()) {
            if (const Specialty* sp = findSpecialty(GroupService::specialty(*g))) {
                StudentService::ensureSubjectsFromSpecialty(s,*sp);
            }
        }

        for (size_t i = 3; i < parts.size(); ++i) {
            if (parts[i].empty()) continue;
            if (parts[i][0] == '!') {
                applyAbsBlock(s,parts[i].substr(1));
            }
            else {
                applyGradeBlock(s,parts[i]);
            }
        }
        addStudent(s);
    }
}

void Registry::loadTeachers() {
    std::ifstream f(tf_);
    std::string line;
    while (std::getline(f,line)) {
        if (line.empty()) continue;
        const auto parts = split(line,';');
        if (parts.size() < 2) continue;

        Teacher t(unesc(parts[0]),std::stoi(parts[1]));
        TeacherService::validate(t);

        for (size_t i = 2; i < parts.size(); ++i) {
            const auto kv = split(parts[i],'|');
            if (kv.size() != 2) continue;
            addSubjectsToTeacher(t,unesc(kv[0]),kv[1]);
        }
        addTeacher(t);
    }
}
}

