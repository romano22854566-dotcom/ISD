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
    ~ISD_QT2() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // === Студенты ===
    void refreshStudents();                 // Обновление таблицы студентов
    void onAddStudent();                    // Добавление студента
    void onSearchStudent();                 // Поиск по ФИО
    void onImportCSV();                     // Импорт студентов из CSV
    void onStudentDoubleClicked(int row,int column); // Двойной клик по студенту

    // === Группы ===
    void refreshGroups();                   // Обновление таблицы групп
    void onAddGroup();                      // Добавление группы
    void onSearchGroup();                   // Поиск по номеру группы
    void onGroupDoubleClicked(int row,int column); // Двойной клик по группе

    // === Преподаватели ===
    void refreshTeachers();                 // Обновление таблицы преподавателей
    void onAddTeacher();                    // Добавление преподавателя
    void onSearchTeacher();                 // Поиск по ФИО
    void onTeacherDoubleClicked(int row,int column); // Двойной клик по преподавателю

    // === Специальности ===
    void refreshSpecialties();              // Обновление таблицы специальностей
    void onAddSpecialty();                  // Добавление специальности

    // === Предметы ===
    void refreshSubjects();                 // Обновление таблицы предметов
    void onAddSubjectToSpecialty();         // Добавление предмета в специальность

private:
    Ui::ISD_QT2* ui;
    isd::Registry reg;

    // === Вспомогательные методы ===
    void exportStudentReport(int sid);      // Экспорт справки по студенту
    void refreshAllAfterSpecChange();       // Комплексное обновление после изменений специальностей/предметов

    // === Преобразование из индекс строки в ID ===
    int studentRowToId(int row) const;
    int groupRowToId(int row) const;
    int teacherRowToId(int row) const;
    int specialtyRowToId(int row) const;

    // === Ограничения для ввода ===
    static constexpr int MIN_STUDENT_AGE = 14;
    static constexpr int MAX_STUDENT_AGE = 120;
    static constexpr int DEFAULT_STUDENT_AGE = 18;
    static constexpr int MIN_TEACHER_AGE = 18;
    static constexpr int MAX_TEACHER_AGE = 120;
    static constexpr int DEFAULT_TEACHER_AGE = 30;
};
