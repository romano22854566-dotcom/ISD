#include "TeacherDialog.h"
#include "ui_TeacherDialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include "TeacherService.hpp"
#include "GroupService.hpp"

using namespace isd;

TeacherDialog::TeacherDialog(isd::Registry& r,int id,QWidget* parent)
    : QDialog(parent),
    ui(new Ui::TeacherDialog),
    reg(r),
    teacherId(id)
{
    ui->setupUi(this);
    loadData();
    connect(ui->btnAdd,&QPushButton::clicked,this,&TeacherDialog::onAddSubject);
    connect(ui->btnRemove,&QPushButton::clicked,this,&TeacherDialog::onRemoveSubject);
}

TeacherDialog::~TeacherDialog() {
    delete ui;
}

void TeacherDialog::insertSubjectRow(int row,const std::string& group,const std::string& subjName)
{
    ui->tblSubjects->insertRow(row);
    ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(group)));
    ui->tblSubjects->setItem(row,1,new QTableWidgetItem(QString::fromStdString(subjName)));
}

void TeacherDialog::disableSubjectTableEditing()
{
    for (int row = 0; row < ui->tblSubjects->rowCount(); ++row) {
        for (int col = 0; col < 2; ++col) {
            QTableWidgetItem* item = ui->tblSubjects->item(row,col);
            if (item) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
}

void TeacherDialog::loadData() {
    const Teacher* t = reg.getTeacher(teacherId);
    if (!t) return;
    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(2);
    ui->tblSubjects->setHorizontalHeaderLabels({"Группа","Предмет"});

    int row = 0;
    for (const auto& [group,subjects] : t->groupSubjects) {
        for (const auto& subj : subjects) {
            insertSubjectRow(row,group,subj.name); 
            ++row;
        }
    }
    disableSubjectTableEditing();
}

void TeacherDialog::onAddSubject() {
    Teacher* t = reg.getTeacherMutable(teacherId);
    if (!t) return;

    bool ok;
    QStringList existingGroups;
    auto groupIds = reg.allGroupIds();
    for (auto gid : groupIds) {
        const Group* g = reg.getGroup(gid);
        existingGroups << QString::fromStdString(g->name);
    }

    if (existingGroups.isEmpty()) {
        QMessageBox::warning(this,"Ошибка","Нет доступных групп");
        return;
    }

    QString group = QInputDialog::getItem(this,"Группа",
                                         "Выберите группу:",
                                         existingGroups,0,false,&ok);
    if (!ok || group.isEmpty()) return;

    const Group* selectedGroup = reg.findGroup(group.toStdString());
    if (!selectedGroup || selectedGroup->specialtyName.empty()) {
        QMessageBox::warning(this,"Ошибка",
                           "У выбранной группы не указана специальность");
        return;
    }

    const Specialty* sp = reg.findSpecialty(selectedGroup->specialtyName);
    if (!sp || sp->subjects.empty()) {
        QMessageBox::warning(this,"Ошибка",
                           "У специальности нет предметов");
        return;
    }

    QStringList availableSubjects;
    for (const auto& subject : sp->subjects) {
        availableSubjects << QString::fromStdString(subject.name);
    }

    QString subj = QInputDialog::getItem(this,"Предмет",
                                        "Выберите предмет:",
                                        availableSubjects,0,false,&ok);
    if (!ok || subj.isEmpty()) return;
   
    try {
        TeacherService::addSubject(*t,group.toStdString(),Subject{subj.toStdString()});
        loadData();
    }
    catch (const ISDException& e) {
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void TeacherDialog::onRemoveSubject() {
    Teacher* t = reg.getTeacherMutable(teacherId);
    if (!t) return;
    const int row = ui->tblSubjects->currentRow();
    if (row < 0) return;
    const QString group = ui->tblSubjects->item(row,0)->text();
    const QString subj = ui->tblSubjects->item(row,1)->text();
    TeacherService::removeSubject(*t,group.toStdString(),subj.toStdString());
    loadData();
}
