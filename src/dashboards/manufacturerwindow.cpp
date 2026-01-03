#include "manufacturerwindow.h"
#include "ui_manufacturer.h"

#include "common/switchroledialog.h"
#include "common/rolewindowfactory.h"
#include "common/sessionmanager.h"

ManufacturerWindow::ManufacturerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManufacturerWindow)
{
    ui->setupUi(this);

    connect(ui->actionSwitch_Role, &QAction::triggered,
            this, &ManufacturerWindow::changeRole);
}

ManufacturerWindow::~ManufacturerWindow()
{
    delete ui;
}

void ManufacturerWindow::changeRole(){
    // 1️⃣ Open role selection dialog
    SwitchRoleDialog dlg(this);

    if (dlg.exec() != QDialog::Accepted)
        return;

    // 2️⃣ Get selected role
    QString newRole = dlg.selectedRole();

    // 3️⃣ Ignore if same role selected
    if (newRole == SessionManager::activeRole())
        return;

    // 4️⃣ Update session
    SessionManager::setActiveRole(newRole);

    // 5️⃣ Open new role window
    QWidget *nextWindow = RoleWindowFactory::create(newRole);
    if (!nextWindow)
        return;

    nextWindow->show();

    // 6️⃣ Close current window
    this->close();
}
