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

void TeacherDialog::loadData() {
    const Teacher* t = reg.getTeacher(teacherId);
    if (!t) return;
    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(2);
    ui->tblSubjects->setHorizontalHeaderLabels({"Группа","Предмет"});

    int row = 0;
    for (const auto& [group,subjects] : t->groupSubjects_) {
        for (const auto& subj : subjects) {
            ui->tblSubjects->insertRow(row);
            ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(group)));
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(QString::fromStdString(subj.name)));
            ++row;
        }
    }
}

void TeacherDialog::onAddSubject() {
    Teacher* t = reg.getTeacherMutable(teacherId);
    if (!t) return;
    bool ok;
    const QString group = QInputDialog::getText(this,"Группа","Введите название группы:",QLineEdit::Normal,"",&ok);
    if (!ok || group.isEmpty()) return;
    const QString subj = QInputDialog::getText(this,"Предмет","Введите название предмета:",QLineEdit::Normal,"",&ok);
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
