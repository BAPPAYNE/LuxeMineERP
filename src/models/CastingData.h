#ifndef CASTINGDATA_H
#define CASTINGDATA_H

#include <QString>

struct CastingData
{
    // ---------- Primary ----------
    int id = 0;

    // ---------- Linking ----------
    int jobId = 0;
    int orderId = 0;

    // ---------- Casting Details ----------
    QString castingDate;     // yyyy-MM-dd
    QString castingName;
    int pcs = 0;

    // ---------- Issue Metal & Diamond ----------
    QString issueMetalName;
    QString issueMetalPurity;
    double issueMetalWt = 0.0;
    int issueDiamondPcs = 0;
    double issueDiamondWt = 0.0;

    // ---------- Receive Metal & Diamond ----------
    double receiveRunnerWt = 0.0;
    double receiveProductWt = 0.0;
    int receiveDiamondPcs = 0;
    double receiveDiamondWt = 0.0;

    // ---------- Meta ----------
    int accountantId = 0;
    QString status;          // OPEN / RECEIVED / CLOSED
    QString createdAt;
};

#endif // CASTINGDATA_H
