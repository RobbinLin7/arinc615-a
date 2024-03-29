﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_configwidget.h"
#include "mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
using namespace GlobalDefine;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置线程池大小
    pool.setMaxThreadCount(maxThreadCount);
    //在发现设备前，不允许进行其它操作
    EnableOrdisableExceptFind(false);
    checkedDevices = new QList<const Device*>();
    this->entryList = new QList<QNetworkAddressEntry>();
    userProfile = QProcessEnvironment::systemEnvironment().value("userprofile");
    qDebug() << entryList->size() << "entryList size";
    //启动tftp服务器线程，用于监听服务器请求
    tftpServerThread = std::shared_ptr<TftpServerThread>(new TftpServerThread());
    connect(this, &MainWindow::tftpServerThreadExitSignal, tftpServerThread.get(), &TftpServerThread::onRcvThreadExitSig);
    connect(tftpServerThread.get(), &TftpServerThread::requestReceivedSignal, this, &MainWindow::onRequestReceived);
    tftpServerThread->start();
//    tftpServer = new QUdpSocket(this);
//    tftpServer->bind(QHostAddress::AnyIPv4, 8888, QAbstractSocket::ShareAddress);
//    connect(tftpServer, &QUdpSocket::readyRead, this, &MainWindow::tftpServerTftpReadReady);

    initMainWindow();
    //触发加卸载操作的connect函数
    connect(ui->actionFindOp, SIGNAL(triggered()), this, SLOT(execFindOperation()));

    connect(ui->actionInformationOp, SIGNAL(triggered()), this, SLOT(execInformationOperation()));

    connect(ui->actionUploadOp, SIGNAL(triggered()), this, SLOT(execUploadOperation()));
    connect(ui->uploadPushButton, SIGNAL(clicked()), this, SLOT(execUploadOperation()));

    connect(ui->actionMediaDownloadOp, SIGNAL(triggered()), this, SLOT(execMediaDownloadOperation()));
    connect(ui->mediaDownloadPushBtn, SIGNAL(clicked()), this, SLOT(execMediaDownloadOperation()));

    connect(ui->actionOperatorDownloadOp, SIGNAL(triggered()), this, SLOT(execOperatorDownloadOperation()));
    connect(ui->operatorDownloadPushBtn, SIGNAL(clicked()), this, SLOT(execOperatorDownloadOperation()));

    connect(ui->actionAbortOp, SIGNAL(triggered()), this, SLOT(execAbortOperation()));
    connect(ui->informationPushBtn, SIGNAL(clicked()), this, SLOT(execInformationOperation()));
    connect(ui->actionAutoConfigOp, SIGNAL(triggered()), this, SLOT(execAutoConfigOperation()));
    //触发退出按钮
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(execExitTool()));

    //触发全选按钮
    connect(ui->selectALLOrNotCheckBox, SIGNAL(clicked(bool)), this, SLOT(selectAllDeviceOrNot(bool)));

    //触发自动配置按钮
    connect(ui->autoConfigBtn, SIGNAL(clicked()), this, SLOT(execAutoConfigOperation()));

    connect(ui->actionToolVersion, &QAction::triggered, this, [=](){
        QMessageBox::about(this,"About ARINC615ATool","ARINC615ATool V1.0.19(2023-07-13) \n新增特性\n"
                                                      "1.另启线程接收状态文件\n"
                                                      "2.修改了一些闪退错误\n"
                                                      "3.每次发现操作都检查是否需要更新当前的IP地址\n"
                                                       );
    });

    connect(ui->paraConfigAction, &QAction::triggered, this, &MainWindow::execParaConfigOperation);
}

MainWindow::~MainWindow()
{
    emit(mainThreadExit());
    emit(findThreadExitSignal());
    emit(tftpServerThreadExitSignal());
    findThread->wait();
    tftpServerThread->wait();
    saveLog();
    savexml();
    waitForAllWorkingThreadsDone();
    delete entry;
    delete entryList;
//    delete devices;
    delete checkedDevices;
    delete ui;
}

//==================================================================
//function name: initMainWindow
//author: lcy
//date: 2022-8-1
//function:初始化主界面的相关参数
//input arg: 无
//return:无
//log:8-1新增
//==================================================================
void MainWindow::initMainWindow()
{
    //设置名称
    this->setWindowTitle(MAINWINDOW_TITLE);

    //设置界面大小
    //this->resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    //设置布局
    layout = std::make_shared<QVBoxLayout>(ui->operationWidget);

    //输出日志信息
    ui->logTextBrowser->append("等待用户操作！");

    //隐藏listWidget中的横向滚动条
    ui->deviceListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

//==================================================================
//function name: execFindOperation
//author: lcy
//date: 2022-7-28
//function:作为响应界面触发的find操作的槽函数,执行find操作
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execFindOperation()
{
    timer = std::make_shared<QTimer>(this);
    //timer = new QTimer(this);
    getAllEntry();
    if(this->entryList->size() > 0)
    {
        FindDialog *findDialogInstance = getFindDialogInstance();
        findDialogInstance->exec();
    }
    else
    {
        addLogToDockWidget(FIND_OP_CODE, QString(tr("未找到ip地址, Find操作异常终止")));
    }
}

//==================================================================
//function name: execInformationOperation
//author: lcy
//date: 2022-7-28
//function:作为响应界面触发的Information操作的槽函数,执行Information操作
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execInformationOperation()
{
    responseDevicesCnt = 0;
    threadsCnt = 0;
    threads.clear();
    //threads.erase(threads.begin(), threads.end());
    addLogToDockWidget(INFO_OP_CODE, "开始信息操作");
    focusOnCurrentOperation();
    mInformationWidget = std::make_shared<InformationWidget>(this);//初始化information界面
    timer = std::make_shared<QTimer>(this);
    //timer = new QTimer(this);
    qDebug() << "Information triggered!";
    for(int i = 0; i < deviceCnt; i++){
        mDevicesList.at(i)->setProgress(0);
    }
    for(int i = 0; i < deviceCnt; i++){
        if(mDevicesList.at(i)->checkedOrNot()){
            MyThread* thread = new InformationThread(mDevicesList.at(i)->getDevice(), new TftpRequest());
            threads[mDevicesList.at(i)->getDeviceIP()] = thread;
            //threads.append(thread);
            connect((InformationThread*)thread, &InformationThread::informationFinished, this, &MainWindow::finishInformation);
            connect((InformationThread*)thread, &InformationThread::informationFinished, mDevicesList.at(i), [=](File_LCL* LCL_struct){
               if(LCL_struct != NULL){
                   mDevicesList.at(i)->setProgress(100);
               }
               else{
                   mDevicesList.at(i)->setProgress(0);
                   mDevicesList.at(i)->setProgress(0, false);
               }
            });
            connect((InformationThread*)thread, &InformationThread::informationStatusMessage, this, [=](QString msg){
                this->addLogToDockWidget(INFO_OP_CODE, msg, mDevicesList.at(i)->getDevice()->getName() + ':' +
                                         mDevicesList.at(i)->getDeviceIP());
            });
            connect(this, &MainWindow::mainThreadExit, thread, &MyThread::mainThreadExited);
            pool.start(thread);
            thread->setAutoDelete(true);
        }
    }
    if(operationWidget){
        operationWidget->hide();
        layout->replaceWidget(operationWidget.get(), mInformationWidget.get());
    }
    operationWidget = mInformationWidget;
}

//==================================================================
//function name: execUploadOperation
//author: lcy
//date: 2022-7-28
//function:作为响应界面触发的Upload操作的槽函数,执行Upload操作
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execUploadOperation()
{
    threadsCnt = 0;
    qDebug() << "Upload triggered!";
    addLogToDockWidget(UPLOAD_OP_CODE, "开始上传操作");
    focusOnCurrentOperation();
    GET_DIR
    qDebug() << path;
    QStringList pathList = QFileDialog::getOpenFileNames(this, tr("选择上传文件"), path, "*.bin");
    if(pathList.size() > 0){
        UPDATE_DIR(pathList)
    }
    mUploadWidget = std::make_shared<UploadWidget>(&pool, threads, threadsCnt, &mDevicesList, pathList, this);
    //mUploadWidget->setWindowModality(Qt::ApplicationModal);
    //mUploadWidget->show();
    if(operationWidget){
        operationWidget->hide();
        layout->replaceWidget(operationWidget.get(), mUploadWidget.get());
    }
    operationWidget = mUploadWidget;
    connect(mUploadWidget.get(), &UploadWidget::uploadStatusMsg, this, [=](QString msg, QString fileName){
        this->addLogToDockWidget(UPLOAD_OP_CODE, msg, fileName);
    });
    connect(mUploadWidget.get(), &UploadWidget::uploadFinish, this, [=](){
        unfocusOnCurrentOperation();
        operationWidget->setEnabled(false);
    });
}

//==================================================================
//function name: execMediaDownloadOperation
//author: lcy
//date: 2022-7-28
//function:作为响应界面触发的MediaDownload操作的槽函数,执行MediaDownload操作
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execMediaDownloadOperation()
{
    threadsCnt = 0;
    addLogToDockWidget(MDOWN_OP_CODE, "开始介质定义下载");
    focusOnCurrentOperation();
    QString fileFolder = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), userProfile);
    mMDownloadWidget = std::make_shared<MDownloadWidget>(&pool, threads, threadsCnt, &mDevicesList, fileFolder, this);
    if(operationWidget){
        operationWidget->hide();
        layout->replaceWidget(operationWidget.get(), mMDownloadWidget.get());
    }
    operationWidget = mMDownloadWidget;
    connect(mMDownloadWidget.get(),
            &MDownloadWidget::sendLNSStatusDes,
            this, [=](QString message){
        addLogToDockWidget(MDOWN_OP_CODE, message);});
    connect(mMDownloadWidget.get(), &MDownloadWidget::MDownloadFinish, this, [=](){
        unfocusOnCurrentOperation();
        operationWidget->setEnabled(false);
    });
    qDebug() << "MediaDownload triggered!";
}

//==================================================================
//function name: execOperatorDownloadOperation
//author: lcy
//date: 2022-7-28
//function:作为响应界面触发的OperatorDownload操作的槽函数,执行OperatorDownload操作
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execOperatorDownloadOperation()
{
    threadsCnt = 0;
    addLogToDockWidget(ODOWN_OP_CODE, "开始用户定义下载");
    focusOnCurrentOperation();
    qDebug() << "OperatorDownload triggered!";
    mODownloadWidget = std::make_shared<ODownloadWidget>(&pool, threads, threadsCnt, &mDevicesList, this);
    //mODownloadWidget->setWindowModality(Qt::ApplicationModal);
    //mODownloadWidget->show();
    //operationWidget = mODownloadWidget;
    if(operationWidget){
        operationWidget->hide();
        layout->replaceWidget(operationWidget.get(), mODownloadWidget.get());
    }
    operationWidget = mODownloadWidget;
    connect(mODownloadWidget.get(),
            &ODownloadWidget::sendODLNSStatusDes,
            this, [=](QString message, QString deviceName){
        addLogToDockWidget(ODOWN_OP_CODE, message, deviceName);});
    connect(mODownloadWidget.get(), &ODownloadWidget::ODownloadFinish, this, [=](){
       unfocusOnCurrentOperation();
       operationWidget->setEnabled(false);
    });
    //arinc615_operator_download_op();
}

//==================================================================
//function name: execAbortOperation
//author: lcy
//date: 2022-7-28
//function:作为响应界面触发的Abort操作的槽函数,执行Abort操作
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execAbortOperation()
{
    qDebug() << "Abort triggered!";
}

void MainWindow::execParaConfigOperation()
{
    std::shared_ptr<ParaConfigDialog> paraConfigDialog = getParaConfigDialogInstance();
    connect(paraConfigDialog.get(), &ParaConfigDialog::sendParas, this, [=](size_t mfrt, size_t mtt, size_t wtms, size_t sfsi){
        max_find_response_time_ms = mfrt * 1000;
        max_retrans_times = mtt;
        wait_time_ms = wtms;
        state_file_send_interval = sfsi;
    });
    paraConfigDialog->exec();
    //paraConfigDialog->show();
}

//==================================================================
//function name: execExitTool
//author: lcy
//date: 2022-7-28
//function:关闭软件
//input arg: 无
//return:无
//log:7-28新增
//==================================================================
void MainWindow::execExitTool()
{
    this->close();
}

//==================================================================
//function name: execExitTool
//author: lcy
//date: 2022-8-1
//function:作为响应界面触发的全选操作的槽函数
//input arg: radio button是否被按下
//return:无
//log:7-28新增
//==================================================================
void MainWindow::selectAllDeviceOrNot(bool checked)
{
    //遍历设备列表中的设备
    int deviceNum = ui->deviceListWidget->count();

    if(deviceNum <= 0)
    {
        return;
    }

    for(int i = 0; i < deviceNum; i++)
    {
        //获取到list widget中的对象
        DeviceInfoWidget * devWidget = (DeviceInfoWidget * )ui->deviceListWidget->itemWidget(ui->deviceListWidget->item(i));

        //根据全选标志设置每个设备的标志
        devWidget->setRadioBtn(checked);

    }
}

//==================================================================
//function name: createDeviceList
//author: lcy
//date: 2022-8-1
//function:创建设备列表
//input arg: deviceNum: find到的设备数量
//return:返回创建成功与否的状态
//log:8-1新增
//==================================================================
int MainWindow::createDeviceWidget(const Device& device)
{    
    DeviceInfoWidget* deviceInfo = new DeviceInfoWidget(this, &device);
    this->mDevicesList << deviceInfo;
    connect(deviceInfo, &DeviceInfoWidget::radioBtnChecked, this, &MainWindow::on_radio_toggled);
    QListWidgetItem *widgetItem = new QListWidgetItem();
    widgetItem->setSizeHint(QSize(MAX_LIST_WIDGET_WIDTH, MAX_LIST_WIDGET_HEIGHT));
    ui->deviceListWidget->addItem(widgetItem);
    ui->deviceListWidget->setItemWidget(widgetItem, deviceInfo);

//    //初始化设备列表
//    for(int i = 0; i < devices->size(); i++)
//    {

//        DeviceInfoWidget *deviceInfo = new DeviceInfoWidget(this, &devices->at(i));
//        //填入相关信息

//        //存入vector
//        this->mDevicesList << deviceInfo;

//        connect(deviceInfo, &DeviceInfoWidget::radioBtnChecked, this, &MainWindow::on_radio_toggled);
//    }

//    //将设备列表加载到list widget
//    for(int i = 0; i < deviceNum; i++)
//    {
//        QListWidgetItem *widgetItem = new QListWidgetItem();
//        widgetItem->setSizeHint(QSize(MAX_LIST_WIDGET_WIDTH, MAX_LIST_WIDGET_HEIGHT));
//        ui->deviceListWidget->addItem(widgetItem);
//        ui->deviceListWidget->setItemWidget(widgetItem, this->mDevicesList.at(i));
//    }

    return FUNC_RETUEN_SUCCESS;
}



//==================================================================
//function name: clearDeviceList
//author: lcy
//date: 2022-8-1
//function:清空设备列表
//input arg:
//return:
//log:8-1新增
//==================================================================
void MainWindow::clearDeviceList()
{
    if(ui->deviceListWidget->count() != 0)
    {
        //delete 申请的内存空间
        for(int i = 0; i < this->mDevicesList.size(); i++)
        {
            delete this->mDevicesList.at(i);
            //delete ui->deviceListWidget->item(i);
        }
        this->mDevicesList.clear();
        //对QListWidget调用clear函数会自动释放QListWidget里面的内容，故不需要额外的delete
        ui->deviceListWidget->clear();
    }

}


//==================================================================
//function name: addLogToDockWidget
//author: lcy
//date: 2022-8-1
//function:写入日志信息
//input arg:    [in]operationCode: find到的设备数量
//              [in]log:详细操作信息
//return:
//log:8-1新增
//==================================================================
void MainWindow::addLogToDockWidget(const int &operationCode, const QString log, const QString deviceName)
{
    qDebug() << "deviceName = " << deviceName;
    QString currentTime;
    currentTime= QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString info;

    switch(operationCode)
    {
    case FIND_OP_CODE:
        info = QString("[%1]: %2").arg(FIND_OPERATION).arg(log);
        break;
    case INFO_OP_CODE:
        info = QString("[%1]: %2").arg(INFORMATION_OPERATION).arg(log);
        break;
    case UPLOAD_OP_CODE:
        info = QString("[%1]: %2").arg(UPLOAD_OPERATION).arg(log);
        break;
    case MDOWN_OP_CODE:
        info = QString("[%1]: %2").arg(MEDIA_DOWNLOAD_OPERATION).arg(log);
        break;
    case ODOWN_OP_CODE:
        info = QString("[%1]: %2").arg(OPERATOR_DOWNLOAD_OPERATION).arg(log);
        break;
    case AUTO_CONFIG_OP_CODE:
        info = QString("[%1]: %2").arg(AUTO_CONFIG_OPERATION).arg(log);
        break;
    case PARAMETER_CONFIG_OP_CODE:
        info = QString("[%1]: %2").arg(PARAMETER_CONFIG_OPERATION).arg(log);
        break;
    default:
        break;
    }
    if(deviceName != "default") ui->logTextBrowser->append(currentTime + " " + deviceName + "-" +  info);
    else ui->logTextBrowser->append(currentTime + " " + info);
}

void MainWindow::onDeviceFound(const QStringList &deviceInfo)
{
    if(deviceInfo.size() == 6){
        deviceCnt++;
        mFindInfoWidget->insertDeviceInfo(deviceInfo);
        Device* device = new Device(deviceInfo[0], deviceInfo[1], deviceInfo[2], deviceInfo[3], deviceInfo[4], deviceInfo[5]);
        devices.insert(device->getHostAddress(), device);
        //devices->append(device);
        createDeviceWidget(*device);
    }
    else{
        this->addLogToDockWidget(FIND_OP_CODE, QString("find响应报文格式有误"));
    }
}

void MainWindow::onRequestReceived(const QByteArray &tftpRequestDatagram, const QHostAddress& remote, const quint16 port)
{
    QString fileName = tftpRequestDatagram.mid(2).split('\0').at(0);
    MyThread* thread = threads.contains(remote.toString()) ? threads.value(remote.toString()) : nullptr;
    TftpRequest* tftpRequest = nullptr;
    if(fileName.endsWith(".LUS") || fileName.endsWith(".LNS")){
        tftpRequest = new TftpRequest();
        //MyThread* thread = threads.value(remote.toString());
        StatusFileRcvThread::StatusFileType fileType;
        if(fileName.endsWith(".LUS")) {
            fileType = StatusFileRcvThread::LUS;
        }
        else{
            fileType = StatusFileRcvThread::LNS;
        }
        tftpRequest->setRequestAndPort(tftpRequestDatagram, port);
        MyThread* statusFileRcvThread = new StatusFileRcvThread(fileType,
                                                                tftpRequest,
                                                                thread ? thread->getDevice() : devices.value(remote.toString()),
                                                                this);

        if(thread != nullptr){
            UploadThread* uploadThread = dynamic_cast<UploadThread*>(thread);
            ODownloadThread* oDownloadThread = dynamic_cast<ODownloadThread*>(thread);
            AutoConfigThread* autoConfigThread = dynamic_cast<AutoConfigThread*>(thread);
            if(uploadThread){
                connect((StatusFileRcvThread*)statusFileRcvThread, &StatusFileRcvThread::sendLUSInfSignal, uploadThread,
                        &UploadThread::rcvStatusCodeAndMessageSlot);
            }
            else if(oDownloadThread){
                connect((StatusFileRcvThread*)statusFileRcvThread, &StatusFileRcvThread::sendLNSInfSignal, oDownloadThread,
                        &ODownloadThread::rcvStatusCodeAndMessageSlot);
            }
            else if(autoConfigThread){
                if(fileType == StatusFileRcvThread::LUS){
                    connect((StatusFileRcvThread*)statusFileRcvThread, &StatusFileRcvThread::sendLUSInfSignal, autoConfigThread,
                            &AutoConfigThread::rcvLUSInfSlot);
                }
                else if(fileType == StatusFileRcvThread::LNS){
                    connect((StatusFileRcvThread*)statusFileRcvThread, &StatusFileRcvThread::sendLNSInfSignal, autoConfigThread,
                            &AutoConfigThread::rcvLNSInfSlot);
                }
            }
        }
        else{
           connect(static_cast<StatusFileRcvThread*>(statusFileRcvThread), &StatusFileRcvThread::statusMessageSignal, this, (void (MainWindow::*)(const int &operationCode, const QString log, const QString deviceName))&MainWindow::addLogToDockWidget);
        }
        pool.start(statusFileRcvThread);
        statusFileRcvThread->setAutoDelete(true);
    }
    else{
        tftpRequest = threads.value(remote.toString())->getTftpRequest();
        tftpRequest->setRequestAndPort(tftpRequestDatagram, port);
    }
//    TftpRequest* tftpRequest = nullptr;
//    QString fileName = tftpRequestDatagram.mid(2).split('\0').at(0);
//    if(threads.contains(remote.toString())){

//    }
//    else{
//        TftpRequest *tftpRequest = new TftpRequest();
//    }
}



void MainWindow::onTimerTimeout(const unsigned int steps){
    progressDialog->setValue(steps);
    if(steps > (unsigned int)progressDialog->maximum() || progressDialog->wasCanceled()){
        //disconnect(uSock, &QUdpSocket::readyRead, this, &MainWindow::parseFindResponse);
        emit(findThreadExitSignal());
        //timer->stop();
        //delete progressDialog;
        qDebug() << "find time off";
        //解除计时器信号槽并暂停计时器
        qDebug() << "disconnect success";
        //mFindInfoWidget->setWindowModality(Qt::ApplicationModal);
        //mFindInfoWidget->show();
        //日志显示
        addLogToDockWidget(FIND_OP_CODE, QString("查找到[%1]个设备").arg(deviceCnt));
        //createDeviceWidget(deviceCnt, devices);
        //绘制XML
#if 1
        a615_targets_find_list_t findList;
        findList.device_num = devices.size();
        findList.targets_info = (a615_target_find_info_t*)malloc(sizeof(a615_target_find_info_t) * devices.size());
        size_t i = 0;
        for(const auto &device : devices){
            strcpy(findList.targets_info[i].ip_addr, device->getHostAddress().toUtf8().data());
            strcpy(findList.targets_info[i].identifier, device->getName().toUtf8().data());
            strcpy(findList.targets_info[i].type_name, device->getHardwareType().toUtf8().data());
            strcpy(findList.targets_info[i].position, device->getPosition().toUtf8().data());
            strcpy(findList.targets_info[i].literal_name, device->getLiteralName().toUtf8().data());
            strcpy(findList.targets_info[i].manufacture_code, device->getManufactureCode().toUtf8().data());
        }
        for(uint i = 0; i < findList.device_num; i++){

        }
        if(CreateTopologyXML::createTopologyXMLFile("../topology.xml", findList, entry->ip().toString())){
            qDebug() << "XML写入完成";
        }
        free(findList.targets_info);
#else
        a615_targets_find_list_t findList;
        findList.device_num = 20;
        findList.targets_info = (a615_target_find_info_t*)malloc(sizeof(a615_target_find_info_t) * 20);
        for(uint i = 0; i < findList.device_num; i++){
            strcpy(findList.targets_info[i].ip_addr, QString("192.168.0.%1").arg(i + 1).toStdString().c_str());
            strcpy(findList.targets_info[i].identifier, "sad");
            strcpy(findList.targets_info[i].type_name, "de");
            strcpy(findList.targets_info[i].position, "ww");
            if(i % 2 == 0)
            {
                strcpy(findList.targets_info[i].literal_name, "adgsaifdESsafdea");
            }else
            {
                strcpy(findList.targets_info[i].literal_name, "xawdabufSWvorj");
            }

            strcpy(findList.targets_info[i].manufacture_code, "12");
        }
        if(CreateTopologyXML::createTopologyXMLFile("../topology.xml", findList, entry->ip().toString())){
            qDebug() << "XML写入完成";
        }
        free(findList.targets_info);
#endif
    }
}

//获取包含本地地址的所有
void MainWindow::getAllEntry(){
    this->entryList->clear();
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces()){
        if(!netInterface.flags().testFlag(QNetworkInterface::IsRunning)) continue;
        QList<QNetworkAddressEntry> entryList = netInterface.addressEntries();
        foreach(QNetworkAddressEntry entry, entryList){
            if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol
                    //&& !entry.ip().toString().startsWith("127.0.0.1")
                    && !entry.ip().toString().startsWith("169.254")){
                this->entryList->append(entry);
            }
        }
    }
}

void MainWindow::find(int index){
    clearDeviceList();
    ui->selectALLOrNotCheckBox->setChecked(false);
    deviceCnt = 0;
    EnableOrdisableExceptFind(false);
    if(devices.size() > 0){
        devices.clear();
        //mFindInfoWidget->clearTableWidget();
    }
//    else{
//        devices = new QList<Device>();
//    }
    checkedDevicesCnt = 0;
    checkedDevices->clear();
    entry = new QNetworkAddressEntry(entryList->at(index));
    mFindInfoWidget = std::make_shared<FindInfoWidget>(this);
    if(operationWidget) {
        layout->replaceWidget(operationWidget.get(), mFindInfoWidget.get());
        operationWidget->hide();
    }
    else{
        layout->addWidget(mFindInfoWidget.get());
    }
    operationWidget = mFindInfoWidget;
    progressDialog = std::shared_ptr<QProgressDialog>(new QProgressDialog(tr("发现操作进度"), tr("取消"), 0, 100, this));
    progressDialog->setWindowTitle(tr("设备发现进度"));
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->show();
    //if(progressDialog != nullptr) delete progressDialog;

    //mFindInfoWidget = new FindInfoWidget(this); //初始化Find界面
    qDebug() << "find triggered!";
    addLogToDockWidget(FIND_OP_CODE, "开始查找设备");
    //构造FIND请求报文
    QByteArray datagram = makeFindRequest();
    //发送FIND请求报文
    findThread = std::shared_ptr<FindThread>(new FindThread(entry));
    connect(findThread.get(), &FindThread::deviceFoundSignal, this, &MainWindow::onDeviceFound);
    connect(findThread.get(), &FindThread::timerTimeOutSignal, this, &MainWindow::onTimerTimeout);
    connect(this, &MainWindow::findThreadExitSignal, findThread.get(), &FindThread::onRcvThreadExitSig);
    findThread->start();
//    uSock = new QUdpSocket(this);
//    uSock->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress);
//    uSock->writeDatagram(datagram.data(), datagram.size(), entry->broadcast(), 1001);
//    //在规定时间内接收FIND响应包
//    //connect(uSock, &QUdpSocket::readyRead, this, &MainWindow::parseFindResponse);
//    connect(timer.get(), &QTimer::timeout, this, &MainWindow::onTimerTimeout);
//    timer->start(max_find_response_time_ms / 100);
}

QByteArray MainWindow::makeFindRequest(){
    find_req req;   //find请求
    memset(&req, 0, sizeof(req));
    req.opcode = 256;
    req.ASCII_String = '\0';
    req.Packet_Terminator = 0x10;
    req.tftpRetryNum = max_retrans_times;
    req.tftpTimeout = wait_time_ms;
    req.lusPeriod = state_file_send_interval;
    QByteArray datagram;
    appendBigEndian(&req.opcode, sizeof(req.opcode), datagram);
    datagram.append(req.ASCII_String).append(req.Packet_Terminator);
    appendBigEndian(&req.tftpRetryNum, sizeof(req.tftpRetryNum), datagram);
    appendBigEndian(&req.tftpTimeout, sizeof(req.tftpTimeout), datagram);
    appendBigEndian(&req.lusPeriod, sizeof(req.lusPeriod), datagram);
    return datagram;
}

void MainWindow::waitForAllWorkingThreadsDone()
{
//    waitThread = std::make_shared<WaitThread>(pool);
//    waitThread->start();
    pool.waitForDone();
}

FindDialog *MainWindow::getFindDialogInstance()
{
    if(findDialog != 0){
        getAllEntry();
        findDialog->updateEntryList(entryList);
        return findDialog;
    }
    else{
        findDialog = new FindDialog(this, entryList);
        connect(findDialog, &FindDialog::on_accpetBtn_clicked, this, &MainWindow::find);
        connect(findDialog, &FindDialog::rejected, this, [=](){
            addLogToDockWidget(FIND_OP_CODE, "未选择IP,FIND操作失败");
        });
        return findDialog;
    }

}

std::shared_ptr<ParaConfigDialog> MainWindow::getParaConfigDialogInstance()
{
    if(paraConfigDialog == nullptr){
        paraConfigDialog = std::make_shared<ParaConfigDialog>(this);
        connect(paraConfigDialog.get(), &ParaConfigDialog::sendLog, this, [=](QString log){
           this->addLogToDockWidget(PARAMETER_CONFIG_OP_CODE, log);
        });
    }
    return paraConfigDialog;
}

void MainWindow::EnableOrdisableExceptFind(bool flag){
    ui->actionInformationOp->setEnabled(flag);
    ui->actionUploadOp->setEnabled(flag);
    ui->actionMediaDownloadOp->setEnabled(flag);
    ui->actionOperatorDownloadOp->setEnabled(flag);
    ui->actionAutoConfigOp->setEnabled(flag);
    ui->uploadPushButton->setEnabled(flag);
    ui->mediaDownloadPushBtn->setEnabled(flag);
    ui->operatorDownloadPushBtn->setEnabled(flag);
    ui->informationPushBtn->setEnabled(flag);
    ui->autoConfigBtn->setEnabled(flag);
}

void MainWindow::on_radio_toggled(bool checked, const Device* device){
    //ui->selectALLOrNotCheckBox->blockSignals(true);
    if(checked) {
        checkedDevicesCnt++;
        if(checkedDevicesCnt == deviceCnt) this->ui->selectALLOrNotCheckBox->setChecked(true);
        checkedDevices->append(device);
    }
    else {
        checkedDevicesCnt--;
        this->ui->selectALLOrNotCheckBox->setChecked(false);
        checkedDevices->removeOne(device);
    }
    qDebug() << "checkedDevicesCnt" << checkedDevicesCnt;
    checkIfAnyDeviceSelect();
    //ui->selectALLOrNotCheckBox->blockSignals(false);
}

void MainWindow::checkIfAnyDeviceSelect(){
    if(checkedDevicesCnt > 0) EnableOrdisableExceptFind(true);
    else EnableOrdisableExceptFind(false);
}

void MainWindow::execAutoConfigOperation()
{
    mAutoConfigWidget = std::make_shared<AutoConfigWidget>(&pool, threads, threadsCnt, checkedDevices, &mDevicesList, this);
    connect(mAutoConfigWidget.get(), &AutoConfigWidget::sendAutoConfigStatusMessage, this, [=](QString msg, QString deviceName){
        this->addLogToDockWidget(AUTO_CONFIG_OP_CODE, msg, deviceName);
    });
    addLogToDockWidget(AUTO_CONFIG_OP_CODE, QString(tr("开始自动化配置操作")));
    focusOnCurrentOperation();
    if(operationWidget){
        operationWidget->hide();
        layout->replaceWidget(operationWidget.get(), mAutoConfigWidget.get());
    }
    connect(mAutoConfigWidget.get(), &AutoConfigWidget::AutoConfigFinish, this, [=](){
        operationWidget->setEnabled(false);
        unfocusOnCurrentOperation();
    });
    operationWidget = mAutoConfigWidget;
}

void MainWindow::tftpServerTftpReadReady()
{

}

void MainWindow::finishInformation(File_LCL* LCL_struct, QString name, QString ip)
{
    if(LCL_struct){
        mInformationWidget->setTargetInfo(*LCL_struct, name, ip);
        //free动态分配的内存
        for(int i = 0; i < LCL_struct->Hw_num; i++){
            free(LCL_struct->Hws[i].parts);
        }
        free(LCL_struct->Hws);
        free(LCL_struct);
        addLogToDockWidget(INFO_OP_CODE,
                           QString(tr("信息操作完成%1/%2")).arg(++responseDevicesCnt).arg(checkedDevicesCnt),
                           name + ':' + ip);
    }
    else{
        ++responseDevicesCnt;
    }
    if(responseDevicesCnt == checkedDevicesCnt){
        unfocusOnCurrentOperation();
    }
}

void MainWindow::enableAllExceptCurrentOperation(bool flag){

    ui->deviceListWidget->setEnabled(flag);
    EnableOrdisableExceptFind(flag);
    ui->selectALLOrNotCheckBox->setEnabled(flag);
}


void MainWindow::focusOnCurrentOperation(){
    ui->deviceListWidget->setEnabled(false);
    EnableOrdisableExceptFind(false);
    ui->actionFindOp->setEnabled(false);
    ui->selectALLOrNotCheckBox->setEnabled(false);
}

void MainWindow::unfocusOnCurrentOperation()
{
    qDebug() << "threads.size =" << threads.size();
    qDebug() << "pool.activeThreadCount =" << pool.activeThreadCount();

    qDebug() << "pool.activeThreadCount =" << pool.activeThreadCount();
    ui->deviceListWidget->setEnabled(true);
    EnableOrdisableExceptFind(true);
    ui->actionFindOp->setEnabled(true);
    ui->selectALLOrNotCheckBox->setEnabled(true);
}

void MainWindow::appendBigEndian(void *arg, size_t size, QByteArray &byteArray)
{
    for(int i = size - 1; i > -1; i--){
        byteArray.append(*((char*)arg + i));
    }
}

void MainWindow::saveLog()
{
    QString log = ui->logTextBrowser->toPlainText();
    QString currentTime= QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm");
    QFile logFile(QString("logFile(%1).txt").arg(currentTime));
    if(logFile.open(QIODevice::ReadWrite | QIODevice::Append)){
        QTextStream textStream(&logFile);
        textStream << log;
        logFile.close();
    }
}



