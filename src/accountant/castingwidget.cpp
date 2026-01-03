#include "castingwidget.h"
#include "ui_casting.h"

#include <QMessageBox>
#include <QMdiSubWindow>
#include "database/databaseutils.h"
#include "common/sessionmanager.h"

CastingWidget::CastingWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CastingWidget)
{
    ui->setupUi(this);
}

CastingWidget::~CastingWidget()
{
    delete ui;
}

int CastingWidget::jobId() const
{
    return m_jobId;
}

void CastingWidget::setJobId(int jobId)
{
    m_jobId = jobId;

    // Optional: show in UI label
    QString jobNo = "JOB" + QString("%1").arg(jobId, 7, 10, QChar('0'));
    ui->jobNoLabel->setText(jobNo);

    ui->issueDateEdit->setDate(QDate::currentDate());
    ui->issueDateEdit->setCalendarPopup(true);

    // Later we will:
    // - Load order details
    // - Check existing casting
}

CastingData CastingWidget::collectCastingData() const
{
    CastingData c;

    c.jobId = m_jobId;

    // ----- Detail -----
    c.castingDate = ui->issueDateEdit->date().toString("yyyy-MM-dd");
    c.castingName = ui->nameLineEdit->text().trimmed();
    c.pcs = ui->pcsSpinBox->value();

    // ----- Issue -----
    c.issueMetalName = ui->metalNameComboBox->currentText();
    c.issueMetalPurity = ui->metalPurityComboBox->currentText();
    c.issueMetalWt = ui->issueMetalWtDoubleSpinBox->value();
    c.issueDiamondPcs = ui->issueDiaPcsSpinBox->value();
    c.issueDiamondWt = ui->issueDiaWtDoubleSpinBox->value();

    // ----- Receive -----
    c.receiveRunnerWt = ui->ranarWtDoubleSpinBox->value();
    c.receiveProductWt = ui->productWtDoubleSpinBox->value();
    c.receiveDiamondPcs = ui->diaPcsSpinBox->value();
    c.receiveDiamondWt = ui->diaWtDoubleSpinBox->value();

    // ----- Meta -----
    c.accountantId = SessionManager::currentUser().id;
    c.status = "OPEN";

    return c;
}

void CastingWidget::on_savePushButton_clicked()
{
    if (m_jobId <= 0) {
        QMessageBox::warning(this, "Error", "Invalid Job");
        return;
    }

    CastingData data = collectCastingData();

    int castingId = DatabaseUtils::getCastingIdByJob(m_jobId);

    bool ok = false;
    if (castingId > 0) {
        ok = DatabaseUtils::updateCasting(castingId, data);
    } else {
        ok = DatabaseUtils::insertCasting(data);
    }

    if (!ok) {
        QMessageBox::critical(this, "Error", "Failed to save casting");
        return;
    }

    QMessageBox::information(this, "Success", "Casting saved successfully");

    if (auto *sub = qobject_cast<QMdiSubWindow *>(parentWidget()))
        sub->close();
}
