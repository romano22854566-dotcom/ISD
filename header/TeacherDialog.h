#pragma once
#include <QDialog>
#include "registry.hpp"

namespace Ui{ class TeacherDialog; }

class TeacherDialog: public QDialog {
    Q_OBJECT
public:
    TeacherDialog(isd::Registry& reg,int teacherId,QWidget* parent = nullptr);
    ~TeacherDialog();

private slots:
    void onAddSubject();
    void onRemoveSubject();

private:
    Ui::TeacherDialog* ui;
    isd::Registry& reg;
    int teacherId;
    void loadData();
};
