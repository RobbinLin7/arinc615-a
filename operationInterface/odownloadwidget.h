#ifndef ODOWNLOADWIDGET_H
#define ODOWNLOADWIDGET_H

#include <QWidget>
#include <QUdpSocket>
#include "deviceInfoWidget/deviceinfowidget.h"
#include "tftp/tftp.h"
#include "thread/mythread.h"
#include "thread/odownloadthread.h"
#include <QThreadPool>
#include "fileinfowidget.h"
class ODownloadWidget;
#include "mainwindow.h"
namespace Ui {
class ODownloadWidget;
}

class ODownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ODownloadWidget(QThreadPool* pool, QMap<QString, MyThread*>& threads, unsigned int& threadsCnt, QVector<DeviceInfoWidget *> *deviceList, QWidget *parent = nullptr);
    ~ODownloadWidget();

private:
    Ui::ODownloadWidget *ui;
    QVector<DeviceInfoWidget *>* deviceList;
    const Device* device;
    //MyThread** threads;
    QMap<QString, MyThread*> &threads;
    QThreadPool *pool;
    QVector<FileInfoWidget *> mFilesList;           //存放文件列表
    int numFilesSelected = 0;
    unsigned int &threadsCnt;
    unsigned int finishedThreadCnt = 0;
    QWidget *parent;
private slots:
    void receiveFileList(QList<QPair<QString, QString>> *fileList);
    void downloadBtnClikcked();
    void selectAllFilesOrNot(bool checked);
    void radioChecked(bool checked);
    void transmitODLNSDes(QString msg, QString deviceName);
signals:
    void filesChecked(QStringList fileList);
    void ODownloadFinish();
    void sendODLNSStatusDes(QString, QString);
};

#endif // ODOWNLOADWIDGET_H
