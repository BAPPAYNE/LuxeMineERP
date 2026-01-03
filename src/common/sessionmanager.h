#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "models/User.h"
#include <QStringList>

class SessionManager
{
public:
    // -------- User --------
    static void setCurrentUser(const User &user);
    static const User& currentUser();

    // -------- Roles --------
    static void setRoles(const QStringList &roles);
    static QStringList roles();

    static void setActiveRole(const QString &role);
    static QString activeRole();

    static bool hasRole(const QString &role);

    // -------- Role helpers --------
    static bool isAdmin();
    static bool isSeller();
    static bool isAccountant();
    static bool isManufacturer();
    static bool isDesigner();

    // -------- Session --------
    static bool isLoggedIn();
    static void clear();
};

#endif // SESSIONMANAGER_H
