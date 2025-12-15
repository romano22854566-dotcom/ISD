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

    void exportStudentReport(int sid);      
    void refreshAllAfterSpecChange();       

    int studentRowToId(int row) const;
    int groupRowToId(int row) const;
    int teacherRowToId(int row) const;
    int specialtyRowToId(int row) const;
    void saveSpecialtyName(isd::Id specialtyId,int row); 
    void addSubjectRowToTable(int& row,isd::Id specialtyId,const isd::Specialty* sp,const isd::SpecSubject& s); 
    void disableSubjectTableEditing(); 
    static constexpr int MIN_STUDENT_AGE = 14;
    static constexpr int MAX_STUDENT_AGE = 120;
    static constexpr int DEFAULT_STUDENT_AGE = 18;
    static constexpr int MIN_TEACHER_AGE = 18;
    static constexpr int MAX_TEACHER_AGE = 120;
    static constexpr int DEFAULT_TEACHER_AGE = 30;
};
