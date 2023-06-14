#include "odownloadwidget.h"
#include "ui_odownloadwidget.h"
#include "ui_fileinfowidget.h"
#include<QPushButton>

ODownloadWidget::ODownloadWidget(QThreadPool *pool, QList<MyThread*>& threads, unsigned int& threadsCnt, QVector<DeviceInfoWidget *> *deviceList, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ODownloadWidget),
    threads(threads),
    threadsCnt(threadsCnt)
{
    threads.erase(threads.begin(), threads.end());
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("开始下载"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    this->deviceList = deviceList;
    this->pool = pool;
    this->parent = parent;
    for(int i = 0; i < deviceList->size(); i++){
        if(deviceList->at(i)->checkedOrNot()){
            device = deviceList->at(i)->getDevice();
            qRegisterMetaType<QList<QPair<QString, QString>>*>("QList<QPair<QString, QString>>*");
            MyThread* thread = new ODownloadThread(new TftpRequest(), device, QStringList());
            threads.append(thread);
            //threads[threadsCnt] = new ODownloadThread(new TftpRequest(), device, QStringList(), this);
            connect(this, &ODownloadWidget::filesChecked, (ODownloadThread*)thread, &ODownloadThread::receiveCheckedFiles);
            connect((ODownloadThread*)thread, &ODownloadThread::sendFileList, this, &ODownloadWidget::receiveFileList);
            connect((ODownloadThread*)thread, &ODownloadThread::oDownloadStatusMessage, this, [=](QString msg){
                this->transmitODLNSDes(msg, deviceList->at(i)->getDevice()->getName());
            });
            deviceList->at(i)->setProgress(0);
            connect((ODownloadThread*)thread, &ODownloadThread::oDownloadRate, deviceList->at(i), (newSetProgressType)&DeviceInfoWidget::setProgress);
            connect((ODownloadThread*)thread, &ODownloadThread::threadFinish, this, [=](){
                if(++finishedThreadCnt == threads.size()){
                    emit(ODownloadFinish());
                }
            });
            connect((MainWindow*)this->parent, &MainWindow::mainThreadExit, thread, &MyThread::mainThreadExited);
            pool->start(thread);
            thread->setAutoDelete(true);
            break;
        }
    }
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &ODownloadWidget::downloadBtnClikcked);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, [=](){
        emit(sendODLNSStatusDes(QString(tr("退出用户定义下载")), "default"));
        emit(ODownloadFinish());
        this->close();
    });
    connect(ui->checkBox, &QCheckBox::toggled, this, &ODownloadWidget::selectAllFilesOrNot);
}

ODownloadWidget::~ODownloadWidget()
{
    delete ui;
    //threads.erase(threads.begin(), threads.end());
}

void ODownloadWidget::transmitODLNSDes(QString msg, QString deviceName)
{
    emit sendODLNSStatusDes(msg, deviceName);
}

void ODownloadWidget::receiveFileList(QList<QPair<QString, QString> > *fileList)
{
    for(int i = 0; i < fileList->size(); i++){
        FileInfoWidget *fileInfoWidget = new FileInfoWidget(fileList->value(i).first,fileList->value(i).second, this);
        mFilesList << fileInfoWidget;
        QListWidgetItem *widgetItem = new QListWidgetItem();
        ui->listWidget->addItem(widgetItem);
        widgetItem->setSizeHint(QSize(600, 70));
        ui->listWidget->setItemWidget(widgetItem, fileInfoWidget);
        connect(fileInfoWidget->ui->radioButton, &QRadioButton::toggled, this, &ODownloadWidget::radioChecked);
    }
}

void ODownloadWidget::downloadBtnClikcked()
{
    this->setEnabled(false);
    QStringList fileList;
    for(int i = 0; i < mFilesList.size(); i++){
        if(mFilesList.at(i)->getChecked()) fileList.append(mFilesList.at(i)->getFileName());
    }
    emit(filesChecked(fileList));
    //this->close();
}

void ODownloadWidget::selectAllFilesOrNot(bool checked){
    for(int i = 0; i < ui->listWidget->count(); i++){
        FileInfoWidget* widget = (FileInfoWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        widget->setSelectRadio(checked);
    }
}

void ODownloadWidget::radioChecked(bool checked){
    ui->checkBox->blockSignals(true);
    if(checked){
        numFilesSelected++;
        if(numFilesSelected == ui->listWidget->count()){
            ui->checkBox->setChecked(true);
        }
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else{
        ui->checkBox->setChecked(false);
        numFilesSelected--;
        if(numFilesSelected == 0) ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    ui->checkBox->blockSignals(false);
}
