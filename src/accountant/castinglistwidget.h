#ifndef CASTINGLISTWIDGET_H
#define CASTINGLISTWIDGET_H

#include <QWidget>
#include <QMdiArea>

namespace Ui {
class CastingListWidget;
}

class CastingListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CastingListWidget(QWidget *parent = nullptr);
    ~CastingListWidget();

private slots:
    void onTableContextMenu(const QPoint &pos);


private:
    Ui::CastingListWidget *ui;

    void setupTable();
    void loadOrders();

    void openCastingWidget(int jobId);
    QMdiArea* findMdiArea(QWidget *w);
};

#endif // CASTINGLISTWIDGET_H
