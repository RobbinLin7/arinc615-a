﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QVBoxLayout>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkAddressEntry>
#include <QProgressDialog>
#include <QThreadPool>
#include <QMetaType>
#include <QProcessEnvironment>
#include "deviceInfoWidget/deviceinfowidget.h"
#include "protocal/arinc615_struct_define.h"
#include "topology/createtopologyxml.h"
#include "operationInterface/findinfowidget.h"
#include "operationInterface/informationwidget.h"
#include "operationInterface/configwidget.h"
#include "operationInterface/uploadwidget.h"
#include "operationInterface/odownloadwidget.h"
#include "operationInterface/mdownloadwidget.h"
#include "successdialog.h"
#include "finddialog.h"
#include "arinc615A/api_615a_socket.h"
#include "device.h"
#include "thread/uploadthread.h"
#include "thread/informationthread.h"
#include "operationInterface/autoconfigwidget.h"
#include "tftp/tftprequest.h"
#include "globalDefine.h"
#include "paraconfigdialog.h"
#include "dir.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QUdpSocket;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initMainWindow();
    void getAllEntry();
    void EnableOrdisableExceptFind(bool flag);
    void checkIfAnyDeviceSelect();
private slots:

    void execFindOperation();

    void execInformationOperation();

    void execUploadOperation();

    void execMediaDownloadOperation();

    void execOperatorDownloadOperation();

    void execAbortOperation();

    //
    void execExitTool();

    void selectAllDeviceOrNot(bool checked);

    void parseFindResponse();

    void onTimerTimeout();

    void find(int index);

    void on_radio_toggled(bool checked, const Device* device);

    void execAutoConfigOperation();

    void tftpServerTftpReadReady();

    void finishInformation(File_LCL* LCL_struct);

    //void beginUpload(QStringList pathList);

    void addLogToDockWidget(const int &operationCode, const QString log){
        addLogToDockWidget(operationCode, log, "default");
    }

    void addLogToDockWidget(const int &operationCode, const QString log, const QString deviceName);


private:
    Ui::MainWindow *ui;
    void enableAllExceptCurrentOperation(bool flag);
    int createDeviceList(const int &deviceNum, QList<Device>* devices);
    void clearDeviceList();
    void focusOnCurrentOperation();
    void unfocusOnCurrentOperation();
    QByteArray makeFindRequest();                   //构造FIND请求报文
    void disableAllExceptCurrentOperation(const unsigned int& op_code);

    QVector<DeviceInfoWidget *> mDevicesList;       //存放设备列表

    QVector<FileInfoWidget *> mFilesList;           //存放文件列表

    std::shared_ptr<FindInfoWidget> mFindInfoWidget;    //FIND操作widget

    std::shared_ptr<InformationWidget> mInformationWidget;  //INFORMATION操作widget

    std::shared_ptr<UploadWidget> mUploadWidget;            //UPLOAD操作widget

    std::shared_ptr<MDownloadWidget> mMDownloadWidget;      //介质定义下载操作widget

    std::shared_ptr<ODownloadWidget> mODownloadWidget;      //用户定义下载操作widget

    std::shared_ptr<AutoConfigWidget> mAutoConfigWidget;    //自动配置widget

    std::shared_ptr<ParaConfigDialog> paraConfigDialog = nullptr;

    std::shared_ptr<QVBoxLayout> layout;                                   //ui->operationWidget的layout

    std::shared_ptr<QWidget> operationWidget = 0;                      //指向当前operationWidget显示的对象

    QString uploadDir;                             //上传文件夹

    QString localIp;                               //记录本地IP地址

    QNetworkAddressEntry* entry = 0;

    QUdpSocket* uSock;                             //FIND操作socket,用于发送FIND请求报文和接受FIND相应报文

    int deviceCnt = 0;                                 //记录规定时间内响应FIND请求的设备数目

    QList<Device> *devices = 0;                        //记录响应FIND请求的设备信息

    QList<const Device*> *checkedDevices = 0;                  //记录选中的设备信息

    int checkedDevicesCnt = 0;                            //记录选中的设备数量

    int responseDevicesCnt = 0;                           //记录响应的设备数量

    QTimer *timer;

    QList<QNetworkAddressEntry> *entryList;        //记录本地所有IP信息

    QProgressDialog *progressDialog;

    QThreadPool pool;                              //线程池

    //MyThread* threads[15];                          //操作线程

    QList<MyThread*> threads;                         //操作线程

    QUdpSocket* tftpServer = 0;


    //单例模式
    FindDialog* findDialog = 0;

    FindDialog* getFindDialogInstance();

    //QUdpSocket* tftpClient = 0;

    //QList<QList<QByteArray>*> datagramFromAllDevices;

    //QList<TftpRequest *> tftpRequests;

    //a615_targets_find_list_t findList;

    unsigned int threadsCnt = 0;

    const unsigned int maxThreadCount = 15;

    QString userProfile;
signals:
    void mainThreadExit();
};
#endif // MAINWINDOW_H
