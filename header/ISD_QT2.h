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
    void refreshStudents();                
    void onAddStudent();                   
    void onSearchStudent();               
    void onImportCSV();                     
    void onStudentDoubleClicked(int row,int column); 

    void refreshGroups();                   
    void onAddGroup();                      
    void onSearchGroup();                   
    void onGroupDoubleClicked(int row,int column); 

    void refreshTeachers();                 
    void onAddTeacher();                    
    void onSearchTeacher();                 
    void onTeacherDoubleClicked(int row,int column); 

    void refreshSpecialties();              
    void onAddSpecialty();                  

    void refreshSubjects();                
    void onAddSubjectToSpecialty();         

private:
    Ui::ISD_QT2* ui;
    isd::Registry reg;

    int studentRowToId(int row) const;
    int groupRowToId(int row) const;
    int teacherRowToId(int row) const;
    int specialtyRowToId(int row) const;
};
