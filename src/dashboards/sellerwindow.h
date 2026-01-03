#ifndef SELLERWINDOW_H
#define SELLERWINDOW_H

#include <QMainWindow>

namespace Ui {
class SellerWindow;
}

class SellerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SellerWindow(QWidget *parent = nullptr);
    ~SellerWindow();

private slots:
    void changeRole();

    void openOrderEditor(int orderId);

private:
    Ui::SellerWindow *ui;

    void openCreateOrder();
    void openOrderList();
};

#endif // SELLERWINDOW_H
