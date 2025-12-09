#include "ISD_QT2.h"
#include "ui_ISD_QT2.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTextStream>
#include <QHeaderView>
#include <QPushButton>
#include <QFile>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif
#include "EditStudentDialog.h"
#include "StudentDialog.h"
#include "TeacherDialog.h"
#include "GroupDialog.h"
#include "StudentService.hpp"
#include "SubjectRecordService.hpp"
#include "GroupService.hpp"
#include "TeacherService.hpp"
#include "SpecialtyService.hpp"
#include "EditStudentDialog.h"

using namespace isd;

ISD_QT2::ISD_QT2(QWidget* parent)
    : QMainWindow(parent)
    ,ui(new Ui::ISD_QT2)
    ,reg("students.txt","teachers.txt","groups.txt","specialties.txt")
{
    ui->setupUi(this);
    showMaximized();

    
    qApp->setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #2b2b2b;
            color: #f0f0f0;
            font-family: Segoe UI, sans-serif;
            font-size: 14px;
        }
        QTabWidget::pane {
            border: 1px solid #444;
            padding: 4px;
        }
        QHeaderView::section {
            background-color: #3c3c3c;
            color: #e0e0e0;
            padding: 6px;
            border: none;
        }
        QTableWidget {
            background-color: #1e1e1e;
            alternate-background-color: #2a2a2a;
            gridline-color: #444;
            selection-background-color: #4CAF50;
            selection-color: white;
        }
        QTableWidget::item { padding: 4px; }
        QLineEdit, QSpinBox {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #666;
            padding: 4px;
            border-radius: 4px;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 6px 12px;
            border-radius: 4px;
        }
        QPushButton:hover { background-color: #45a049; }
        QPushButton:disabled { background-color: #777; color: #ccc; }
        QDialog { background-color: #2b2b2b; }
        QStatusBar { background: #2b2b2b; color: #a0a0a0; }
    )");

#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    std::setlocale(LC_ALL,"Russian");

    try { reg.load(); }
    catch (...) { QMessageBox::warning(this,"Ошибка","Не удалось загрузить данные, начаты новые файлы."); }

    
    connect(ui->tblStudents,&QTableWidget::cellDoubleClicked,this,&ISD_QT2::onStudentDoubleClicked);
    connect(ui->btnAddStudent,&QPushButton::clicked,this,&ISD_QT2::onAddStudent);
    connect(ui->btnSearchStudent,&QPushButton::clicked,this,&ISD_QT2::onSearchStudent);
    connect(ui->btnImportCSV,&QPushButton::clicked,this,&ISD_QT2::onImportCSV);

    
    connect(ui->tblGroups,&QTableWidget::cellDoubleClicked,this,&ISD_QT2::onGroupDoubleClicked);
    connect(ui->btnAddGroup,&QPushButton::clicked,this,&ISD_QT2::onAddGroup);
    connect(ui->btnSearchGroup,&QPushButton::clicked,this,&ISD_QT2::onSearchGroup);

    
    connect(ui->tblTeachers,&QTableWidget::cellDoubleClicked,this,&ISD_QT2::onTeacherDoubleClicked);
    connect(ui->btnAddTeacher,&QPushButton::clicked,this,&ISD_QT2::onAddTeacher);
    connect(ui->btnSearchTeacher,&QPushButton::clicked,this,&ISD_QT2::onSearchTeacher);

    connect(ui->btnAddSpecialty,&QPushButton::clicked,this,&ISD_QT2::onAddSpecialty);
    connect(ui->btnAddSubjectToSpec,&QPushButton::clicked,this,&ISD_QT2::onAddSubjectToSpecialty);

    refreshStudents();
    refreshGroups();
    refreshTeachers();
    refreshSpecialties();
    refreshSubjects();
}

ISD_QT2::~ISD_QT2() { delete ui; }

void ISD_QT2::closeEvent(QCloseEvent* event)
{
    try { reg.save(); }
    catch (const ISDException& e) { QMessageBox::critical(this,"Ошибка сохранения",e.what()); }
    event->accept();
}
int ISD_QT2::studentRowToId(int row) const {
    auto ids = reg.allStudentIds();
    if (row < 0 || row >= (int)ids.size()) return 0;
    return (int)ids[row];
}
int ISD_QT2::groupRowToId(int row) const {
    auto ids = reg.allGroupIds();
    if (row < 0 || row >= (int)ids.size()) return 0;
    return (int)ids[row];
}
int ISD_QT2::teacherRowToId(int row) const {
    auto ids = reg.allTeacherIds();
    if (row < 0 || row >= (int)ids.size()) return 0;
    return (int)ids[row];
}
int ISD_QT2::specialtyRowToId(int row) const {
    auto ids = reg.allSpecialtyIds();
    if (row < 0 || row >= (int)ids.size()) return 0;
    return (int)ids[row];
}

void ISD_QT2::refreshStudents()
{
    ui->tblStudents->clear();
    ui->tblStudents->setRowCount(0);
    ui->tblStudents->setColumnCount(6);
    ui->tblStudents->setHorizontalHeaderLabels({"ФИО","Возраст","Группа","","",""});
    ui->tblStudents->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tblStudents->setColumnWidth(3,100);
    ui->tblStudents->setColumnWidth(4,100);
    ui->tblStudents->setColumnWidth(5,100);
    ui->tblStudents->verticalHeader()->setDefaultSectionSize(36);

    auto ids = reg.allStudentIds();
    ui->tblStudents->setRowCount((int)ids.size());
    int row = 0;
    for (auto id : ids) {
        auto s = reg.getStudent(id);
        ui->tblStudents->setItem(row,0,new QTableWidgetItem(QString::fromStdString(StudentService::fullName(*s))));
        ui->tblStudents->setItem(row,1,new QTableWidgetItem(QString::number(StudentService::age(*s))));
        ui->tblStudents->setItem(row,2,new QTableWidgetItem(QString::fromStdString(StudentService::group(*s))));

        auto btnEdit = new QPushButton("Редакт.");
        connect(btnEdit,&QPushButton::clicked,this,[=]{
            int sid = studentRowToId(row);
            if (!sid) return;
            EditStudentDialog dlg(reg,sid,this);
            dlg.exec();
            refreshStudents();
        });
        ui->tblStudents->setCellWidget(row,3,btnEdit);

        
        auto btnRep = new QPushButton("Справка");
connect(btnRep,&QPushButton::clicked,this,[=]{
    int sid = studentRowToId(row);
    if (!sid) return;
    const auto* s = reg.getStudent(sid);
    if (!s) return;
    const auto* stud = reg.getStudent(sid);
    QString specialty = "Не указана";
    if (auto g = reg.findGroup(StudentService::group(*stud))) {
        if (!GroupService::specialty(*g).empty())
            specialty = QString::fromStdString(GroupService::specialty(*g));
    }

    QString fileName = QFileDialog::getSaveFileName(this,"Сохранить справку",".","Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);

    
    out << "СПРАВКА ОБ УСПЕВАЕМОСТИ СТУДЕНТА\n\n";
    out << "ФИО: " << QString::fromStdString(isd::StudentService::fullName(*s)) << "\n";
    out << "Возраст: " << StudentService::age(*s)<< "\n";
    out << "Группа: " << QString::fromStdString(isd::StudentService::group(*s)) << "\n";
    out << "Специальность: " << specialty << "\n\n";

    out << "+----+-------------------------+--------------+------------------+\n";
    out << QString("| %1 | %2 | %3 | %4 |\n")
           .arg("№", -1)                          
           .arg("Предмет", -23)                   
           .arg("Средний балл", 11)               
           .arg("Пропуски (всего)", 13);          
    out << "+----+-------------------------+--------------+------------------+\n";

    int index = 1;
    double totalAvg = 0.0;
    int subjCount = 0;
    int totalAbs = 0;

    for (const auto& [subj,rec] : StudentService::records(*stud)) {
        double sum = 0.0; int cnt = 0; int abs = 0;
        for (const auto& gkv : rec.grades)
            for (int v : gkv.second.vals_) { sum += v; ++cnt; }
        for (const auto& akv : rec.absences) abs += akv.second;
        double avg = (cnt > 0) ? sum / cnt : 0.0;

        out << QString("| %1 | %2 | %3 | %4 |\n")
               .arg(index, 2)                          
               .arg(QString::fromStdString(subj), -23) 
               .arg(QString::number(avg,'f',2), 12)    
               .arg(QString::number(abs), 16);         

        totalAvg += avg;
        ++subjCount;
        totalAbs += abs;
        ++index;
    }

    out << "+----+-------------------------+--------------+------------------+\n\n";

    double finalAvg = (subjCount > 0) ? (totalAvg / subjCount) : 0.0;
    out << "Общее количество пропусков: " << totalAbs << "\n";
    out << "Средний балл по всем предметам: " << QString::number(finalAvg,'f',2) << "\n\n";

    QDate today = QDate::currentDate();
    out << "Дата выдачи: " << today.toString("dd.MM.yyyy") << "\n\n";
    out << "Декан: _____________________________\n";
});




        ui->tblStudents->setCellWidget(row,4,btnRep);

        auto btnDel = new QPushButton("Удалить");
        connect(btnDel,&QPushButton::clicked,this,[=]{
            int sid = studentRowToId(row);
            if (sid) { reg.removeStudent(sid); refreshStudents(); }
        });
        ui->tblStudents->setCellWidget(row,5,btnDel);

        ++row;
    }
}

void ISD_QT2::onAddStudent()
{
    QString name = QInputDialog::getText(this,"Добавить студента","ФИО:");
    if (name.trimmed().isEmpty()) return;
    int age = QInputDialog::getInt(this,"Возраст","Возраст:",18,14,120);

    auto gids = reg.allGroupIds();
    if (gids.empty()) {
        QMessageBox::warning(this,"Нет групп","Сначала добавьте группу.");
        return;
    }

    QStringList groupNames;
    for (auto id : gids) groupNames << QString::fromStdString(reg.getGroup(id)->name_);

    bool ok = false;
    QString g = QInputDialog::getItem(this,"Группа","Выберите группу:",groupNames,0,false,&ok);
    if (!ok || g.isEmpty()) return;

    try {
        Student s(name.toStdString(),age,g.toStdString());
        if (auto gr = reg.findGroup(g.toStdString())){
            if (!gr->specialtyName_.empty()){
                if (auto sp = reg.findSpecialty(gr->specialtyName_))
                    StudentService::ensureSubjectsFromSpecialty(s,*sp);
            }

        }
        reg.addStudent(s);
        refreshStudents();
    }
    catch (const ISDException& e) {
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void ISD_QT2::onSearchStudent()
{
    QString query = ui->editSearchStudent->text().trimmed();
    for (int i = 0; i < ui->tblStudents->rowCount(); ++i) {
        auto item = ui->tblStudents->item(i,0);
        bool match = item && item->text().contains(query,Qt::CaseInsensitive);
        ui->tblStudents->setRowHidden(i,!match);
    }
}

void ISD_QT2::onImportCSV()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Импорт CSV",".","CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this,"Ошибка","Не удалось открыть CSV");
        return;
    }
    QTextStream in(&f);
    int imported = 0;
    while (!in.atEnd()){
        QString line = in.readLine();
        auto parts = line.split(",",Qt::KeepEmptyParts);
        if (parts.size() < 3) continue;
        QString name = parts[0].trimmed();
        int age = parts[1].trimmed().toInt();
        QString group = parts[2].trimmed();
        if (name.isEmpty() || group.isEmpty()) continue;
        try {
            Student s(name.toStdString(),age,group.toStdString());
            if (auto gr = reg.findGroup(group.toStdString())){
                if (!gr->specialtyName_.empty()){
                    if (auto sp = reg.findSpecialty(gr->specialtyName_))
                        StudentService::ensureSubjectsFromSpecialty(s,*sp);
                }

            }
            reg.addStudent(s);
            ++imported;
        }
        catch (...) {}
    }
    QMessageBox::information(this,"Импорт завершён",QString("Импортировано записей: %1").arg(imported));
    refreshStudents();
}

void ISD_QT2::onStudentDoubleClicked(int row,int)
{
    int id = studentRowToId(row);
    if (!id) return;
    StudentDialog dlg(reg,id,this);
    dlg.exec();
    refreshStudents();
}

void ISD_QT2::refreshGroups()
{
    ui->tblGroups->clear();
    ui->tblGroups->setRowCount(0);
    ui->tblGroups->setColumnCount(3);
    ui->tblGroups->setHorizontalHeaderLabels({"Группа","Специальность",""});
    ui->tblGroups->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tblGroups->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tblGroups->setColumnWidth(2,100);
    ui->tblGroups->verticalHeader()->setDefaultSectionSize(36);

    auto ids = reg.allGroupIds();
    ui->tblGroups->setRowCount((int)ids.size());
    int row = 0;
    for (auto id : ids) {
        auto g = reg.getGroup(id);
        ui->tblGroups->setItem(row,0,new QTableWidgetItem(QString::fromStdString(g->name_)));
        ui->tblGroups->setItem(row,1,new QTableWidgetItem(QString::fromStdString(g->specialtyName_)));

        auto btn = new QPushButton("Удалить");
        connect(btn,&QPushButton::clicked,this,[=]{
            int gid = groupRowToId(row);
            if (gid) {
                reg.removeGroup(gid);
                refreshGroups();
                refreshStudents();
            }
        });
        ui->tblGroups->setCellWidget(row,2,btn);
        ++row;
    }
}


void ISD_QT2::onAddGroup()
{
    QString name = QInputDialog::getText(this,"Добавить группу","Номер группы:");
    if (name.trimmed().isEmpty()) return;
    auto sids = reg.allSpecialtyIds();
    if (sids.empty()){
        QMessageBox::warning(this,"Нет специальностей","Сначала добавьте специальность.");
        return;
    }
    QStringList specNames;
    for (auto id : sids) specNames << QString::fromStdString(reg.getSpecialty(id)->name_);

    bool ok = false;
    QString sp = QInputDialog::getItem(this,"Специальность","Выберите:",specNames,0,false,&ok);
    if (!ok || sp.isEmpty()) return;
    try {
        Group g(name.toStdString());
        GroupService::setSpecialty(g,sp.toStdString());

        reg.addGroup(g);
        refreshGroups();
    }
    catch (const ISDException& e) {
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void ISD_QT2::onSearchGroup()
{
    QString query = ui->editSearchGroup->text().trimmed();
    for (int i = 0; i < ui->tblGroups->rowCount(); ++i) {
        auto item = ui->tblGroups->item(i,0);
        bool match = item && item->text().contains(query,Qt::CaseInsensitive);
        ui->tblGroups->setRowHidden(i,!match);
    }
}

void ISD_QT2::onGroupDoubleClicked(int row,int)
{
    int id = groupRowToId(row);
    if (!id) return;
    GroupDialog dlg(reg,id,this);
    dlg.exec();
    refreshGroups();
    refreshStudents();
}

void ISD_QT2::refreshTeachers()
{
    ui->tblTeachers->clear();
    ui->tblTeachers->setRowCount(0);
    ui->tblTeachers->setColumnCount(3);
    ui->tblTeachers->setHorizontalHeaderLabels({"ФИО","Возраст",""});
    ui->tblTeachers->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tblTeachers->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tblTeachers->setColumnWidth(2,100);
    ui->tblTeachers->verticalHeader()->setDefaultSectionSize(36);

    auto ids = reg.allTeacherIds();
    ui->tblTeachers->setRowCount((int)ids.size());
    int row = 0;
    for (auto id : ids) {
        auto t = reg.getTeacher(id);
        ui->tblTeachers->setItem(row,0,new QTableWidgetItem(QString::fromStdString(t->fullName())));
        ui->tblTeachers->setItem(row,1,new QTableWidgetItem(QString::number(t->age())));
        auto btn = new QPushButton("Удалить");
        connect(btn,&QPushButton::clicked,this,[=]{
            int tid = teacherRowToId(row);
            if (tid) { reg.removeTeacher(tid); refreshTeachers(); }
        });
        ui->tblTeachers->setCellWidget(row,2,btn);
        ++row;
    }
}

void ISD_QT2::onAddTeacher()
{
    QString name = QInputDialog::getText(this,"Добавить преподавателя","ФИО:");
    if (name.trimmed().isEmpty()) return;
    int age = QInputDialog::getInt(this,"Возраст","Возраст:",30,18,120);
    try {
        Teacher t(name.toStdString(),age);
        reg.addTeacher(t);
        refreshTeachers();
    }
    catch (const ISDException& e) {
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void ISD_QT2::onSearchTeacher()
{
    QString query = ui->editSearchTeacher->text().trimmed();
    for (int i = 0; i < ui->tblTeachers->rowCount(); ++i) {
        auto item = ui->tblTeachers->item(i,0);
        bool match = item && item->text().contains(query,Qt::CaseInsensitive);
        ui->tblTeachers->setRowHidden(i,!match);
    }
}

void ISD_QT2::onTeacherDoubleClicked(int row,int)
{
    int id = teacherRowToId(row);
    if (!id) return;
    TeacherDialog dlg(reg,id,this);
    dlg.exec();
    refreshTeachers();
}

void ISD_QT2::refreshSpecialties()
{
    ui->tblSpecialties->clear();
    ui->tblSpecialties->setRowCount(0);
    ui->tblSpecialties->setColumnCount(2);
    ui->tblSpecialties->setHorizontalHeaderLabels({"Название специальности",""});
    ui->tblSpecialties->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tblSpecialties->setColumnWidth(1,100);
    ui->tblSpecialties->verticalHeader()->setDefaultSectionSize(36);

    auto ids = reg.allSpecialtyIds();
    ui->tblSpecialties->setRowCount((int)ids.size());
    int row = 0;
    for (auto id : ids) {
        const auto* sp = reg.getSpecialty(id);
        ui->tblSpecialties->setItem(row,0,new QTableWidgetItem(QString::fromStdString(sp->name_)));

        auto btn = new QPushButton("Удалить");
        connect(btn,&QPushButton::clicked,this,[=]{
            reg.removeSpecialty(id);
            refreshSpecialties();
            refreshSubjects();
            refreshGroups();
            refreshStudents();
        });
        ui->tblSpecialties->setCellWidget(row,1,btn);
        ++row;
    }
}


void ISD_QT2::onAddSpecialty()
{
    refreshSpecialties();
    refreshSubjects();
    QString name = QInputDialog::getText(this,"Добавить специальность","Название:");
    if (name.trimmed().isEmpty()) return;
    try {
        Specialty sp(name.toStdString());
        reg.addSpecialty(sp);
        refreshSpecialties();
        refreshSubjects();
    }
    catch (const ISDException& e){
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}


void ISD_QT2::refreshSubjects()
{
    ui->tblSubjects->clear();
    ui->tblSubjects->setRowCount(0);
    ui->tblSubjects->setColumnCount(4);
    ui->tblSubjects->setHorizontalHeaderLabels({"Специальность","Предмет","Контроль",""});
    ui->tblSubjects->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tblSubjects->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tblSubjects->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tblSubjects->setColumnWidth(3,120);
    ui->tblSubjects->verticalHeader()->setDefaultSectionSize(36);

    int row = 0;
    for (auto id : reg.allSpecialtyIds()) {
        const auto* sp = reg.getSpecialty(id);
        if (!sp) continue;
        for (const auto& s : sp->subjects_)
        {
            ui->tblSubjects->insertRow(row);
            ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(sp->name_)));
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(QString::fromStdString(s.name)));
            QString ctl = (s.control == ControlType::Zachet) ? "Зачёт" : "Экзамен";
            ui->tblSubjects->setItem(row,2,new QTableWidgetItem(ctl));

            auto btn = new QPushButton("Удалить");
            connect(btn,&QPushButton::clicked,this,[=]{
                auto* spm = reg.getSpecialtyMutable(id);
                if (spm) {
                    try { SpecialtyService::removeSubject(*spm,s.name); }
                    catch (const ISDException& e){ QMessageBox::warning(this,"Ошибка",e.what()); }
                }
                refreshSubjects();
                refreshStudents();
            });
            ui->tblSubjects->setCellWidget(row,3,btn);
            ++row;
        }
    }
}

       
void ISD_QT2::onAddSubjectToSpecialty()
{
    refreshSpecialties();
    refreshSubjects();
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
    ControlType ct = (ctrl == "Экзамен") ? ControlType::Exam : ControlType::Zachet;

    std::vector<ClassType> types = {ClassType::LK, ClassType::PZ, ClassType::LR};

    for (auto id : sids) {
        auto* sp = reg.getSpecialtyMutable(id);
        if (sp && sp->name_ == spname.toStdString()) {
            try {
                SpecialtyService::addSubject(*sp,SpecSubject{subj.toStdString(), ct, types});
            }
            catch (const ISDException& e) {
                QMessageBox::warning(this,"Ошибка",e.what());
            }
            break;
        }
    }
    refreshSpecialties();   
    refreshSubjects();      
    refreshStudents();      

    }


