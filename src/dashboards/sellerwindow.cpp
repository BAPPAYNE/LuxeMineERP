#include "sellerwindow.h"
#include "ui_seller.h"

#include <QMdiSubWindow>
#include "seller/orderwidget.h"
#include "seller/orderlistwidget.h"
#include "common/switchroledialog.h"
#include "common/rolewindowfactory.h"
#include "common/sessionmanager.h"

SellerWindow::SellerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SellerWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew_Order, &QAction::triggered,
            this, &SellerWindow::openCreateOrder);
    connect(ui->actionOrder_List, &QAction::triggered,
            this, &SellerWindow::openOrderList);

    connect(ui->actionSwitch_Role, &QAction::triggered,
            this, &SellerWindow::changeRole);

}

SellerWindow::~SellerWindow()
{
    delete ui;
}

void SellerWindow::openCreateOrder()
{
    // Prevent opening multiple same windows (optional but good UX)
    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        if (sub->widget()->objectName() == "OrderCreationWidget") {
            sub->setFocus();
            return;
        }
    }

    auto *widget = new OrderWidget;
    widget->setObjectName("OrderCreationWidget");

    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(widget);
    subWindow->setWindowTitle("Create Order");
    subWindow->setAttribute(Qt::WA_DeleteOnClose);

    widget->show();
}

void SellerWindow::openOrderList()
{
    // 1️⃣ Prevent opening multiple Order List windows
    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        if (sub->widget() &&
            sub->widget()->objectName() == "OrderListWidget") {
            ui->mdiArea->setActiveSubWindow(sub);
            return;
        }
    }

    // 2️⃣ Create OrderListWidget
    auto *widget = new OrderListWidget;
    widget->setObjectName("OrderListWidget");

    // 3️⃣ Add to MDI
    auto *sub = ui->mdiArea->addSubWindow(widget);
    sub->setWindowTitle("My Orders");
    sub->setAttribute(Qt::WA_DeleteOnClose);

    sub->show();   // ✅ correct

    // 4️⃣ Connect signal
    connect(widget, &OrderListWidget::requestOpenOrder,
            this, &SellerWindow::openOrderEditor);
}


void SellerWindow::openOrderEditor(int orderId)
{
    const QString editorName = QString("OrderEditor_%1").arg(orderId);

    // 1️⃣ Prevent opening same order twice
    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        if (sub->widget() &&
            sub->widget()->objectName() == editorName) {
            ui->mdiArea->setActiveSubWindow(sub);
            return;
        }
    }

    // 2️⃣ Create editor
    auto *widget = new OrderWidget(orderId);
    widget->setObjectName(editorName);

    // 3️⃣ Add to MDI
    auto *sub = ui->mdiArea->addSubWindow(widget);
    sub->setWindowTitle(QString("Edit Order #%1").arg(orderId));
    sub->setAttribute(Qt::WA_DeleteOnClose);

    sub->show();   // ✅ correct
}



void SellerWindow::changeRole(){
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
