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
    auto t = reg.getTeacher(teacherId);
    if (!t) return;
    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(2);
    ui->tblSubjects->setHorizontalHeaderLabels({"Группа","Предмет"});
    int row = 0;
    for (auto& kv : t->groupSubjects_) {
        for (auto& subj : kv.second) {
            ui->tblSubjects->insertRow(row);
            ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(kv.first)));
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(QString::fromStdString(subj.name)));
            row++;
        }
    }
}

void TeacherDialog::onAddSubject() {
    auto t = reg.getTeacherMutable(teacherId);
    if (!t) return;
    bool ok;
    QString group = QInputDialog::getText(this,"Группа","Введите название группы:",QLineEdit::Normal,"",&ok);
    if (!ok || group.isEmpty()) return;
    QString subj = QInputDialog::getText(this,"Предмет","Введите название предмета:",QLineEdit::Normal,"",&ok);
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
    auto t = reg.getTeacherMutable(teacherId);
    if (!t) return;
    int row = ui->tblSubjects->currentRow();
    if (row < 0) return;
    QString group = ui->tblSubjects->item(row,0)->text();
    QString subj = ui->tblSubjects->item(row,1)->text();
    TeacherService::removeSubject(*t,group.toStdString(),subj.toStdString());
    loadData();
}
