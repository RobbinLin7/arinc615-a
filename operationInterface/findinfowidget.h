#ifndef FINDINFOWIDGET_H
#define FINDINFOWIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QList>

namespace Ui {
class FindInfoWidget;
}

class FindInfoWidget : public QWidget
{
    Q_OBJECT

#define MAX_COLUMN_COUNT            6           //表的列数

public:
    explicit FindInfoWidget(QWidget *parent = nullptr);
    ~FindInfoWidget();

    void insertDeviceInfo(const QStringList &deviceInfo);

    void clearTableWidget();

private:
    Ui::FindInfoWidget *ui;
    QList<QTableWidgetItem*> widgetItemList;
    void initTableWidget();
    int mCurrentRow;         //记录当前行数
};

#endif // FINDINFOWIDGET_H
