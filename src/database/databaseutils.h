#ifndef DATABASEUTILS_H
#define DATABASEUTILS_H

#include "models/OrderData.h"
#include "models/CastingData.h"

class DatabaseUtils
{
public:
    static bool createOrder(const OrderData &o,
                            int &outJobId,
                            int &outSellerSeq);
    static QList<OrderData> getOrdersForSeller(int sellerId);

    static QList<OrderData> getAllOrders();

    static bool getOrderById(int orderId, OrderData &o);

    static bool updateOrder(int orderId, const OrderData &o);

    static QList<QPair<int, QString>> getJobsForCasting();

    static int getCastingIdByJob(int jobId);

    static bool insertCasting(const CastingData &c);

    static bool updateCasting(int castingId, const CastingData &c);

    DatabaseUtils();
};

#endif // DATABASEUTILS_H
