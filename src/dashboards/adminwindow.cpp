#include "AdminWindow.h"
#include "ui_admin.h"

#include "common/SessionManager.h"
#include "admin/UserCreationWidget.h"
#include "admin/ViewUsersWidget.h"
#include "common/switchroledialog.h"
#include "common/rolewindowfactory.h"
#include "seller/orderlistwidget.h"
// #include "seller/orderwidget.h"

#include <QMdiSubWindow>


AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    // Security check
    if (!SessionManager::hasRole("admin")) {
        close();
        return;
    }

    setWindowTitle("Admin Dashboard");

    // 🔗 Menu actions
    connect(ui->actionCreate_Users, &QAction::triggered,
            this, &AdminWindow::openCreateUser);

    connect(ui->actionView_Users, &QAction::triggered,
            this, &AdminWindow::openViewUsers);

    connect(ui->actionOrder_List, &QAction::triggered,
            this, &AdminWindow::openOrderList);

    connect(ui->actionSwitch_Role, &QAction::triggered,
            this, &AdminWindow::changeRole);

}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::openCreateUser()
{
    // Prevent opening multiple same windows (optional but good UX)
    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        if (sub->widget()->objectName() == "UserCreationWidget") {
            sub->setFocus();
            return;
        }
    }

    auto *widget = new UserCreationWidget;
    widget->setObjectName("UserCreationWidget");

    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(widget);
    subWindow->setWindowTitle("Create User");
    subWindow->setAttribute(Qt::WA_DeleteOnClose);

    widget->show();
}


void AdminWindow::openViewUsers()
{
    // 🔁 Prevent opening duplicate View Users windows
    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        if (sub->widget()->objectName() == "ViewUsersWidget") {
            sub->setFocus();
            return;
        }
    }

    auto *widget = new ViewUsersWidget;
    widget->setObjectName("ViewUsersWidget");

    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(widget);
    subWindow->setWindowTitle("View Users");
    subWindow->setAttribute(Qt::WA_DeleteOnClose);

    widget->show();
}

void AdminWindow::changeRole(){
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

void AdminWindow::openOrderList()
{
    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        if (sub->widget()->objectName() == "OrderListWidget") {
            sub->setFocus();
            return;
        }
    }

    auto *widget = new OrderListWidget;
    widget->setObjectName("OrderListWidget");

    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(widget);
    subWindow->setWindowTitle("Orders");
    subWindow->setAttribute(Qt::WA_DeleteOnClose);

    widget->show();
}


