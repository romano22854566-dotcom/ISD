#pragma once
#include <QDialog>
#include "registry.hpp"

namespace Ui{ class StudentDialog; }

class StudentDialog: public QDialog {
    Q_OBJECT
public:
    StudentDialog(isd::Registry& reg,isd::Id studentId,QWidget* parent = nullptr);
    ~StudentDialog() override;

private slots:
    void onSave();

private:
    Ui::StudentDialog* ui;
    isd::Registry& reg;
    isd::Id studentId;
    void loadData();

    static QString joinGrades(const std::vector<int>& v);
};
