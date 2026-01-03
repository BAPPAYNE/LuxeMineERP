#ifndef USER_H
#define USER_H

#include <QString>
#include <QStringList>

struct User
{
    int id = -1;
    QString username;

    // 🔑 Multiple roles (CSV from DB: "Admin,Seller,Manager")
    QStringList roles;

    bool isActive = false;

    bool isValid() const {
        return id > 0 && isActive;
    }

    bool hasRole(const QString &r) const {
        return roles.contains(r);
    }
};

#endif // USER_H
