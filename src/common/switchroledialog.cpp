#include "SwitchRoleDialog.h"
#include "SessionManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

SwitchRoleDialog::SwitchRoleDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Switch Role");
    setModal(true);
    resize(300, 120);

    // ---------- Role Combo ----------
    roleCombo = new QComboBox(this);
    roleCombo->addItems(SessionManager::roles());
    roleCombo->setCurrentText(SessionManager::activeRole());

    // ---------- Buttons ----------
    auto *btnSwitch = new QPushButton("Switch");
    auto *btnCancel = new QPushButton("Cancel");

    connect(btnSwitch, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    // ---------- Layout ----------
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Select Role:"));
    mainLayout->addWidget(roleCombo);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnSwitch);
    btnLayout->addWidget(btnCancel);

    mainLayout->addLayout(btnLayout);
}

QString SwitchRoleDialog::selectedRole() const
{
    return roleCombo->currentText();
}
