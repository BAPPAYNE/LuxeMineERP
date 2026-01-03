#include "DatabaseUtils.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>

bool DatabaseUtils::createOrder(const OrderData &o,
                                int &outJobId,
                                int &outSellerSeq)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    if (!db.transaction()) {
        qCritical() << "Transaction start failed";
        return false;
    }

    // ---------- 1️⃣ jobs ----------
    if (!q.exec("INSERT INTO jobs DEFAULT VALUES")) {
        qCritical() << q.lastError() << "11111";
        db.rollback();
        return false;
    }
    int jobId = q.lastInsertId().toInt();

    // ---------- 2️⃣ seller_order_counter ----------
    // 1️⃣ Ensure seller counter row exists (SAFE, no race condition)
    q.prepare(R"(
    INSERT OR IGNORE INTO seller_order_counter (seller_id, last_order_no)
    VALUES (:sid, 0)
)");
    q.bindValue(":sid", o.sellerId);

    if (!q.exec()) {
        qCritical() << "Insert seller counter failed:" << q.lastError();
        db.rollback();
        return false;
    }

    // 2️⃣ Increment counter
    q.prepare(R"(
    UPDATE seller_order_counter
    SET last_order_no = last_order_no + 1
    WHERE seller_id = :sid
)");
    q.bindValue(":sid", o.sellerId);

    if (!q.exec() || q.numRowsAffected() == 0) {
        qCritical() << "Update seller counter failed:" << q.lastError();
        db.rollback();
        return false;
    }

    // 3️⃣ Fetch updated value
    q.prepare(R"(
        SELECT last_order_no
        FROM seller_order_counter
        WHERE seller_id = :sid
    )");
    q.bindValue(":sid", o.sellerId);

    if (!q.exec() || !q.next()) {
        qCritical() << "Select seller counter failed:" << q.lastError();
        db.rollback();
        return false;
    }

    int sellerSeq = q.value(0).toInt();

    // ---------- 3️⃣ orders ----------
    q.prepare(R"(
        INSERT INTO orders (job_id, seller_id, seller_order_seq)
        VALUES (:job_id, :seller_id, :seq)
    )");
    q.bindValue(":job_id", jobId);
    q.bindValue(":seller_id", o.sellerId);
    q.bindValue(":seq", sellerSeq);

    if (!q.exec()) {
        qCritical() << q.lastError();
        db.rollback();
        return false;
    }

    int orderId = q.lastInsertId().toInt();

    // ---------- 4️⃣ order_book_detail ----------
    q.prepare(R"(
        INSERT INTO order_book_detail (
            order_id, job_id,
            sellerName, sellerId,
            partyId, partyName,
            clientId, agencyId, shopId, reteillerId, starId,
            address, city, state, country,
            orderDate, deliveryDate,
            productName, productPis,
            approxProductWt, approxDiamondWt,
            metalPrice, metalName, metalPurity, metalColor,
            sizeNo, sizeMM, length, width, height,
            diaPacific, diaPurity, diaColor, diaPrice,
            stPacific, stPurity, stColor, stPrice,
            designNo, image1Path, image2Path,
            metalCertiName, metalCertiType,
            diaCertiName, diaCertiType,
            pesSaki, chainLock, polish, settingLebour, metalStemp,
            paymentMethod, totalAmount, advance, remaining,
            note, extraDetail, isSaved
        ) VALUES (
            :order_id, :job_id,
            :sellerName, :sellerId,
            :partyId, :partyName,
            :clientId, :agencyId, :shopId, :reteillerId, :starId,
            :address, :city, :state, :country,
            :orderDate, :deliveryDate,
            :productName, :productPis,
            :approxProductWt, :approxDiamondWt,
            :metalPrice, :metalName, :metalPurity, :metalColor,
            :sizeNo, :sizeMM, :length, :width, :height,
            :diaPacific, :diaPurity, :diaColor, :diaPrice,
            :stPacific, :stPurity, :stColor, :stPrice,
            :designNo, :image1Path, :image2Path,
            :metalCertiName, :metalCertiType,
            :diaCertiName, :diaCertiType,
            :pesSaki, :chainLock, :polish, :settingLebour, :metalStemp,
            :paymentMethod, :totalAmount, :advance, :remaining,
            :note, :extraDetail, 1
        )
    )");

    // 🔁 Bind everything EXACTLY as before

    // -------- IDs (NEW SYSTEM) --------
    q.bindValue(":order_id", orderId);
    q.bindValue(":job_id", jobId);

    // Seller & party
    q.bindValue(":sellerName", o.sellerName);
    q.bindValue(":sellerId", o.sellerId);
    q.bindValue(":partyId", o.partyId);
    q.bindValue(":partyName", o.partyName);

    // Client hierarchy
    q.bindValue(":clientId", o.clientId);
    q.bindValue(":agencyId", o.agencyId);
    q.bindValue(":shopId", o.shopId);
    q.bindValue(":retaillerId", o.retaillerId);
    q.bindValue(":starId", o.starId);

    // Address
    q.bindValue(":address", o.address);
    q.bindValue(":city", o.city);
    q.bindValue(":state", o.state);
    q.bindValue(":country", o.country);

    // Dates
    q.bindValue(":orderDate", o.orderDate);
    q.bindValue(":deliveryDate", o.deliveryDate);

    // Product
    q.bindValue(":productName", o.productName);
    q.bindValue(":productPis", o.productPis);
    q.bindValue(":approxProductWt", o.approxProductWt);
    q.bindValue(":approxDiamondWt", o.approxDiamondWt);

    // Metal
    q.bindValue(":metalPrice", o.metalPrice);
    q.bindValue(":metalName", o.metalName);
    q.bindValue(":metalPurity", o.metalPurity);
    q.bindValue(":metalColor", o.metalColor);

    // Size
    q.bindValue(":sizeNo", o.sizeNo);
    q.bindValue(":sizeMM", o.sizeMM);
    q.bindValue(":length", o.length);
    q.bindValue(":width", o.width);
    q.bindValue(":height", o.height);

    // Diamond
    q.bindValue(":diaPacific", o.diaPacific);
    q.bindValue(":diaPurity", o.diaPurity);
    q.bindValue(":diaColor", o.diaColor);
    q.bindValue(":diaPrice", o.diaPrice);

    // Stone
    q.bindValue(":stPacific", o.stPacific);
    q.bindValue(":stPurity", o.stPurity);
    q.bindValue(":stColor", o.stColor);
    q.bindValue(":stPrice", o.stPrice);

    // Design & images
    q.bindValue(":designNo", o.designNo);
    // q.bindValue(":designNo2", o.designNo2);
    q.bindValue(":image1Path", o.image1Path);
    q.bindValue(":image2Path", o.image2Path);

    // Certificates
    q.bindValue(":metalCertiName", o.metalCertiName);
    q.bindValue(":metalCertiType", o.metalCertiType);
    q.bindValue(":diaCertiName", o.diaCertiName);
    q.bindValue(":diaCertiType", o.diaCertiType);

    // Manufacturing
    q.bindValue(":pesSaki", o.pesSaki);
    q.bindValue(":chainLock", o.chainLock);
    q.bindValue(":polish", o.polish);
    q.bindValue(":settingLebour", o.settingLebour);
    q.bindValue(":metalStemp", o.metalStemp);

    // Payment
    q.bindValue(":paymentMethod", o.paymentMethod);
    q.bindValue(":totalAmount", o.totalAmount);
    q.bindValue(":advance", o.advance);
    q.bindValue(":remaining", o.remaining);

    // Notes
    q.bindValue(":note", o.note);
    q.bindValue(":extraDetail", o.extraDetail);

    q.bindValue(":isSaved", o.isSaved);

    if (!q.exec()) {
        qCritical() << "order_book_detail insert error:" << q.lastError();
        db.rollback();
        return false;
    }

    // ---------- 5️⃣ order_status ----------
    q.prepare("INSERT INTO order_status (job_id) VALUES (:job_id)");
    q.bindValue(":job_id", jobId);

    if (!q.exec()) {
        qCritical() << q.lastError();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qCritical() << "Commit failed";
        return false;
    }

    outJobId = jobId;
    outSellerSeq = sellerSeq;
    return true;
}

QList<OrderData> DatabaseUtils::getOrdersForSeller(int sellerId)
{
    QList<OrderData> list;
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    q.prepare(R"(
        SELECT
            o.order_id,
            o.job_id,
            o.seller_order_seq,
            od.partyName,
            od.sellerName,
            od.productPis,
            od.metalName,
            od.metalPurity,
            od.designNo,
            od.orderDate,
            od.deliveryDate,
            od.extraDetail
        FROM orders o
        JOIN order_book_detail od
            ON o.order_id = od.order_id
        WHERE o.seller_id = :sid
        ORDER BY o.order_id DESC
    )");

    q.bindValue(":sid", sellerId);

    if (!q.exec()) {
        qCritical() << "getOrdersForSeller failed:" << q.lastError();
        return list;
    }

    while (q.next()) {
        OrderData o;

        o.orderId             = q.value("order_id").toInt();
        o.sellerOrderSeq      = q.value("seller_order_seq").toInt();
        o.jobId               = q.value("job_id").toInt();
        o.partyName           = q.value("partyName").toString();
        o.sellerName          = q.value("sellerName").toString();
        o.productPis          = q.value("productPis").toInt();
        o.metalName           = q.value("metalName").toString();
        o.metalPurity         = q.value("metalPurity").toString();
        o.designNo            = q.value("designNo").toString();
        o.orderDate           = q.value("orderDate").toString();
        o.deliveryDate        = q.value("deliveryDate").toString();
        o.extraDetail         = q.value("extraDetail").toString();

        list.append(o);
    }

    return list;
}

QList<OrderData> DatabaseUtils::getAllOrders()
{
    QList<OrderData> list;
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    q.prepare(R"(
        SELECT
            o.order_id,
            o.job_id,
            o.seller_order_seq,
            od.partyName,
            od.sellerName,
            od.orderDate,
            od.deliveryDate
        FROM orders o
        JOIN order_book_detail od
            ON o.order_id = od.order_id
        ORDER BY o.order_id DESC
    )");

    if (!q.exec()) {
        qCritical() << "getAllOrders failed:" << q.lastError();
        return list;
    }

    while (q.next()) {
        OrderData o;
\
        o.orderId             = q.value("order_id").toInt();
        o.sellerOrderSeq      = q.value("seller_order_seq").toInt();
        o.jobId               = q.value("job_id").toInt();
        o.partyName           = q.value("partyName").toString();
        o.sellerName          = q.value("sellerName").toString();
        o.orderDate           = q.value("orderDate").toString();
        o.deliveryDate        = q.value("deliveryDate").toString();

        list.append(o);
    }

    return list;
}

bool DatabaseUtils::getOrderById(int orderId, OrderData &o)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    q.prepare(R"(
        SELECT
            o.order_id,
            o.job_id,
            o.seller_order_seq,
            od.sellerName, od.sellerId,
            od.partyId, od.partyName,
            od.clientId, od.agencyId, od.shopId, od.reteillerId, od.starId,
            od.address, od.city, od.state, od.country,
            od.orderDate, od.deliveryDate,
            od.productName, od.productPis,
            od.approxProductWt, od.approxDiamondWt,
            od.metalPrice, od.metalName, od.metalPurity, od.metalColor,
            od.sizeNo, od.sizeMM, od.length, od.width, od.height,
            od.diaPacific, od.diaPurity, od.diaColor, od.diaPrice,
            od.stPacific, od.stPurity, od.stColor, od.stPrice,
            od.designNo, od.image1Path, od.image2Path,
            od.metalCertiName, od.metalCertiType,
            od.diaCertiName, od.diaCertiType,
            od.pesSaki, od.chainLock, od.polish, od.settingLebour, od.metalStemp,
            od.paymentMethod, od.totalAmount, od.advance, od.remaining,
            od.note, od.extraDetail
        FROM orders o
        JOIN order_book_detail od
            ON o.order_id = od.order_id
        WHERE o.order_id = :id
    )");

    q.bindValue(":id", orderId);

    if (!q.exec() || !q.next()) {
        qCritical() << "Failed to fetch order:" << q.lastError();
        return false;
    }

    // Primary
    o.id = orderId;
    o.orderId = q.value("order_id").toInt();
    o.jobId = q.value("job_id").toInt();
    o.sellerOrderSeq = q.value("seller_order_seq").toInt();

    // Seller & Party
    o.sellerName = q.value("sellerName").toString();
    o.sellerId = q.value("sellerId").toInt();
    o.partyId = q.value("partyId").toString();
    o.partyName = q.value("partyName").toString();

    // Client hierarchy
    o.clientId = q.value("clientId").toString();
    o.agencyId = q.value("agencyId").toString();
    o.shopId = q.value("shopId").toString();
    o.retaillerId = q.value("reteillerId").toString();
    o.starId = q.value("starId").toString();

    // Address
    o.address = q.value("address").toString();
    o.city = q.value("city").toString();
    o.state = q.value("state").toString();
    o.country = q.value("country").toString();

    // Dates
    o.orderDate = q.value("orderDate").toString();
    o.deliveryDate = q.value("deliveryDate").toString();

    // Product
    o.productName = q.value("productName").toString();
    o.productPis = q.value("productPis").toInt();
    o.approxProductWt = q.value("approxProductWt").toDouble();
    o.approxDiamondWt = q.value("approxDiamondWt").toDouble();

    // Metal
    o.metalPrice = q.value("metalPrice").toDouble();
    o.metalName = q.value("metalName").toString();
    o.metalPurity = q.value("metalPurity").toString();
    o.metalColor = q.value("metalColor").toString();

    // Size & dimensions
    o.sizeNo = q.value("sizeNo").toDouble();
    o.sizeMM = q.value("sizeMM").toDouble();
    o.length = q.value("length").toDouble();
    o.width = q.value("width").toDouble();
    o.height = q.value("height").toDouble();

    // Diamond
    o.diaPacific = q.value("diaPacific").toString();
    o.diaPurity = q.value("diaPurity").toString();
    o.diaColor = q.value("diaColor").toString();
    o.diaPrice = q.value("diaPrice").toDouble();

    // Stone
    o.stPacific = q.value("stPacific").toString();
    o.stPurity = q.value("stPurity").toString();
    o.stColor = q.value("stColor").toString();
    o.stPrice = q.value("stPrice").toDouble();

    // Design
    o.designNo = q.value("designNo").toString();
    o.image1Path = q.value("image1Path").toString();
    o.image2Path = q.value("image2Path").toString();

    // Certification
    o.metalCertiName = q.value("metalCertiName").toString();
    o.metalCertiType = q.value("metalCertiType").toString();
    o.diaCertiName = q.value("diaCertiName").toString();
    o.diaCertiType = q.value("diaCertiType").toString();

    // Manufacturing options
    o.pesSaki = q.value("pesSaki").toString();
    o.chainLock = q.value("chainLock").toString();
    o.polish = q.value("polish").toString();
    o.settingLebour = q.value("settingLebour").toString();
    o.metalStemp = q.value("metalStemp").toString();

    // Payment
    o.paymentMethod = q.value("paymentMethod").toString();
    o.totalAmount = q.value("totalAmount").toDouble();
    o.advance = q.value("advance").toDouble();
    o.remaining = q.value("remaining").toDouble();

    // Notes
    o.note = q.value("note").toString();
    o.extraDetail = q.value("extraDetail").toString();

    o.isSaved = 1;
    return true;
}

bool DatabaseUtils::updateOrder(int orderId, const OrderData &o)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    q.prepare(R"(
        UPDATE order_book_detail SET

            sellerName = :sellerName,
            sellerId   = :sellerId,

            partyId   = :partyId,
            partyName = :partyName,

            clientId    = :clientId,
            agencyId    = :agencyId,
            shopId      = :shopId,
            reteillerId = :reteillerId,
            starId      = :starId,

            address = :address,
            city    = :city,
            state   = :state,
            country = :country,

            orderDate    = :orderDate,
            deliveryDate = :deliveryDate,

            productName     = :productName,
            productPis      = :productPis,
            approxProductWt = :approxProductWt,
            approxDiamondWt = :approxDiamondWt,

            metalPrice  = :metalPrice,
            metalName   = :metalName,
            metalPurity = :metalPurity,
            metalColor  = :metalColor,

            sizeNo = :sizeNo,
            sizeMM = :sizeMM,
            length = :length,
            width  = :width,
            height = :height,

            diaPacific = :diaPacific,
            diaPurity  = :diaPurity,
            diaColor   = :diaColor,
            diaPrice   = :diaPrice,

            stPacific = :stPacific,
            stPurity  = :stPurity,
            stColor   = :stColor,
            stPrice   = :stPrice,

            designNo   = :designNo,
            image1Path = :image1Path,
            image2Path = :image2Path,

            metalCertiName = :metalCertiName,
            metalCertiType = :metalCertiType,
            diaCertiName   = :diaCertiName,
            diaCertiType   = :diaCertiType,

            pesSaki       = :pesSaki,
            chainLock     = :chainLock,
            polish        = :polish,
            settingLebour = :settingLebour,
            metalStemp    = :metalStemp,

            paymentMethod = :paymentMethod,
            totalAmount   = :totalAmount,
            advance       = :advance,
            remaining     = :remaining,

            note        = :note,
            extraDetail = :extraDetail,
            isSaved     = :isSaved

        WHERE id = :order_id
    )");

    // -------- IDs (NEW SYSTEM) --------
    // q.bindValue(":order_id", orderId);
    // q.bindValue(":job_id", jobId);

    // Seller & party
    q.bindValue(":sellerName", o.sellerName);
    q.bindValue(":sellerId", o.sellerId);
    q.bindValue(":partyId", o.partyId);
    q.bindValue(":partyName", o.partyName);

    // Client hierarchy
    q.bindValue(":clientId", o.clientId);
    q.bindValue(":agencyId", o.agencyId);
    q.bindValue(":shopId", o.shopId);
    q.bindValue(":retaillerId", o.retaillerId);
    q.bindValue(":starId", o.starId);

    // Address
    q.bindValue(":address", o.address);
    q.bindValue(":city", o.city);
    q.bindValue(":state", o.state);
    q.bindValue(":country", o.country);

    // Dates
    q.bindValue(":orderDate", o.orderDate);
    q.bindValue(":deliveryDate", o.deliveryDate);

    // Product
    q.bindValue(":productName", o.productName);
    q.bindValue(":productPis", o.productPis);
    q.bindValue(":approxProductWt", o.approxProductWt);
    q.bindValue(":approxDiamondWt", o.approxDiamondWt);

    // Metal
    q.bindValue(":metalPrice", o.metalPrice);
    q.bindValue(":metalName", o.metalName);
    q.bindValue(":metalPurity", o.metalPurity);
    q.bindValue(":metalColor", o.metalColor);

    // Size
    q.bindValue(":sizeNo", o.sizeNo);
    q.bindValue(":sizeMM", o.sizeMM);
    q.bindValue(":length", o.length);
    q.bindValue(":width", o.width);
    q.bindValue(":height", o.height);

    // Diamond
    q.bindValue(":diaPacific", o.diaPacific);
    q.bindValue(":diaPurity", o.diaPurity);
    q.bindValue(":diaColor", o.diaColor);
    q.bindValue(":diaPrice", o.diaPrice);

    // Stone
    q.bindValue(":stPacific", o.stPacific);
    q.bindValue(":stPurity", o.stPurity);
    q.bindValue(":stColor", o.stColor);
    q.bindValue(":stPrice", o.stPrice);

    // Design & images
    q.bindValue(":designNo", o.designNo);
    // q.bindValue(":designNo2", o.designNo2);
    q.bindValue(":image1Path", o.image1Path);
    q.bindValue(":image2Path", o.image2Path);

    // Certificates
    q.bindValue(":metalCertiName", o.metalCertiName);
    q.bindValue(":metalCertiType", o.metalCertiType);
    q.bindValue(":diaCertiName", o.diaCertiName);
    q.bindValue(":diaCertiType", o.diaCertiType);

    // Manufacturing
    q.bindValue(":pesSaki", o.pesSaki);
    q.bindValue(":chainLock", o.chainLock);
    q.bindValue(":polish", o.polish);
    q.bindValue(":settingLebour", o.settingLebour);
    q.bindValue(":metalStemp", o.metalStemp);

    // Payment
    q.bindValue(":paymentMethod", o.paymentMethod);
    q.bindValue(":totalAmount", o.totalAmount);
    q.bindValue(":advance", o.advance);
    q.bindValue(":remaining", o.remaining);

    // Notes
    q.bindValue(":note", o.note);
    q.bindValue(":extraDetail", o.extraDetail);

    q.bindValue(":isSaved", o.isSaved);

    // WHERE clause
    q.bindValue(":order_id", orderId);


    if (!q.exec()) {
        qCritical() << "Update failed:" << q.lastError();
        return false;
    }

    return true;
}

QList<QPair<int, QString>> DatabaseUtils::getJobsForCasting()
{
    QList<QPair<int, QString>> list;

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    q.prepare(R"(
        SELECT job_id, deliveryDate
        FROM order_book_detail
        ORDER BY deliveryDate ASC
    )");

    if (!q.exec()) {
        qCritical() << "Failed to fetch jobs for casting:" << q.lastError();
        return list;
    }

    while (q.next()) {
        int jobId = q.value(0).toInt();
        QString deliveryDate = q.value(1).toString();
        list.append({jobId, deliveryDate});
    }

    return list;
}

int DatabaseUtils::getCastingIdByJob(int jobId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("SELECT id FROM casting_entry WHERE job_id = :job");
    q.bindValue(":job", jobId);

    if (!q.exec())
        return 0;

    if (q.next())
        return q.value(0).toInt();

    return 0;
}

bool DatabaseUtils::insertCasting(const CastingData &c)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    if (!db.transaction())
        return false;

    q.prepare(R"(
        INSERT INTO casting_entry (
            job_id, casting_date, casting_name, pcs,
            issue_metal_name, issue_metal_purity, issue_metal_wt,
            issue_diamond_pcs, issue_diamond_wt,
            receive_runner_wt, receive_product_wt,
            receive_diamond_pcs, receive_diamond_wt,
            accountant_id, status
        ) VALUES (
            :job, :date, :name, :pcs,
            :metal, :purity, :metalWt,
            :diaPcs, :diaWt,
            :runner, :product,
            :recvDiaPcs, :recvDiaWt,
            :acc, :status
        )
    )");

    q.bindValue(":job", c.jobId);
    q.bindValue(":date", c.castingDate);
    q.bindValue(":name", c.castingName);
    q.bindValue(":pcs", c.pcs);

    q.bindValue(":metal", c.issueMetalName);
    q.bindValue(":purity", c.issueMetalPurity);
    q.bindValue(":metalWt", c.issueMetalWt);
    q.bindValue(":diaPcs", c.issueDiamondPcs);
    q.bindValue(":diaWt", c.issueDiamondWt);

    q.bindValue(":runner", c.receiveRunnerWt);
    q.bindValue(":product", c.receiveProductWt);
    q.bindValue(":recvDiaPcs", c.receiveDiamondPcs);
    q.bindValue(":recvDiaWt", c.receiveDiamondWt);

    q.bindValue(":acc", c.accountantId);
    q.bindValue(":status", c.status);

    if (!q.exec()) {
        db.rollback();
        qCritical() << q.lastError();
        return false;
    }

    return db.commit();
}

bool DatabaseUtils::updateCasting(int castingId, const CastingData &c)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare(R"(
        UPDATE casting_entry SET
            casting_date = :date,
            casting_name = :name,
            pcs = :pcs,

            issue_metal_name = :metal,
            issue_metal_purity = :purity,
            issue_metal_wt = :metalWt,
            issue_diamond_pcs = :diaPcs,
            issue_diamond_wt = :diaWt,

            receive_runner_wt = :runner,
            receive_product_wt = :product,
            receive_diamond_pcs = :recvDiaPcs,
            receive_diamond_wt = :recvDiaWt,

            status = :status
        WHERE id = :id
    )");

    q.bindValue(":id", castingId);
    q.bindValue(":date", c.castingDate);
    q.bindValue(":name", c.castingName);
    q.bindValue(":pcs", c.pcs);

    q.bindValue(":metal", c.issueMetalName);
    q.bindValue(":purity", c.issueMetalPurity);
    q.bindValue(":metalWt", c.issueMetalWt);
    q.bindValue(":diaPcs", c.issueDiamondPcs);
    q.bindValue(":diaWt", c.issueDiamondWt);

    q.bindValue(":runner", c.receiveRunnerWt);
    q.bindValue(":product", c.receiveProductWt);
    q.bindValue(":recvDiaPcs", c.receiveDiamondPcs);
    q.bindValue(":recvDiaWt", c.receiveDiamondWt);

    q.bindValue(":status", c.status);

    return q.exec();
}


