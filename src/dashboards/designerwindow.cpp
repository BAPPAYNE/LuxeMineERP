#include "designerwindow.h"
#include "ui_designer.h"

#include "common/switchroledialog.h"
#include "common/rolewindowfactory.h"
#include "common/sessionmanager.h"

DesignerWindow::DesignerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DesignerWindow)
{
    ui->setupUi(this);

    connect(ui->actionSwitch_Role, &QAction::triggered,
            this, &DesignerWindow::changeRole);
}

DesignerWindow::~DesignerWindow()
{
    delete ui;
}

void DesignerWindow::changeRole(){
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
