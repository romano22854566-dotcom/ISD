#pragma once

#include <QMainWindow>
#include "registry.hpp"

QT_BEGIN_NAMESPACE
namespace Ui{ class ISD_QT2; }
QT_END_NAMESPACE

class ISD_QT2: public QMainWindow
{
    Q_OBJECT

public:
    explicit ISD_QT2(QWidget* parent = nullptr);
    ~ISD_QT2();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // === Студенты ===
    void refreshStudents();                 // обновление таблицы студентов
    void onAddStudent();                    // добавить студента
    void onSearchStudent();                 // поиск по ФИО
    void onImportCSV();                     // импорт студентов из CSV
    void onStudentDoubleClicked(int row,int column); // двойной клик по студенту

    // === Группы ===
    void refreshGroups();                   // обновление таблицы групп
    void onAddGroup();                      // добавить группу
    void onSearchGroup();                   // поиск по номеру группы
    void onGroupDoubleClicked(int row,int column); // двойной клик по группе

    // === Преподаватели ===
    void refreshTeachers();                 // обновление таблицы преподавателей
    void onAddTeacher();                    // добавить преподавателя
    void onSearchTeacher();                 // поиск по ФИО
    void onTeacherDoubleClicked(int row,int column); // двойной клик по преподавателю

    // === Специальности ===
    void refreshSpecialties();              // обновление таблицы специальностей
    void onAddSpecialty();                  // добавить специальность

    // === Предметы ===
    void refreshSubjects();                 // обновление таблицы предметов
    void onAddSubjectToSpecialty();         // добавить предмет в специальность

private:
    Ui::ISD_QT2* ui;
    isd::Registry reg;

    // === Вспомогательные методы ===
    void exportStudentReport(int sid);      // генерация справки по студенту
    void refreshAllAfterSpecChange();       // каскадное обновление после изменения специальности/предметов

    // === Привязка строк таблиц к ID ===
    int studentRowToId(int row) const;
    int groupRowToId(int row) const;
    int teacherRowToId(int row) const;
    int specialtyRowToId(int row) const;

    // === Константы для валидации ===
    static constexpr int MIN_STUDENT_AGE = 14;
    static constexpr int MAX_STUDENT_AGE = 120;
    static constexpr int DEFAULT_STUDENT_AGE = 18;
    static constexpr int MIN_TEACHER_AGE = 18;
    static constexpr int MAX_TEACHER_AGE = 120;
    static constexpr int DEFAULT_TEACHER_AGE = 30;
};
