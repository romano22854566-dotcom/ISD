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
#include <QDate>
#include <clocale>

#ifdef _WIN32
#include <Windows.h>
#endif
#include "student.hpp"
#include "StudentDialog.h"
#include "EditStudentDialog.h"
#include "GroupDialog.h"
#include "TeacherDialog.h"

#include "registry.hpp"

using namespace isd;

namespace{
// Общие утилиты

void setupTable(QTableWidget* tbl,int cols,const QStringList& headers) {
    tbl->clear();
    tbl->setRowCount(0);
    tbl->setColumnCount(cols);
    tbl->setHorizontalHeaderLabels(headers);
    tbl->verticalHeader()->setDefaultSectionSize(36);
}

void setSectionModes(QTableWidget* tbl,const std::vector<QHeaderView::ResizeMode>& modes) {
    auto* hdr = tbl->horizontalHeader();
    for (int i = 0; i < (int)modes.size(); ++i) {
        hdr->setSectionResizeMode(i,modes[i]);
    }
}

QPushButton* makeDeleteButton(const QString& text,const QObject* receiver,std::function<void()> onClick) {
    auto* btn = new QPushButton(text);
    QObject::connect(btn,&QPushButton::clicked,const_cast<QObject*>(receiver),std::move(onClick));
    return btn;
}

void filterByText(QTableWidget* tbl,int col,const QString& query) {
    for (int i = 0; i < tbl->rowCount(); ++i) {
        auto* item = tbl->item(i,col);
        const bool match = item && item->text().contains(query,Qt::CaseInsensitive);
        tbl->setRowHidden(i,!match);
    }
}

} // namespace

ISD_QT2::ISD_QT2(QWidget* parent)
    : QMainWindow(parent)
    ,ui(new Ui::ISD_QT2)
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
        QTabWidget::pane { border: 1px solid #444; padding: 4px; }
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
    catch (const ISDException&){ QMessageBox::warning(this,"Ошибка","Не удалось загрузить данные, начаты новые файлы."); }

    // Студенты
    connect(ui->tblStudents,&QTableWidget::cellDoubleClicked,this,&ISD_QT2::onStudentDoubleClicked);
    connect(ui->btnAddStudent,&QPushButton::clicked,this,&ISD_QT2::onAddStudent);
    connect(ui->btnSearchStudent,&QPushButton::clicked,this,&ISD_QT2::onSearchStudent);
    connect(ui->btnImportCSV,&QPushButton::clicked,this,&ISD_QT2::onImportCSV);

    // Группы
    connect(ui->tblGroups,&QTableWidget::cellDoubleClicked,this,&ISD_QT2::onGroupDoubleClicked);
    connect(ui->btnAddGroup,&QPushButton::clicked,this,&ISD_QT2::onAddGroup);
    connect(ui->btnSearchGroup,&QPushButton::clicked,this,&ISD_QT2::onSearchGroup);

    // Преподаватели
    connect(ui->tblTeachers,&QTableWidget::cellDoubleClicked,this,&ISD_QT2::onTeacherDoubleClicked);
    connect(ui->btnAddTeacher,&QPushButton::clicked,this,&ISD_QT2::onAddTeacher);
    connect(ui->btnSearchTeacher,&QPushButton::clicked,this,&ISD_QT2::onSearchTeacher);

    // Специальности / Предметы
    connect(ui->btnAddSpecialty,&QPushButton::clicked,this,&ISD_QT2::onAddSpecialty);
    connect(ui->btnAddSubjectToSpec,&QPushButton::clicked,this,&ISD_QT2::onAddSubjectToSpecialty);

    refreshStudents();
    refreshGroups();
    refreshTeachers();
    refreshSpecialties();
    refreshSubjects();
}

ISD_QT2::~ISD_QT2() { delete ui; }

void ISD_QT2::closeEvent(QCloseEvent* event) {
    try { reg.save(); }
    catch (const ISDException& e) { QMessageBox::critical(this,"Ошибка сохранения",e.what()); }
    event->accept();
}

int ISD_QT2::studentRowToId(int row) const {
    auto ids = reg.allStudentIds();
    return (row >= 0 && row < (int)ids.size()) ? (int)ids[row] : 0;
}
int ISD_QT2::groupRowToId(int row) const {
    auto ids = reg.allGroupIds();
    return (row >= 0 && row < (int)ids.size()) ? (int)ids[row] : 0;
}
int ISD_QT2::teacherRowToId(int row) const {
    auto ids = reg.allTeacherIds();
    return (row >= 0 && row < (int)ids.size()) ? (int)ids[row] : 0;
}
int ISD_QT2::specialtyRowToId(int row) const {
    auto ids = reg.allSpecialtyIds();
    return (row >= 0 && row < (int)ids.size()) ? (int)ids[row] : 0;
}

void ISD_QT2::exportStudentReport(int sid) {
    const Student* s = reg.getStudent(sid);
    if (!s) return;

    QString specialty = "Не указана";
    if (const Group* g = reg.findGroup(s->groupName_); g && !g->specialtyName_.empty()) {
        specialty = QString::fromStdString(g->specialtyName_);
    }

    const QString fileName = QFileDialog::getSaveFileName(this,"Сохранить справку",".","Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);

    out << "СПРАВКА ОБ УСПЕВАЕМОСТИ СТУДЕНТА\n\n";
    out << "ФИО: " << QString::fromStdString(s->name_) << "\n";
    out << "Возраст: " << QString::number(s->age_) << "\n";
    out << "Группа: " << QString::fromStdString(s->groupName_) << "\n";
    out << "Специальность: " << specialty << "\n\n";

    out << "+----+-------------------------+--------------+------------------+\n";
    out << QString("| %1 | %2 | %3 | %4 |\n")
        .arg("№",-1)
        .arg("Предмет",-23)
        .arg("Средний балл",11)
        .arg("Пропуски (всего)",13);
    out << "+----+-------------------------+--------------+------------------+\n";

    int index = 1; double totalAvg = 0.0; int subjCount = 0; int totalAbs = 0;

    const auto& recs = s->records_;
    for (const auto& [subj,rec] : recs) {
        double sum = 0.0; int cnt = 0; int abs = 0;

        for (const auto& [_,gr] : rec.grades) {
            for (size_t i = 0; i < gr.vals_.size(); ++i) {
                sum += gr.vals_[i];
                ++cnt;
            }
        }
        for (const auto& [_,a] : rec.absences) {
            abs += a;
        }

        const double avg = (cnt > 0) ? (sum / cnt) : 0.0;

        out << QString("| %1 | %2 | %3 | %4 |\n")
            .arg(index,2)
            .arg(QString::fromStdString(subj),-23)
            .arg(QString::number(avg,'f',2),12)
            .arg(QString::number(abs),16);

        totalAvg += avg;
        ++subjCount;
        totalAbs += abs;
        ++index;
    }

    out << "+----+-------------------------+--------------+------------------+\n\n";

    const double finalAvg = (subjCount > 0) ? (totalAvg / subjCount) : 0.0;
    out << "Общее количество пропусков: " << totalAbs << "\n";
    out << "Средний балл по всем предметам: " << QString::number(finalAvg,'f',2) << "\n\n";

    const QDate today = QDate::currentDate();
    out << "Дата выдачи: " << today.toString("dd.MM.yyyy") << "\n\n";
    out << "Декан: _____________________________\n";
}

void ISD_QT2::refreshAllAfterSpecChange() {
    refreshSpecialties();
    refreshSubjects();
    refreshGroups();
    refreshStudents();
}

void ISD_QT2::refreshStudents()
{
    setupTable(ui->tblStudents,6,{"ФИО","Возраст","Группа","","",""});
    setSectionModes(ui->tblStudents,{
        QHeaderView::Stretch,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents
    });
    ui->tblStudents->setColumnWidth(3,100);
    ui->tblStudents->setColumnWidth(4,100);
    ui->tblStudents->setColumnWidth(5,100);

    auto ids = reg.allStudentIds();
    ui->tblStudents->setRowCount((int)ids.size());

    for (int row = 0; row < (int)ids.size(); ++row) {
        const Id id = ids[row];
        const Student* s = reg.getStudent(id);
        ui->tblStudents->setItem(row,0,new QTableWidgetItem(QString::fromStdString(s->name_)));
        ui->tblStudents->setItem(row,1,new QTableWidgetItem(QString::number(s->age_)));
        ui->tblStudents->setItem(row,2,new QTableWidgetItem(QString::fromStdString(s->groupName_)));

        // Редактирование
        auto* btnEdit = new QPushButton("Редакт.");
        connect(btnEdit,&QPushButton::clicked,this,[this,row]{
            const int sid = studentRowToId(row);
            if (!sid) return;
            EditStudentDialog dlg(reg,(Id)sid,this);
            dlg.exec();
            refreshStudents();
        });
        ui->tblStudents->setCellWidget(row,3,btnEdit);

        // Справка
        auto* btnRep = new QPushButton("Справка");
        connect(btnRep,&QPushButton::clicked,this,[this,row]{
            const int sid = studentRowToId(row);
            if (!sid) return;
            exportStudentReport(sid);
        });
        ui->tblStudents->setCellWidget(row,4,btnRep);

        // Удаление
        auto* btnDel = makeDeleteButton("Удалить",this,[this,row]{
            const int sid = studentRowToId(row);
            if (sid) { reg.removeStudent((Id)sid); refreshStudents(); }
        });
        ui->tblStudents->setCellWidget(row,5,btnDel);
    }
}

void ISD_QT2::onAddStudent()
{
    const QString name = QInputDialog::getText(this,"Добавить студента","ФИО:");
    if (name.trimmed().isEmpty()) return;
    const int age = QInputDialog::getInt(this,"Возраст","Возраст:",DEFAULT_STUDENT_AGE,MIN_STUDENT_AGE,MAX_STUDENT_AGE);

    auto gids = reg.allGroupIds();
    if (gids.empty()) {
        QMessageBox::warning(this,"Нет групп","Сначала добавьте группу.");
        return;
    }

    QStringList groupNames;
    for (auto gid : gids) {
        const Group* g = reg.getGroup(gid);
        groupNames << QString::fromStdString(g->name_);
    }

    bool ok = false;
    const QString gname = QInputDialog::getItem(this,"Группа","Выберите группу:",groupNames,0,false,&ok);
    if (!ok || gname.isEmpty()) return;

    try {
        Student s(name.toStdString(),age,gname.toStdString());
        if (const Group* g = reg.findGroup(gname.toStdString()); g && !g->specialtyName_.empty()) {
            const Specialty* sp = reg.findSpecialty(g->specialtyName_);
            if (sp) StudentService::ensureSubjectsFromSpecialty(s,*sp);
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
    const QString query = ui->editSearchStudent->text().trimmed();
    filterByText(ui->tblStudents,0,query);
}

void ISD_QT2::onImportCSV()
{
    const QString fileName = QFileDialog::getOpenFileName(this,"Импорт CSV",".","CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this,"Ошибка","Не удалось открыть CSV");
        return;
    }
    QTextStream in(&f);
    int imported = 0;
    int failed = 0;
    while (!in.atEnd()){
        const QString line = in.readLine();
        const QStringList parts = line.split(",",Qt::KeepEmptyParts);
        if (parts.size() < 3) continue;
        const QString name = parts[0].trimmed();
        const int age = parts[1].trimmed().toInt();
        const QString group = parts[2].trimmed();
        if (name.isEmpty() || group.isEmpty()) continue;
        try {
            Student s(name.toStdString(),age,group.toStdString());
            if (const Group* gr = reg.findGroup(group.toStdString()); gr && !gr->specialtyName_.empty()){
                const Specialty* sp = reg.findSpecialty(gr->specialtyName_);
                if (sp) StudentService::ensureSubjectsFromSpecialty(s,*sp);
            }
            reg.addStudent(s);
            ++imported;
        }
        catch (const ISDException&) {
            ++failed;
        }
    }
    QMessageBox::information(this,"Импорт завершён",QString("Импортировано записей: %1. Ошибок: %2").arg(imported).arg(failed));
    refreshStudents();
}

void ISD_QT2::onStudentDoubleClicked(int row,int)
{
    const int id = studentRowToId(row);
    if (!id) return;
    StudentDialog dlg(reg,(Id)id,this);
    dlg.exec();
    refreshStudents();
}

void ISD_QT2::refreshGroups()
{
    setupTable(ui->tblGroups,3,{"Группа","Специальность",""});
    setSectionModes(ui->tblGroups,{
        QHeaderView::Stretch,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents
    });
    ui->tblGroups->setColumnWidth(2,100);

    auto ids = reg.allGroupIds();
    ui->tblGroups->setRowCount((int)ids.size());

    for (int row = 0; row < (int)ids.size(); ++row) {
        const Id id = ids[row];
        const Group* g = reg.getGroup(id);
        ui->tblGroups->setItem(row,0,new QTableWidgetItem(QString::fromStdString(g->name_)));
        ui->tblGroups->setItem(row,1,new QTableWidgetItem(QString::fromStdString(g->specialtyName_)));

        auto* btn = makeDeleteButton("Удалить",this,[this,row]{
            const int gid = groupRowToId(row);
            if (gid) {
                reg.removeGroup((Id)gid);
                refreshGroups();
                refreshStudents();
            }
        });
        ui->tblGroups->setCellWidget(row,2,btn);
    }
}

void ISD_QT2::onAddGroup()
{
    const QString name = QInputDialog::getText(this,"Добавить группу","Номер группы:");
    if (name.trimmed().isEmpty()) return;

    auto sids = reg.allSpecialtyIds();
    if (sids.empty()){
        QMessageBox::warning(this,"Нет специальностей","Сначала добавьте специальность.");
        return;
    }
    QStringList specNames;
    for (auto sid : sids) {
        const Specialty* sp = reg.getSpecialty(sid);
        specNames << QString::fromStdString(sp->name_);
    }

    bool ok = false;
    const QString spname = QInputDialog::getItem(this,"Специальность","Выберите:",specNames,0,false,&ok);
    if (!ok || spname.isEmpty()) return;

    try {
        Group g(name.toStdString());
        GroupService::setSpecialty(g,spname.toStdString());
        reg.addGroup(g);
        refreshGroups();
    }
    catch (const ISDException& e) {
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void ISD_QT2::onSearchGroup()
{
    const QString query = ui->editSearchGroup->text().trimmed();
    filterByText(ui->tblGroups,0,query);
}

void ISD_QT2::onGroupDoubleClicked(int row,int)
{
    const int id = groupRowToId(row);
    if (!id) return;
    GroupDialog dlg(reg,(Id)id,this);
    dlg.exec();
    refreshGroups();
    refreshStudents();
}

void ISD_QT2::refreshTeachers()
{
    setupTable(ui->tblTeachers,3,{"ФИО","Возраст",""});
    setSectionModes(ui->tblTeachers,{
        QHeaderView::Stretch,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents
    });
    ui->tblTeachers->setColumnWidth(2,100);

    auto ids = reg.allTeacherIds();
    ui->tblTeachers->setRowCount((int)ids.size());

    for (int row = 0; row < (int)ids.size(); ++row) {
        const Id id = ids[row];
        const Teacher* t = reg.getTeacher(id);
        ui->tblTeachers->setItem(row,0,new QTableWidgetItem(QString::fromStdString(t->fullName())));
        ui->tblTeachers->setItem(row,1,new QTableWidgetItem(QString::number(t->age())));

        auto* btn = makeDeleteButton("Удалить",this,[this,row]{
            const int tid = teacherRowToId(row);
            if (tid) { reg.removeTeacher((Id)tid); refreshTeachers(); }
        });
        ui->tblTeachers->setCellWidget(row,2,btn);
    }
}

void ISD_QT2::onAddTeacher()
{
    const QString name = QInputDialog::getText(this,"Добавить преподавателя","ФИО:");
    if (name.trimmed().isEmpty()) return;
    const int age = QInputDialog::getInt(this,"Возраст","Возраст:",DEFAULT_TEACHER_AGE,MIN_TEACHER_AGE,MAX_TEACHER_AGE);
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
    const QString query = ui->editSearchTeacher->text().trimmed();
    filterByText(ui->tblTeachers,0,query);
}

void ISD_QT2::onTeacherDoubleClicked(int row,int)
{
    const int id = teacherRowToId(row);
    if (!id) return;
    TeacherDialog dlg(reg,(Id)id,this);
    dlg.exec();
    refreshTeachers();
}

void ISD_QT2::refreshSpecialties()
{
    setupTable(ui->tblSpecialties,2,{"Название специальности",""});
    setSectionModes(ui->tblSpecialties,{
        QHeaderView::Stretch,
        QHeaderView::ResizeToContents
    });
    ui->tblSpecialties->setColumnWidth(1,100);

    auto ids = reg.allSpecialtyIds();
    ui->tblSpecialties->setRowCount((int)ids.size());

    for (int row = 0; row < (int)ids.size(); ++row) {
        const Id id = ids[row];
        const Specialty* sp = reg.getSpecialty(id);
        ui->tblSpecialties->setItem(row,0,new QTableWidgetItem(QString::fromStdString(sp->name_)));

        auto* btn = makeDeleteButton("Удалить",this,[this,id]{
            reg.removeSpecialty(id);
            refreshAllAfterSpecChange();
        });
        ui->tblSpecialties->setCellWidget(row,1,btn);
    }
}

void ISD_QT2::onAddSpecialty()
{
    const QString name = QInputDialog::getText(this,"Добавить специальность","Название:");
    if (name.trimmed().isEmpty()) return;
    try {
        Specialty sp(name.toStdString());
        reg.addSpecialty(sp);
        refreshAllAfterSpecChange();
    }
    catch (const ISDException& e){
        QMessageBox::warning(this,"Ошибка",e.what());
    }
}

void ISD_QT2::refreshSubjects()
{
    setupTable(ui->tblSubjects,4,{"Специальность","Предмет","Контроль",""});
    setSectionModes(ui->tblSubjects,{
        QHeaderView::ResizeToContents,
        QHeaderView::Stretch,
        QHeaderView::ResizeToContents,
        QHeaderView::ResizeToContents
    });
    ui->tblSubjects->setColumnWidth(3,120);

    int row = 0;
    auto sids = reg.allSpecialtyIds();
    for (auto id : sids) {
        const Specialty* sp = reg.getSpecialty(id);
        if (!sp) continue;

        for (const auto& s : sp->subjects_) {
            ui->tblSubjects->insertRow(row);
            ui->tblSubjects->setItem(row,0,new QTableWidgetItem(QString::fromStdString(sp->name_)));
            ui->tblSubjects->setItem(row,1,new QTableWidgetItem(QString::fromStdString(s.name)));
            const QString ctl = (s.control == ControlType::Zachet) ? "Зачёт" : "Экзамен";
            ui->tblSubjects->setItem(row,2,new QTableWidgetItem(ctl));

            auto* btn = makeDeleteButton("Удалить",this,[this,id,s]{
                if (auto* spm = reg.getSpecialtyMutable(id); spm) {
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
    auto sids = reg.allSpecialtyIds();
    if (sids.empty()) {
        QMessageBox::information(this,"Специальности","Нет специальностей.");
        return;
    }

    QStringList specNames;
    for (auto sid : sids) {
        const Specialty* sp = reg.getSpecialty(sid);
        specNames << QString::fromStdString(sp->name_);
    }

    bool ok = false;
    const QString spname = QInputDialog::getItem(this,"Специальность","Выберите:",specNames,0,false,&ok);
    if (!ok || spname.isEmpty()) return;

    const QString subj = QInputDialog::getText(this,"Добавить предмет","Название предмета:");
    if (subj.trimmed().isEmpty()) return;

    const QString ctrl = QInputDialog::getItem(this,"Тип контроля","Выберите:",{"Зачёт","Экзамен"},0,false,&ok);
    if (!ok || ctrl.isEmpty()) return;
    const ControlType ct = (ctrl == "Экзамен") ? ControlType::Exam : ControlType::Zachet;

    const std::vector<ClassType> types = {ClassType::LK, ClassType::PZ, ClassType::LR};

    for (auto sid : sids) {
        Specialty* sp = reg.getSpecialtyMutable(sid);
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
    refreshAllAfterSpecChange();
}
