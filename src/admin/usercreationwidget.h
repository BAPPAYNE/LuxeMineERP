#ifndef USERCREATIONWIDGET_H
#define USERCREATIONWIDGET_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class UserCreationWidget;
}

class UserCreationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserCreationWidget(QWidget *parent = nullptr);
    ~UserCreationWidget();

private slots:
    void onRolesSelectionChanged();
    void onCreateUserClicked();

private:
    bool isSellerSelected() const;
    void resetForm();

private:
    Ui::UserCreationWidget *ui;
};

#endif // USERCREATIONWIDGET_H
