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
    auto s = reg.getStudent(studentId);
    auto g = reg.findGroup(StudentService::group(*s));
    if (!g) return;
    const auto* sp = reg.findSpecialty(GroupService::specialty(*g));
    if (!sp) return;

    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(9);
    ui->tblSubjects->setHorizontalHeaderLabels({
        "Предмет","Оценки (ЛК)","Оценки (ПЗ)","Оценки (ЛР)",
        "Пропуски (ЛК)","Пропуски (ПЗ)","Пропуски (ЛР)",
        "Средний балл","Всего пропусков"
    });

    int row = 0;
    for (auto& specSubj : sp->subjects_) {
        ui->tblSubjects->insertRow(row);
        ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(specSubj.name)));

        const auto& recs = StudentService::records(*s);
        auto it = recs.find(specSubj.name);
        double avg = 0.0; int count = 0; int totalAbs = 0;

        if (it != recs.end()){
            const auto& rec = it->second;
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(joinGrades(SubjectRecordService::gradesAt(rec,ClassType::LK).vals_)));
            ui->tblSubjects->setItem(row,2,new QTableWidgetItem(joinGrades(SubjectRecordService::gradesAt(rec,ClassType::PZ).vals_)));
            ui->tblSubjects->setItem(row,3,new QTableWidgetItem(joinGrades(SubjectRecordService::gradesAt(rec,ClassType::LR).vals_)));
            ui->tblSubjects->setItem(row,4,new QTableWidgetItem(QString::number(SubjectRecordService::absenceAt(rec,ClassType::LK))));
            ui->tblSubjects->setItem(row,5,new QTableWidgetItem(QString::number(SubjectRecordService::absenceAt(rec,ClassType::PZ))));
            ui->tblSubjects->setItem(row,6,new QTableWidgetItem(QString::number(SubjectRecordService::absenceAt(rec,ClassType::LR))));

            for (auto& gkv : rec.grades){
                for (int v : gkv.second.vals_){ avg += v; ++count; }
            }
            for (auto& akv : rec.absences){ totalAbs += akv.second; }
        }
        else {
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(""));
            ui->tblSubjects->setItem(row,2,new QTableWidgetItem(""));
            ui->tblSubjects->setItem(row,3,new QTableWidgetItem(""));
            ui->tblSubjects->setItem(row,4,new QTableWidgetItem("0"));
            ui->tblSubjects->setItem(row,5,new QTableWidgetItem("0"));
            ui->tblSubjects->setItem(row,6,new QTableWidgetItem("0"));
        }
        double finalAvg = (count > 0) ? (avg / count) : 0.0;
        ui->tblSubjects->setItem(row,7,new QTableWidgetItem(QString::number(finalAvg,'f',2)));
        ui->tblSubjects->setItem(row,8,new QTableWidgetItem(QString::number(totalAbs)));
        ++row;
    }
}

void StudentDialog::onSave() {
    auto s = reg.getStudentMutable(studentId);
    for (int row = 0; row < ui->tblSubjects->rowCount(); ++row) {
        QString subj = ui->tblSubjects->item(row,0)->text();

        auto parseGrades = [&](int col,ClassType ct){
            auto* item = ui->tblSubjects->item(row,col);
            QStringList parts = item ? item->text().split(" ",Qt::SkipEmptyParts) : QStringList{};
            StudentService::clearGrades(*s,subj.toStdString(),ct);
            for (auto& p : parts) {
                bool ok = false; int val = p.toInt(&ok);
                if (ok) StudentService::addGrade(*s,subj.toStdString(),ct,val);
            }
        };
        parseGrades(1,ClassType::LK);
        parseGrades(2,ClassType::PZ);
        parseGrades(3,ClassType::LR);

        auto parseAbs = [&](int col,ClassType ct){
            auto* item = ui->tblSubjects->item(row,col);
            int val = item ? item->text().toInt() : 0;
            StudentService::setAbsence(*s,subj.toStdString(),ct,val);
        };
        parseAbs(4,ClassType::LK);
        parseAbs(5,ClassType::PZ);
        parseAbs(6,ClassType::LR);
    }
    accept();
}
