#ifndef AUTOCONFIGWIDGET_H
#define AUTOCONFIGWIDGET_H

#include <QWidget>
#include "device.h"
#include "ui_autoconfigwidget.h"
#include "thread/mythread.h"
#include "thread/uploadthread.h"
#include "thread/mdownloadthread.h"
#include "thread/autoconfigthread.h"
#include "deviceInfoWidget/deviceinfowidget.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include <QThreadPool>
class AutoConfigWidget;
#include "mainwindow.h"

namespace Ui {
class AutoConfigWidget;
}

class AutoConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AutoConfigWidget(QThreadPool* pool, QList<MyThread*> &threads, unsigned int &threadsCnt, QList<const Device*>* devices, QVector<DeviceInfoWidget *> *deviceList,QWidget *parent = nullptr);
    ~AutoConfigWidget();

    void initTableWidget();

    void addRowItems(const QString &devInfo, const QString &ip);

private:
    Ui::AutoConfigWidget *ui;

    QThreadPool *pool;

    //MyThread** threads;

    unsigned int finishedThreadCnt = 0;

    QList<MyThread*> &threads;

    unsigned int& threadsCnt;

    QMap<Device, QStringList> deviceFileInfo;

    QMap<QString, QByteArray> fileMd5;

    QList<Device> deviceIP;

    void createNewRowItem(int rowNum, const QString &devName, const QString &ip);

    bool MD5_test(QString filePath1, QString filePath2);

    QVector<DeviceInfoWidget *> *deviceList;

    QWidget *parent;

    QMap<QString, size_t> path_number;

    QMap<QString, size_t> fileName_used;



private slots:
    void selectFiles();

    void cancelFiles();
    void on_beginCfgBtn_clicked();

signals:
    void sendAutoConfigStatusMessage(QString message, QString deviceName);
    void AutoConfigFinish();

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // AUTOCONFIGWIDGET_H
