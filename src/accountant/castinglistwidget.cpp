#include "castinglistwidget.h"
#include "ui_castinglist.h"

#include "database/databaseutils.h"
#include "accountant/castingwidget.h"

#include <QMenu>
// #include <QMdiArea>
#include <QMdiSubWindow>

CastingListWidget::CastingListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CastingListWidget)
{
    ui->setupUi(this);

    setupTable();

    ui->castingTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->castingTableWidget,
            &QTableWidget::customContextMenuRequested,
            this,
            &CastingListWidget::onTableContextMenu);


    loadOrders();

}

CastingListWidget::~CastingListWidget()
{
    delete ui;
}

void CastingListWidget::setupTable()
{
    auto *table = ui->castingTableWidget;

    QStringList headers = {
        "Job No",
        "Delivery Date",
        "Casting Date",
        "Vendor Name",
        "PCS",
        "Issue Metal",
        "Purity",
        "Issue Metal Wt.",
        "Issue Dia Pcs.",
        "Issue Dia Wt.",
        "Ranar Wt.",
        "Product Wt",
        "Receive Dia Pcs.",
        "Receive Dia Wt.",
        "Gross Loss",
        "Fine Loss",
        "Dia Pcs Loss",
        "Dia Wt. Loss",
        "Dia Price",
        "Dia Loss Price",
        "Status",
        "Action"
    };

    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    table->verticalHeader()->setVisible(false);
}

void CastingListWidget::loadOrders()
{
    auto *table = ui->castingTableWidget;
    table->setRowCount(0);

    auto orders = DatabaseUtils::getJobsForCasting();

    for (int i = 0; i < orders.size(); ++i) {
        table->insertRow(i);

        // Job No
        // QString jobNo = "JOB" + QString("%1").arg(order.jobId, 7, 10, QChar('0'));
        auto *jobItem = new QTableWidgetItem(QString::number(orders[i].first));
        jobItem->setData(Qt::UserRole, orders[i].first); // store job_id
        table->setItem(i, 0, jobItem);

        // Delivery Date
        table->setItem(i, 1,
                       new QTableWidgetItem(orders[i].second)
                       );
    }
}

void CastingListWidget::onTableContextMenu(const QPoint &pos)
{
    auto *table = ui->castingTableWidget;

    QModelIndex index = table->indexAt(pos);
    if (!index.isValid())
        return;

    int row = index.row();

    // Fetch job_id from column 0 (Job No)
    QTableWidgetItem *item = table->item(row, 0);
    if (!item)
        return;

    int jobId = item->data(Qt::UserRole).toInt();
    if (jobId <= 0)
        return;

    QMenu menu(this);

    QAction *castingAction = menu.addAction("Casting");

    QAction *selectedAction =
        menu.exec(table->viewport()->mapToGlobal(pos));

    if (selectedAction == castingAction) {
        openCastingWidget(jobId);
    }
}

QMdiArea* CastingListWidget::findMdiArea(QWidget *w)
{
    while (w) {
        if (auto *mdi = qobject_cast<QMdiArea *>(w))
            return mdi;
        w = w->parentWidget();
    }
    return nullptr;
}

void CastingListWidget::openCastingWidget(int jobId)
{
    QMdiArea *mdi = findMdiArea(this);
    if (!mdi)
        return;

    // 🔍 Check if already open
    for (QMdiSubWindow *sub : mdi->subWindowList()) {
        auto *w = qobject_cast<CastingWidget *>(sub->widget());
        if (w && w->jobId() == jobId) {
            mdi->setActiveSubWindow(sub);
            return;   // ✅ DO NOT open duplicate
        }
    }

    // 🆕 Open new Casting window
    auto *castingWidget = new CastingWidget;
    castingWidget->setJobId(jobId);

    QMdiSubWindow *sub = mdi->addSubWindow(castingWidget);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    sub->setWindowTitle(QString("Casting - Job %1").arg(jobId));

    castingWidget->show();
}


