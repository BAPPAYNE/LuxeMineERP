#include "orderlistwidget.h"
#include "ui_orderlist.h"

#include "DatabaseUtils.h"
#include "SessionManager.h"

#include <QPushButton>
#include <QMessageBox>

OrderListWidget::OrderListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OrderListWidget)
{
    ui->setupUi(this);

    setupTable();
    loadOrders();

}

OrderListWidget::~OrderListWidget()
{
    delete ui;
}

void OrderListWidget::setupTable()
{
    QStringList headers = {
        "Order No", //done 0-0
        "Job No", //done 1-1
        "Party Name", //done 2-2
        "Product Pis", //done new-3
        "Metal", //extra new-4
        "Purity", //done new-5
        "Design No.", //done new-6
        "Status", //done 3-7
        "Order Date",  //done 4-8
        "Delivery Date", //done 5-9
        "Remark", //done new-10
        "Action" //extra 6-11
    };

    ui->ordersTableWidget->setColumnCount(headers.size());
    ui->ordersTableWidget->setHorizontalHeaderLabels(headers);
    ui->ordersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->ordersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void OrderListWidget::loadOrders()
{
    ui->ordersTableWidget->setRowCount(0);

    QList<OrderData> orders;

    if (SessionManager::isSeller()) {
        orders = DatabaseUtils::getOrdersForSeller(
            SessionManager::currentUser().id
            );
    } else if (SessionManager::isAdmin()) {
        orders = DatabaseUtils::getAllOrders();
    }

    ui->ordersTableWidget->setRowCount(orders.size());

    for (int i = 0; i < orders.size(); ++i) {
        addRow(i, orders[i]);
    }
}

void OrderListWidget::addRow(int row, const OrderData &order)
{
    auto *table = ui->ordersTableWidget;
    // qDebug() << order.orderId << "order added!" << order.jobId;
    // table->setItem(row, 0, new QTableWidgetItem(QString::number(order.sellerOrderSeq)));
    // QString sellerName = order.sellerName;
    // int sellerOrderSeq = order.sellerOrderSeq;
    QString orderNo = order.sellerName
                      + QString("%1").arg(order.sellerOrderSeq, 5, 10, QChar('0'));

    table->setItem(row, 0, new QTableWidgetItem(orderNo));

    QString jobNo = "JOB" + QString("%1").arg(order.jobId, 7, 10, QChar('0'));

    table->setItem(row, 1, new QTableWidgetItem(jobNo));

    // Column 2: Party Name
    table->setItem(row, 2,
                   new QTableWidgetItem(order.partyName));

    table->setItem(row, 3, new QTableWidgetItem(QString::number(order.productPis)));
    // qDebug() << "product pis" << order.productPis;

    table->setItem(row, 4, new QTableWidgetItem(order.metalName));

    table->setItem(row, 5, new QTableWidgetItem(order.metalPurity));

    table->setItem(row, 6, new QTableWidgetItem(order.designNo));

    // Column 7: Status (temporary placeholder)
    table->setItem(row, 7,
                   new QTableWidgetItem("Pending"));

    // Column 8: Order Date
    table->setItem(row, 8,
                   new QTableWidgetItem(order.orderDate));

    // Column 9: Delivery Date
    table->setItem(row, 9,
                   new QTableWidgetItem(order.deliveryDate));
    // qDebug() << order.deliveryDate;

    table->setItem(row, 10, new QTableWidgetItem(order.extraDetail));

    // Column 11: Action buttons
    QWidget *actionWidget = new QWidget(table);
    QHBoxLayout *layout = new QHBoxLayout(actionWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // Seller → Edit (if allowed)
    if (SessionManager::isSeller() && order.isEditable()) {
        QPushButton *editBtn = new QPushButton("Edit", actionWidget);
        connect(editBtn, &QPushButton::clicked, this, [this, order]() {
            onEditClicked(order.orderId);
        });
        layout->addWidget(editBtn);
    }

    // // Admin → Delete
    if (SessionManager::isAdmin()) {
        QPushButton *delBtn = new QPushButton("Delete", actionWidget);
        connect(delBtn, &QPushButton::clicked, this, [this, order]() {
            onDeleteClicked(order.orderId);
        });
        layout->addWidget(delBtn);
    }

    layout->addStretch();
    table->setCellWidget(row, 11, actionWidget);
}

void OrderListWidget::onEditClicked(int orderId)
{
    // We’ll implement OrderFormWidget next
    emit requestOpenOrder(orderId);
}

void OrderListWidget::onDeleteClicked(int orderId)
{
    // if (QMessageBox::question(this,
    //                           "Delete Order",
    //                           "Are you sure? This cannot be undone.") != QMessageBox::Yes)
    //     return;

    // // DatabaseUtils::deleteOrder(orderId);
    // loadOrders();
}
