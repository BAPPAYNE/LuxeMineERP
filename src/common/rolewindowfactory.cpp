#include "RoleWindowFactory.h"
#include "common/SessionManager.h"

// Include role-based windows
#include "dashboards/adminwindow.h"
#include "dashboards/accountantwindow.h"
#include "dashboards/designerwindow.h"
#include "dashboards/manufacturerwindow.h"
#include "dashboards/sellerwindow.h"

QWidget* RoleWindowFactory::create(const QString &role)
{
    // Safety check
    if (!SessionManager::hasRole(role))
        return nullptr;

    // Create window based on role
    if (role.compare("Admin", Qt::CaseInsensitive) == 0)
        return new AdminWindow;

    if (role.compare("Seller", Qt::CaseInsensitive) == 0)
        return new SellerWindow;

    if (role.compare("Manufacturer", Qt::CaseInsensitive) == 0)
        return new ManufacturerWindow;

    if (role.compare("Accountant", Qt::CaseInsensitive) == 0)
        return new AccountantWindow;

    if (role.compare("Designer", Qt::CaseInsensitive) == 0)
        return new DesignerWindow;

    return nullptr;
}
