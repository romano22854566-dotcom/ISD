#pragma once
#include <QDialog>
#include "registry.hpp"

namespace Ui{ class EditStudentDialog; }

class EditStudentDialog: public QDialog {
    Q_OBJECT
public:
    EditStudentDialog(isd::Registry& reg,isd::Id studentId,QWidget* parent = nullptr);
    ~EditStudentDialog() override;

private slots:
    void onSave();

private:
    Ui::EditStudentDialog* ui;
    isd::Registry& reg;
    isd::Id studentId;
    void loadData();
};
