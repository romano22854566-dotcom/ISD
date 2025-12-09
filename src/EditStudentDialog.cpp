#include "EditStudentDialog.h"
#include "ui_EditStudentDialog.h"
#include <QMessageBox>
#include "StudentService.hpp"

using namespace isd;

EditStudentDialog::EditStudentDialog(isd::Registry& r,isd::Id id,QWidget* parent)
    : QDialog(parent),
    ui(new Ui::EditStudentDialog),
    reg(r),
    studentId(id)
{
    ui->setupUi(this);
    loadData();
    connect(ui->btnSave,&QPushButton::clicked,this,&EditStudentDialog::onSave);
}

EditStudentDialog::~EditStudentDialog() {
    delete ui;
}

void EditStudentDialog::loadData() {
    auto s = reg.getStudent(studentId);
    if (!s) return;
    ui->editName->setText(QString::fromStdString(StudentService::fullName(*s)));
    ui->spinAge->setValue(StudentService::age(*s));
    ui->editGroup->setText(QString::fromStdString(StudentService::group(*s)));
}

void EditStudentDialog::onSave() {
    auto s = reg.getStudentMutable(studentId);
    if (!s) return;
    try {
        std::string newName = ui->editName->text().toStdString();
        int newAge = ui->spinAge->value();
        std::string newGroup = ui->editGroup->text().toStdString();

        Student edited(newName,newAge,newGroup);
        StudentService::validate(edited);
        edited.records_ = s->records_; 
        *s = edited;
        accept();
    }
    catch (const ISDException& e) {
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}
