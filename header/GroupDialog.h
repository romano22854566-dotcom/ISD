#pragma once
#include <QDialog>
#include "registry.hpp"

namespace Ui{ class GroupDialog; }

class GroupDialog: public QDialog {
    Q_OBJECT
public:
    GroupDialog(isd::Registry& reg,int groupId,QWidget* parent = nullptr);
    ~GroupDialog() override;

private slots:
    void onAddSubject();
    void onRemoveSubject();
    void onAddSpecialty();
    void onAddSpecSubject();
    void onRemoveSpecItem();

private:
    Ui::GroupDialog* ui;
    isd::Registry& reg;
    int groupId;
    void loadData();
    void loadGroupView();
    void loadSpecialtyView();
};
