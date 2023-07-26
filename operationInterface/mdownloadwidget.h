#ifndef MDOWNLOADWIDGET_H
#define MDOWNLOADWIDGET_H

#include <QWidget>
#include <QThreadPool>
#include "thread/mythread.h"
#include "thread/mdownloadthread.h"
#include "../fileinfowidget.h"
#include "deviceInfoWidget/deviceinfowidget.h"
class MDownloadWidget;
#include "mainwindow.h"

namespace Ui {
class MDownloadWidget;
}

class MDownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MDownloadWidget(QThreadPool *pool, QMap<QString, MyThread*> &threads, unsigned int &threadsCnt, QVector<DeviceInfoWidget *> *deviceList, QString folderPath, QWidget *parent = nullptr);
    ~MDownloadWidget();

private:
    Ui::MDownloadWidget *ui;
    QString folderPath;
    QVector<FileInfoWidget *> mFilesList;           //存放文件列表
    QVector<DeviceInfoWidget*> *deviceList;
    //MyThread** threads;
    QMap<QString, MyThread*> &threads;
    QThreadPool *pool;
    QStringList checkedFileList;
    int numFilesSelected = 0;
    unsigned int& threadsCnt;
    unsigned int finishedThreadCnt = 0;
    void loadFolderFiles(const QString &path);
    QWidget* parent;
private slots:
    void beginMDownload();
    void selectAllFilesOrNot(bool checked);
    void radioChecked(bool checked);
    void on_selectFloderBtn_clicked();

    void transmitLNSDes(QString msg, QString deviceName);

signals:
    void sendLNSStatusDes(QString, QString);
    void MDownloadFinish();
};

#endif // MDOWNLOADWIDGET_H
