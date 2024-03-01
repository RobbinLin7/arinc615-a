﻿#ifndef UPLOADWIDGET_H
#define UPLOADWIDGET_H

#include <QWidget>
#include <QFile>
#include <QErrorMessage>
#include <QAbstractButton>
#include <QThreadPool>
#include "../thread/mythread.h"
#include "../thread/uploadthread.h"
#include "../fileinfowidget.h"
#include "../thread/mythread.h"
#include "../deviceInfoWidget/deviceinfowidget.h"
#include "progressdialog.h"
#include "dir.h"
class UploadWidget;
#include "mainwindow.h"

namespace Ui {
class UploadWidget;
}

class UploadWidget : public QWidget
{
    Q_OBJECT

public:
    //explicit UploadWidget(QStringList files, QWidget *parent = nullptr);
    explicit UploadWidget(QThreadPool* pool, QList<MyThread*>& threads, unsigned int& threadsCnt, QVector<DeviceInfoWidget *> *deviceList, QStringList files, QWidget *parent = nullptr);
    ~UploadWidget();
    void addFiles(QStringList);

private:
    Ui::UploadWidget *ui;
    QWidget *parent;
    QStringList files;
    QStringList fileSelected;
    unsigned int& threadsCnt;
    QVector<FileInfoWidget *> mFilesList;           //存放文件列表
    int numFilesSelected;                           //记录选中文件数目
    //QErrorMessage *dialog;
    QThreadPool* pool;
    //MyThread** threads;
    QList<MyThread*>& threads;
    bool beginUpload();
    QVector<DeviceInfoWidget *> *deviceList;
    unsigned int finishedThreadCnt = 0;
    QList<QListWidgetItem*> widgetItemList;
    std::shared_ptr<ProgressDialog> progressDialog;
private slots:
    void radioChecked(bool checked, QString fileName);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_checkBox_toggled(bool checked);
public slots:
    void on_LUS_received(File_LUS* LUS);
signals:
    void beginUploadBtnClicked(QStringList pathList);
    void uploadStatusMsg(QString message, QString fileName);
    void uploadFinish();
};

#endif // UPLOADWIDGET_H
