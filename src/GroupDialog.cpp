#include "GroupDialog.h"
#include "ui_GroupDialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include "SpecialtyService.hpp"

GroupDialog::GroupDialog(isd::Registry& r,int id,QWidget* parent)
    : QDialog(parent),ui(new Ui::GroupDialog),reg(r),groupId(id)
{
    ui->setupUi(this);
    loadData();

    if (groupId >= 0) {
        connect(ui->btnAddSubject,&QPushButton::clicked,this,&GroupDialog::onAddSubject);
        connect(ui->btnRemoveSubject,&QPushButton::clicked,this,&GroupDialog::onRemoveSubject);
        ui->btnAddSpecSubject->setEnabled(false);
    }
    else {
        connect(ui->btnAddSubject,&QPushButton::clicked,this,&GroupDialog::onAddSpecialty);
        connect(ui->btnRemoveSubject,&QPushButton::clicked,this,&GroupDialog::onRemoveSpecItem);
        connect(ui->btnAddSpecSubject,&QPushButton::clicked,this,&GroupDialog::onAddSpecSubject);
    }
}

GroupDialog::~GroupDialog() {
    delete ui;
}

void GroupDialog::loadData() {
    if (groupId >= 0) loadGroupView();
    else loadSpecialtyView();
}

void GroupDialog::loadGroupView() {
    auto g = reg.getGroup(groupId);
    if (!g) return;
    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(2);
    ui->tblSubjects->setHorizontalHeaderLabels({"Специальность","Предметы (просмотр)"});

    const auto* sp = reg.findSpecialty(g->specialtyName_);
    ui->tblSubjects->insertRow(0);
    ui->tblSubjects->setItem(0,0,new QTableWidgetItem(QString::fromStdString(g->specialtyName_)));
    QString subjList;
    if (sp) {
        for (size_t i = 0; i < sp->subjects_.size(); ++i){
            subjList += QString::fromStdString(sp->subjects_[i].name);
            if (i + 1 < sp->subjects_.size()) subjList += ", ";
        }
    }
    ui->tblSubjects->setItem(0,1,new QTableWidgetItem(subjList));
}

void GroupDialog::loadSpecialtyView() {
    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(3);
    ui->tblSubjects->setHorizontalHeaderLabels({"Специальность","Предмет","Контроль"});

    int row = 0;
    for (auto id : reg.allSpecialtyIds()) {
        const auto* sp = reg.getSpecialty(id);
        if (!sp) continue;

        if (sp->subjects_.empty()) {
            ui->tblSubjects->insertRow(row);
            ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(sp->name_)));
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem("-"));
            ui->tblSubjects->setItem(row,2,new QTableWidgetItem("-"));
            ++row;
        }
        else {
            for (auto& s : sp->subjects_) {
                ui->tblSubjects->insertRow(row);
                ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(sp->name_)));
                ui->tblSubjects->setItem(row,1,new QTableWidgetItem(QString::fromStdString(s.name)));
                QString ctl = (s.control == isd::ControlType::Zachet) ? "Зачёт" : "Экзамен";
                ui->tblSubjects->setItem(row,2,new QTableWidgetItem(ctl));
                ++row;
            }
        }
    }
}

void GroupDialog::onAddSubject() {
    QMessageBox::information(this,"Информация","Предметы группы задаются специальностью.");
}
void GroupDialog::onRemoveSubject() {
    QMessageBox::information(this,"Информация","Удаление предметов выполняется в режиме специальностей.");
}

void GroupDialog::onAddSpecialty() {
    QString name = QInputDialog::getText(this,"Добавить специальность","Название специальности:");
    if (name.trimmed().isEmpty()) return;
    try {
        isd::Specialty sp(name.toStdString());
        isd::SpecialtyService::validate(sp);
        reg.addSpecialty(sp);
        loadSpecialtyView();
    }
    catch (const isd::ISDException& e){
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void GroupDialog::onAddSpecSubject() {
    auto sids = reg.allSpecialtyIds();
    if (sids.empty()) {
        QMessageBox::information(this,"Специальности","Нет специальностей.");
        return;
    }

    QStringList specNames;
    for (auto id : sids) specNames << QString::fromStdString(reg.getSpecialty(id)->name_);
    bool ok = false;
    QString spname = QInputDialog::getItem(this,"Специальность","Выберите:",specNames,0,false,&ok);
    if (!ok || spname.isEmpty()) return;

    QString subj = QInputDialog::getText(this,"Добавить предмет","Название предмета:");
    if (subj.trimmed().isEmpty()) return;

    QString ctrl = QInputDialog::getItem(this,"Тип контроля","Выберите:",{"Зачёт","Экзамен"},0,false,&ok);
    if (!ok || ctrl.isEmpty()) return;
    isd::ControlType ct = (ctrl == "Экзамен") ? isd::ControlType::Exam : isd::ControlType::Zachet;

    std::vector<isd::ClassType> types = {isd::ClassType::LK, isd::ClassType::PZ, isd::ClassType::LR};

    for (auto id : sids) {
        auto* sp = reg.getSpecialtyMutable(id);
        if (sp && sp->name_ == spname.toStdString()) {
            try {
                isd::SpecialtyService::addSubject(*sp,isd::SpecSubject{subj.toStdString(), ct, types});
                loadSpecialtyView();
            }
            catch (const isd::ISDException& e) {
                QMessageBox::warning(this,"Ошибка",e.what());
            }
            break;
        }
    }
}

void GroupDialog::onRemoveSpecItem() {
    int row = ui->tblSubjects->currentRow();
    if (row < 0) return;

    QString spname = ui->tblSubjects->item(row,0)->text();
    QString subj = ui->tblSubjects->item(row,1)->text();

    for (auto id : reg.allSpecialtyIds()) {
        auto* sp = reg.getSpecialtyMutable(id);
        if (!sp || QString::fromStdString(sp->name_) != spname) continue;

        if (subj == "-" || subj.isEmpty()) {
            reg.removeSpecialty(id);
        }
        else {
            try {
                isd::SpecialtyService::removeSubject(*sp,subj.toStdString());
            }
            catch (const isd::ISDException& e) {
                QMessageBox::warning(this,"Ошибка",e.what());
            }
        }
        break;
    }
    loadSpecialtyView();
}
