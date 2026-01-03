#ifndef ROLEWINDOWFACTORY_H
#define ROLEWINDOWFACTORY_H

#include <QWidget>
#include <QString>

class RoleWindowFactory
{
public:
    static QWidget* create(const QString &role);
};

#endif // ROLEWINDOWFACTORY_H
