#include "SessionManager.h"

// ---------- Internal session state ----------
static User g_user;
static QStringList g_roles;
static QString g_activeRole;

// ---------- User ----------
void SessionManager::setCurrentUser(const User &user)
{
    g_user = user;
}

const User& SessionManager::currentUser()
{
    return g_user;
}

// ---------- Roles ----------
void SessionManager::setRoles(const QStringList &roles)
{
    g_roles = roles;
}

QStringList SessionManager::roles()
{
    return g_roles;
}

void SessionManager::setActiveRole(const QString &role)
{
    // Safety: active role must exist in role list
    if (g_roles.contains(role, Qt::CaseInsensitive)) {
        g_activeRole = role;
    }
}

QString SessionManager::activeRole()
{
    return g_activeRole;
}

bool SessionManager::hasRole(const QString &role)
{
    return g_roles.contains(role, Qt::CaseInsensitive);
}

// ---------- Role helpers ----------
bool SessionManager::isAdmin()
{
    return g_activeRole.compare("Admin", Qt::CaseInsensitive) == 0;
}

bool SessionManager::isSeller()
{
    return g_activeRole.compare("Seller", Qt::CaseInsensitive) == 0;
}

bool SessionManager::isAccountant()
{
    return g_activeRole.compare("Accountant", Qt::CaseInsensitive) == 0;
}

bool SessionManager::isManufacturer()
{
    return g_activeRole.compare("Manufacturer", Qt::CaseInsensitive) == 0;
}

bool SessionManager::isDesigner()
{
    return g_activeRole.compare("Designer", Qt::CaseInsensitive) == 0;
}

// ---------- Session ----------
bool SessionManager::isLoggedIn()
{
    return g_user.isValid();
}

void SessionManager::clear()
{
    g_user = User{};
    g_roles.clear();
    g_activeRole.clear();
}
