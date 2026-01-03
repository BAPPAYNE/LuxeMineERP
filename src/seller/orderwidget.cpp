#include "orderwidget.h"
#include "ui_order.h"

#include "database/DatabaseUtils.h"
#include "common/sessionmanager.h"
#include "models/imageclicklabel.h"

#include <QMessageBox>
#include <QDate>
#include <QStandardItemModel>
#include <QDir>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QPixmap>

OrderWidget::OrderWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OrderWidget)
{
    ui->setupUi(this);

    setMinimumSize(100, 100);
    setMaximumSize(1180, 450);
    resize(1180, 480);

    prefillData();

    setupMetalComboBoxes();
    setupCertificateComboBoxes();
    setupPolishOptions();
    setupImageUploadHandlers();
    setupDateFields();
}

OrderWidget::OrderWidget(int orderId, QWidget *parent)
    : QWidget(parent), ui(new Ui::OrderWidget), isEditMode(true), m_orderId(orderId)
{
    ui->setupUi(this);
    setMinimumSize(100, 100);
    setMaximumSize(1180, 450);
    resize(1180, 480);

    ui->savePushButton->setText("Update");

    setupDateFields();
    setupMetalComboBoxes();
    setupCertificateComboBoxes();
    setupPolishOptions();
    setupImageUploadHandlers();

    loadOrder(orderId);
}

OrderWidget::~OrderWidget()
{
    delete ui;
}

void OrderWidget::prefillData()
{
    // Seller info (from session)
    ui->sellerNameLineEdit->setText(
        SessionManager::currentUser().username
        );
    ui->sellerIdLineEdit->setText(
        QString::number(SessionManager::currentUser().id)
        );

    // Order date (today)
    ui->orderDateDateEdit->setDisplayFormat("dd-MM-yyyy");
    ui->orderDateDateEdit->setDate(QDate::currentDate());

    // Lock fields
    ui->sellerNameLineEdit->setReadOnly(true);
    ui->sellerIdLineEdit->setReadOnly(true);
    ui->orderDateDateEdit->setReadOnly(true);
}

bool OrderWidget::validateInput() const
{
    // if (ui->jobNoLineEdit->text().isEmpty())
    //     return false;

    if (ui->partyNameLineEdit->text().isEmpty())
        return false;

    // if (ui->deliveryDateDateEdit->date() < QDate::currentDate())
    //     return false;

    return true;
}

void OrderWidget::fillOrderData(OrderData &order)
{
    // --------------------
    // Identity
    // --------------------
    // order.id = dummyOrderId;

    // --------------------
    // Seller (from session – DO NOT trust UI for this)
    // --------------------

    order.sellerId   = SessionManager::currentUser().id;
    order.sellerName = SessionManager::currentUser().username;

    // --------------------
    // Party & Client Hierarchy
    // --------------------
    order.partyId   = ui->partyIdLineEdit->text().trimmed();
    order.partyName = ui->partyNameLineEdit->text().trimmed();

    order.clientId    = ui->clientIdLineEdit->text().trimmed();
    order.agencyId    = ui->agencyIdLineEdit->text().trimmed();
    order.shopId      = ui->shopIdLineEdit->text().trimmed();
    order.retaillerId = ui->reteailleIdLineEdit->text().trimmed();
    order.starId      = ui->starIdLineEdit->text().trimmed();

    // --------------------
    // Address
    // --------------------
    order.address = ui->addressLineEdit->text().trimmed();
    order.city    = ui->cityLineEdit->text().trimmed();
    order.state   = ui->stateLineEdit->text().trimmed();
    order.country = ui->countryLineEdit->text().trimmed();

    // --------------------
    // Order Identity
    // --------------------
    // order.jobNo   = ui->jobNoLineEdit->text().trimmed();
    // order.orderNo = ui->orderNoLineEdit->text().trimmed();

    // --------------------
    // Dates
    // --------------------
    order.orderDate    = QDate::currentDate().toString("yyyy-MM-dd");
    order.deliveryDate = ui->deliveryDateDateEdit->date().toString("yyyy-MM-dd");

    // --------------------
    // Product
    // --------------------
    order.productName     = ui->productNameLineEdit->text().trimmed();
    order.productPis      = ui->productPisSpinBox->value();
    order.approxProductWt = ui->approxWeightDoubleSpinBox->value();
    order.approxDiamondWt = ui->approxDiaWeightDoubleSpinBox->value();

    // --------------------
    // Metal
    // --------------------
    order.metalName   = ui->metalNameComboBox->currentText();
    order.metalPurity = ui->metalPurityComboBox->currentText();
    order.metalColor  = ui->metalColorComboBox->currentText();
    order.metalPrice  = ui->metalPriceDoubleSpinBox->value();

    // --------------------
    // Size & Dimensions
    // --------------------
    order.sizeNo = ui->sizeNoDoubleSpinBox->value();
    order.sizeMM = ui->sizeMMDoubleSpinBox->value();
    order.length = ui->lengthDoubleSpinBox->value();
    order.width  = ui->widthDoubleSpinBox->value();
    order.height = ui->heightDoubleSpinBox->value();

    // --------------------
    // Diamond
    // --------------------
    order.diaPacific = ui->diaPacificLineEdit->text().trimmed();
    order.diaPurity  = ui->diaPurityLineEdit->text().trimmed();
    order.diaColor   = ui->diaColorLineEdit->text().trimmed();
    order.diaPrice   = ui->diaPriceDoubleSpinBox->value();

    // --------------------
    // Stone
    // --------------------
    order.stPacific = ui->stPacificLineEdit->text().trimmed();
    order.stPurity  = ui->stPurityLineEdit->text().trimmed();
    order.stColor   = ui->stColorLineEdit->text().trimmed();
    order.stPrice   = ui->stPriceDoubleSpinBox->value();

    // --------------------
    // Design & Images
    // --------------------
    order.designNo   = ui->designNoLineEdit->text().trimmed();
    // order.designNo2  = ui->designNoLineEdit2->text().trimmed();
    order.image1Path = imagePath1;
    order.image2Path = imagePath2;

    // --------------------
    // Certification (Multi-select)
    // --------------------
    order.metalCertiName = ui->metalCertiNameComboBox->currentText();
    {
        QStringList list;
        auto model = qobject_cast<QStandardItemModel*>(ui->metalCertiTypeComboBox->model());
        for (int i = 0; i < model->rowCount(); ++i)
            if (model->item(i)->checkState() == Qt::Checked)
                list << model->item(i)->text();
        order.metalCertiType = list.join(", ");
    }

    order.diaCertiName = ui->diaCertiNameComboBox->currentText();
    {
        QStringList list;
        auto model = qobject_cast<QStandardItemModel*>(ui->diaCertiTypeComboBox->model());
        for (int i = 0; i < model->rowCount(); ++i)
            if (model->item(i)->checkState() == Qt::Checked)
                list << model->item(i)->text();
        order.diaCertiType = list.join(", ");
    }

    // --------------------
    // Manufacturing Options
    // --------------------
    order.pesSaki       = ui->pesSakiComboBox->currentText();
    order.chainLock     = ui->chainLockComboBox->currentText();
    order.polish        = ui->polishComboBox->currentText();
    order.settingLebour = ui->settingLabourComboBox->currentText();
    order.metalStemp    = ui->metalStempComboBox->currentText();

    // --------------------
    // Payment
    // --------------------
    order.paymentMethod = ui->payMethodComboBox->currentText();
    order.totalAmount   = ui->totalAmountDoubleSpinBox->value();
    order.advance       = ui->advanceDoubleSpinBox->value();
    order.remaining     = order.totalAmount - order.advance;

    // --------------------
    // Notes
    // --------------------
    order.note        = ui->notePlainTextEdit->toPlainText().trimmed();
    order.extraDetail = ui->extraDetailPlainTextEdit->toPlainText().trimmed();

    // --------------------
    // State
    // --------------------
    order.isSaved = 1;

}

void OrderWidget::on_savePushButton_clicked()
{
    if (!validateInput())
        return;

    OrderData o;
    fillOrderData(o);

    if (isEditMode) {
        if (!DatabaseUtils::updateOrder(m_orderId, o)) {
            QMessageBox::critical(this, "Error", "Failed to update order");
            return;
        }
        QMessageBox::information(this, "Success", "Order updated successfully");
    } else {
        int jobId = 0, sellerSeq = 0;
        if (!DatabaseUtils::createOrder(o, jobId, sellerSeq)) {
            QMessageBox::critical(this, "Error", "Failed to create order");
            return;
        }
        QMessageBox::information(this, "Success", "Order created successfully");
    }

    if (auto *sub = qobject_cast<QMdiSubWindow *>(parentWidget()))
        sub->close();
}

void OrderWidget::setupDateFields() {
    QDate today = QDate::currentDate();
    QDate minDeliveryDate = today.addDays(1);

    ui->orderDateDateEdit->setDate(today);
    ui->orderDateDateEdit->setCalendarPopup(true);
    ui->orderDateDateEdit->setReadOnly(true);
    ui->orderDateDateEdit->setEnabled(false);

    ui->deliveryDateDateEdit->setDate(minDeliveryDate);
    // ui->deliveryDateDateEdit->setMinimumDate(minDeliveryDate);
    ui->deliveryDateDateEdit->setCalendarPopup(true);
}

void OrderWidget::setupCertificateComboBoxes() {
    QStringList metalCertificates = {
        "Hallmark", "Purity Card", "Purity Mark", "Assay Certificate", "NABL Certificate",
        "ISO Certification", "In-house Certificate", "Recycled Metal Certificate"
    };

    QStandardItemModel* metalModel = new QStandardItemModel(ui->metalCertiTypeComboBox);
    for (const QString& cert : metalCertificates) {
        QStandardItem* item = new QStandardItem(cert);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        metalModel->appendRow(item);
    }
    ui->metalCertiTypeComboBox->setModel(metalModel);
    ui->metalCertiTypeComboBox->setEditable(true);
    ui->metalCertiTypeComboBox->lineEdit()->setReadOnly(true);
    connect(metalModel, &QStandardItemModel::itemChanged, this, [=]() {
        QStringList selected;
        for (int i = 0; i < metalModel->rowCount(); ++i) {
            QStandardItem* item = metalModel->item(i);
            if (item->checkState() == Qt::Checked) selected << item->text();
        }
        ui->metalCertiTypeComboBox->lineEdit()->setText(selected.join(", "));
    });

    ui->metalCertiNameComboBox->addItem("-");
    ui->metalCertiNameComboBox->addItems({"Gold", "Silver", "Platinum", "Copper", "Palladium", "Titanium"});
    connect(ui->metalCertiNameComboBox, &QComboBox::currentTextChanged, this, [=]() {
        for (int i = 0; i < metalModel->rowCount(); ++i)
            metalModel->item(i)->setCheckState(Qt::Unchecked);
        ui->metalCertiTypeComboBox->lineEdit()->clear();
    });

    ui->diaCertiNameComboBox->addItem("-");
    ui->diaCertiNameComboBox->addItems({"Diamond", "CVD", "HPHT", "Stone"});

    QStringList diaCertTypes = {"-", "IGI", "GIA", "HRD", "SGL"};
    QStandardItemModel* diaCertModel = new QStandardItemModel(ui->diaCertiTypeComboBox);
    for (const QString& cert : diaCertTypes) {
        QStandardItem* item = new QStandardItem(cert);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        diaCertModel->appendRow(item);
    }
    ui->diaCertiTypeComboBox->setModel(diaCertModel);
    ui->diaCertiTypeComboBox->setEditable(true);
    ui->diaCertiTypeComboBox->lineEdit()->setReadOnly(true);
    connect(diaCertModel, &QStandardItemModel::itemChanged, this, [=]() {
        QStringList selected;
        for (int i = 1; i < diaCertModel->rowCount(); ++i) {
            if (diaCertModel->item(i)->checkState() == Qt::Checked)
                selected << diaCertModel->item(i)->text();
        }
        ui->diaCertiTypeComboBox->lineEdit()->setText(selected.join(", "));
    });
}

void OrderWidget::setupMetalComboBoxes() {
    ui->metalNameComboBox->addItem("-");
    ui->metalNameComboBox->addItems({"Gold", "Silver"}); //, "Copper", "Palladium", "Titanium", "Tungsten", "Brass"});

    QMap<QString, QStringList> purityMap;
    purityMap["Gold"] = { "24K (99.9%)", "22K (91.6%)", "20K (83.3%)", "18K (75%)", "14K (58.5%)", "10K (41.7%)", "9K (37.5%)" };
    purityMap["Silver"] = { "Fine Silver (99.9%)", "Sterling Silver (92.5%)", "Coin Silver (90%)" };
    // purityMap["Platinum"] = { "950 Platinum", "900 Platinum", "850 Platinum" };
    // purityMap["Copper"] = { "Pure Copper (99.9%)" };
    // purityMap["Palladium"] = { "950 Palladium", "900 Palladium" };
    // purityMap["Titanium"] = { "Commercial Pure", "Grade 5 (90% Ti)" };
    // purityMap["Tungsten"] = { "Tungsten Carbide" };
    // purityMap["Brass"] = { "Red Brass (85% Cu)", "Yellow Brass (65% Cu)", "Cartridge Brass (70% Cu)" };

    ui->metalColorComboBox->addItems({"-", "Yellow", "White", "Rose"}); // , "Green", "Champagne", "Black (Rhodium)", "Two-Tone"});
    ui->metalColorComboBox->setEnabled(false);

    connect(ui->metalNameComboBox, &QComboBox::currentTextChanged, this, [=](const QString &metal) {
        ui->metalPurityComboBox->clear();
        if (purityMap.contains(metal)) {
            ui->metalPurityComboBox->addItem("-");
            ui->metalPurityComboBox->addItems(purityMap.value(metal));
        } else {
            ui->metalPurityComboBox->addItem("-");
        }

        bool enableColor = (metal == "Gold");
        ui->metalColorComboBox->setEnabled(enableColor);
        ui->metalColorComboBox->setCurrentIndex(0);
    });

    ui->metalNameComboBox->setCurrentIndex(0);
    ui->metalPurityComboBox->addItem("-");
}

void OrderWidget::setupPolishOptions() {
    ui->pesSakiComboBox->addItem("-");
    ui->pesSakiComboBox->addItems({"South Pes", "Paip South", "Regular", "Antic Pes"});

    ui->chainLockComboBox->addItem("-");
    ui->chainLockComboBox->addItems({"S Huck", "Italian Looks", "Kadi Akda"});

    ui->polishComboBox->addItem("-");
    ui->polishComboBox->addItems({"Regular", "Export", "Dal", "Mat", "Raf"});

    ui->settingLabourComboBox->addItem("-");
    ui->settingLabourComboBox->addItems({"OpenSetting-150", "CloseSetting-145", "Kundan-120", "Stone-100"});

    ui->metalStempComboBox->addItem("-");
    ui->metalStempComboBox->addItems({"Company Name Mark", "Logo Mark", "PurityMark + DiamondMark"});

    ui->payMethodComboBox->addItem("-");
    ui->payMethodComboBox->addItems({
        "Cash", "RTGS", "Author Bank Pay", "Gold convert pay",
        "Diamond convert pay", "Hawala Pay", "Angdiya", "VPP", "UPI", "Card Pay"
    });
}

void OrderWidget::setupImageUploadHandlers() {
    connect(ui->imageLabel1, &ImageClickLabel::rightClicked, this, [=]() {
        QString path = selectAndSaveImage("image1");
        if (!path.isEmpty()) {
            ui->imageLabel1->setPixmap(QPixmap(path).scaled(ui->imageLabel1->size(), Qt::KeepAspectRatio));
            imagePath1 = path;
        }
    });

    connect(ui->imageLabel2, &ImageClickLabel::rightClicked, this, [=]() {
        QString path = selectAndSaveImage("image2");
        if (!path.isEmpty()) {
            ui->imageLabel2->setPixmap(QPixmap(path).scaled(ui->imageLabel2->size(), Qt::KeepAspectRatio));
            imagePath2 = path;
        }
    });
}

QString OrderWidget::selectAndSaveImage(const QString &prefix) {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), "Images (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) return "";

    QDir appDir(QCoreApplication::applicationDirPath());
    QString imageDirPath = appDir.filePath("OrderBookImages");

    QDir().mkpath(imageDirPath);  // ensure OrderBookImages/ exists

    QString fileName = prefix + "_" + QFileInfo(filePath).fileName();
    QString destPath = QDir(imageDirPath).filePath(fileName);

    // ✅ If already exists, skip copying
    if (QFile::exists(destPath)) {
        qDebug() << "⚠️ Image already exists, using existing file:" << destPath;
        return destPath;
    }

    // Try to copy
    if (QFile::copy(filePath, destPath)) {
        qDebug() << "✅ Image copied to:" << destPath;
        return destPath;
    } else {
        QMessageBox::warning(this, "Image Copy Failed", "Could not copy image to:\n" + destPath);
        return "";
    }
}

void OrderWidget::loadOrder(int orderId)
{
    OrderData o;
    if (!DatabaseUtils::getOrderById(orderId, o)) {
        QMessageBox::critical(this, "Error", "Failed to load order");
        return;
    }

    QString orderNo = o.sellerName
                      + QString("%1").arg(o.sellerOrderSeq, 5, 10, QChar('0'));


    QString jobNo = "JOB" + QString("%1").arg(o.jobId, 7, 10, QChar('0'));

    // --------------------
    // Identity
    // --------------------
    ui->orderNoLineEdit->setText(orderNo);
    ui->jobNoLineEdit->setText(jobNo);

    // --------------------
    // Seller (read-only / informational if shown)
    // --------------------
    ui->sellerNameLineEdit->setText(o.sellerName);

    // --------------------
    // Party & Client Hierarchy
    // --------------------
    ui->partyIdLineEdit->setText(o.partyId);
    ui->partyNameLineEdit->setText(o.partyName);

    ui->clientIdLineEdit->setText(o.clientId);
    ui->agencyIdLineEdit->setText(o.agencyId);
    ui->shopIdLineEdit->setText(o.shopId);
    ui->reteailleIdLineEdit->setText(o.retaillerId);
    ui->starIdLineEdit->setText(o.starId);

    // --------------------
    // Address
    // --------------------
    ui->addressLineEdit->setText(o.address);
    ui->cityLineEdit->setText(o.city);
    ui->stateLineEdit->setText(o.state);
    ui->countryLineEdit->setText(o.country);

    // --------------------
    // Dates
    // --------------------
    ui->orderDateDateEdit->setDate(
        QDate::fromString(o.orderDate, "yyyy-MM-dd")
        );
    ui->deliveryDateDateEdit->setDate(
        QDate::fromString(o.deliveryDate, "yyyy-MM-dd")
        );

    // --------------------
    // Product
    // --------------------
    ui->productNameLineEdit->setText(o.productName);
    ui->productPisSpinBox->setValue(o.productPis);
    ui->approxWeightDoubleSpinBox->setValue(o.approxProductWt);
    ui->approxDiaWeightDoubleSpinBox->setValue(o.approxDiamondWt);

    // --------------------
    // Metal
    // --------------------
    ui->metalNameComboBox->setCurrentText(o.metalName);
    ui->metalPurityComboBox->setCurrentText(o.metalPurity);
    ui->metalColorComboBox->setCurrentText(o.metalColor);
    ui->metalPriceDoubleSpinBox->setValue(o.metalPrice);

    // --------------------
    // Size & Dimensions
    // --------------------
    ui->sizeNoDoubleSpinBox->setValue(o.sizeNo);
    ui->sizeMMDoubleSpinBox->setValue(o.sizeMM);
    ui->lengthDoubleSpinBox->setValue(o.length);
    ui->widthDoubleSpinBox->setValue(o.width);
    ui->heightDoubleSpinBox->setValue(o.height);

    // --------------------
    // Diamond
    // --------------------
    ui->diaPacificLineEdit->setText(o.diaPacific);
    ui->diaPurityLineEdit->setText(o.diaPurity);
    ui->diaColorLineEdit->setText(o.diaColor);
    ui->diaPriceDoubleSpinBox->setValue(o.diaPrice);

    // --------------------
    // Stone
    // --------------------
    ui->stPacificLineEdit->setText(o.stPacific);
    ui->stPurityLineEdit->setText(o.stPurity);
    ui->stColorLineEdit->setText(o.stColor);
    ui->stPriceDoubleSpinBox->setValue(o.stPrice);

    // --------------------
    // Design & Images
    // --------------------
    ui->designNoLineEdit->setText(o.designNo);

    imagePath1 = o.image1Path;
    imagePath2 = o.image2Path;
    // refreshImagePreview(); // if you have preview logic

    // --------------------
    // Certification (Multi-select)
    // --------------------
    ui->metalCertiNameComboBox->setCurrentText(o.metalCertiName);
    {
        QStringList selected = o.metalCertiType.split(", ", Qt::SkipEmptyParts);
        auto model = qobject_cast<QStandardItemModel*>(ui->metalCertiTypeComboBox->model());
        for (int i = 0; i < model->rowCount(); ++i)
            model->item(i)->setCheckState(
                selected.contains(model->item(i)->text())
                    ? Qt::Checked
                    : Qt::Unchecked
                );
    }

    ui->diaCertiNameComboBox->setCurrentText(o.diaCertiName);
    {
        QStringList selected = o.diaCertiType.split(", ", Qt::SkipEmptyParts);
        auto model = qobject_cast<QStandardItemModel*>(ui->diaCertiTypeComboBox->model());
        for (int i = 0; i < model->rowCount(); ++i)
            model->item(i)->setCheckState(
                selected.contains(model->item(i)->text())
                    ? Qt::Checked
                    : Qt::Unchecked
                );
    }

    // --------------------
    // Manufacturing Options
    // --------------------
    ui->pesSakiComboBox->setCurrentText(o.pesSaki);
    ui->chainLockComboBox->setCurrentText(o.chainLock);
    ui->polishComboBox->setCurrentText(o.polish);
    ui->settingLabourComboBox->setCurrentText(o.settingLebour);
    ui->metalStempComboBox->setCurrentText(o.metalStemp);

    // --------------------
    // Payment
    // --------------------
    ui->payMethodComboBox->setCurrentText(o.paymentMethod);
    ui->totalAmountDoubleSpinBox->setValue(o.totalAmount);
    ui->advanceDoubleSpinBox->setValue(o.advance);

    // remaining is derived
    // ui->remainingDoubleSpinBox->setValue(o.remaining);

    // --------------------
    // Notes
    // --------------------
    ui->notePlainTextEdit->setPlainText(o.note);
    ui->extraDetailPlainTextEdit->setPlainText(o.extraDetail);
}


