#include "StudentDialog.h"
#include "ui_StudentDialog.h"
#include <QMessageBox>
#include "StudentService.hpp"
#include "GradeService.hpp"
#include "SubjectRecordService.hpp"
#include "GroupService.hpp"
#include "SpecialtyService.hpp"

using namespace isd;

StudentDialog::StudentDialog(Registry& r,Id id,QWidget* parent)
    : QDialog(parent),ui(new Ui::StudentDialog),reg(r),studentId(id)
{
    ui->setupUi(this);
    loadData();
    connect(ui->btnSave,&QPushButton::clicked,this,&StudentDialog::onSave);
}

StudentDialog::~StudentDialog(){ delete ui; }

QString StudentDialog::joinGrades(const std::vector<int>& v){
    QString out;
    for (int x : v){ out += QString::number(x) + " "; }
    return out;
}

void StudentDialog::loadData() {
    const Student* s = reg.getStudent(studentId);
    if (!s) return;

    const Group* g = reg.findGroup(StudentService::group(*s));
    if (!g) return;

    const Specialty* sp = reg.findSpecialty(GroupService::specialty(*g));
    if (!sp) return;

    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(9);
    ui->tblSubjects->setHorizontalHeaderLabels({
        "Предмет","Оценки (ЛК)","Оценки (ПЗ)","Оценки (ЛР)",
        "Пропуски (ЛК)","Пропуски (ПЗ)","Пропуски (ЛР)",
        "Средний балл","Всего пропусков"
    });
    const auto& recs = StudentService::records(*s);
    int row = 0;

    auto fillRow = [&](int r,const SubjectRecord& rec) {
        ui->tblSubjects->setItem(r,1,new QTableWidgetItem(joinGrades(SubjectRecordService::gradesAt(rec,ClassType::LK).vals)));
        ui->tblSubjects->setItem(r,2,new QTableWidgetItem(joinGrades(SubjectRecordService::gradesAt(rec,ClassType::PZ).vals)));
        ui->tblSubjects->setItem(r,3,new QTableWidgetItem(joinGrades(SubjectRecordService::gradesAt(rec,ClassType::LR).vals)));
        ui->tblSubjects->setItem(r,4,new QTableWidgetItem(QString::number(SubjectRecordService::absenceAt(rec,ClassType::LK))));
        ui->tblSubjects->setItem(r,5,new QTableWidgetItem(QString::number(SubjectRecordService::absenceAt(rec,ClassType::PZ))));
        ui->tblSubjects->setItem(r,6,new QTableWidgetItem(QString::number(SubjectRecordService::absenceAt(rec,ClassType::LR))));

        double sum = 0.0; int cnt = 0; int abs = 0;

        for (const auto& [_,gr] : rec.grades) {
            for (int v : gr.vals) { sum += v; ++cnt; }
        }
        for (const auto& [_,a] : rec.absences) { abs += a; }

        const double finalAvg = (cnt > 0) ? (sum / cnt) : 0.0;
        ui->tblSubjects->setItem(r,7,new QTableWidgetItem(QString::number(finalAvg,'f',2)));
        ui->tblSubjects->setItem(r,8,new QTableWidgetItem(QString::number(abs)));
    };

    for (const auto& specSubj : sp->subjects) {
        ui->tblSubjects->insertRow(row);
        ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(specSubj.name)));

        if (auto it = recs.find(specSubj.name); it != recs.end()) {
            const SubjectRecord& rec = it->second;
            fillRow(row,rec);
        }
        else {
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(""));
            ui->tblSubjects->setItem(row,2,new QTableWidgetItem(""));
            ui->tblSubjects->setItem(row,3,new QTableWidgetItem(""));
            ui->tblSubjects->setItem(row,4,new QTableWidgetItem("0"));
            ui->tblSubjects->setItem(row,5,new QTableWidgetItem("0"));
            ui->tblSubjects->setItem(row,6,new QTableWidgetItem("0"));
            ui->tblSubjects->setItem(row,7,new QTableWidgetItem(QString::number(0.0,'f',2)));
            ui->tblSubjects->setItem(row,8,new QTableWidgetItem("0"));
        }
        ++row;
        for (int row = 0; row < ui->tblSubjects->rowCount(); ++row) {
            QTableWidgetItem* item = ui->tblSubjects->item(row,0);
            if (item) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); 
            }
        }
    }
}
void StudentDialog::onSave() {
    Student* s = reg.getStudentMutable(studentId);
    if (!s) { accept(); return; }

    for (int row = 0; row < ui->tblSubjects->rowCount(); ++row) {
        const QString subj = ui->tblSubjects->item(row,0)->text();

        auto parseGrades = [&](int col,ClassType ct){
            auto* item = ui->tblSubjects->item(row,col);
            const QStringList parts = item ? item->text().split(" ",Qt::SkipEmptyParts) : QStringList{};
            StudentService::clearGrades(*s,subj.toStdString(),ct);
            for (const auto& p : parts) {
                bool ok = false; const int val = p.toInt(&ok);
                if (ok) StudentService::addGrade(*s,subj.toStdString(),ct,val);
            }
        };
        parseGrades(1,ClassType::LK);
        parseGrades(2,ClassType::PZ);
        parseGrades(3,ClassType::LR);

        auto parseAbs = [&](int col,ClassType ct){
            auto* item = ui->tblSubjects->item(row,col);
            const int val = item ? item->text().toInt() : 0;
            StudentService::setAbsence(*s,subj.toStdString(),ct,val);
        };
        parseAbs(4,ClassType::LK);
        parseAbs(5,ClassType::PZ);
        parseAbs(6,ClassType::LR);
    }
    accept();
}
