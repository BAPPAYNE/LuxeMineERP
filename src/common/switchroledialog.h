#ifndef SWITCHROLEDIALOG_H
#define SWITCHROLEDIALOG_H

#include <QDialog>
#include <QComboBox>

class SwitchRoleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SwitchRoleDialog(QWidget *parent = nullptr);

    // Returns selected role after dialog accepted
    QString selectedRole() const;

private:
    QComboBox *roleCombo;
};

#endif // SWITCHROLEDIALOG_H
