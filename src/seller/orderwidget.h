#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H

#include <QWidget>
#include "models/OrderData.h"

namespace Ui {
class OrderWidget;
}

class OrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OrderWidget(QWidget *parent = nullptr);
    explicit OrderWidget(int orderId, QWidget *parent = nullptr);
    ~OrderWidget();

private slots:
    void on_savePushButton_clicked();

private:
    Ui::OrderWidget *ui;    

    QString imagePath1, imagePath2;

    void fillOrderData(OrderData &o);
    bool validateInput() const;
    void prefillData();

    void setupMetalComboBoxes();
    void setupCertificateComboBoxes();
    void setupPolishOptions();
    void setupImageUploadHandlers();
    void setupDateFields();
    QString selectAndSaveImage(const QString &prefix);

    void loadOrder(int orderId);
    bool isEditMode = false;
    int m_orderId = 0;

};

#endif // ORDERWIDGET_H
