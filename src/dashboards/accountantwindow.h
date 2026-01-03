#ifndef ACCOUNTANTWINDOW_H
#define ACCOUNTANTWINDOW_H

#include <QMainWindow>

namespace Ui {
class AccountantWindow;
}

class AccountantWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountantWindow(QWidget *parent = nullptr);
    ~AccountantWindow();

private slots:
    void changeRole();

    void openOrderList();

private:
    Ui::AccountantWindow *ui;
};

#endif // ACCOUNTANTWINDOW_H
