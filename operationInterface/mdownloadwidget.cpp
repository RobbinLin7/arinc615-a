#include "mdownloadwidget.h"
#include "ui_mdownloadwidget.h"
#include "ui_fileinfowidget.h"
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

MDownloadWidget::MDownloadWidget(QThreadPool *pool, QList<MyThread*>& threads, unsigned int &threadsCnt, QVector<DeviceInfoWidget *> *deviceList, QString folderPath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MDownloadWidget),
    threads(threads),
    threadsCnt(threadsCnt)

{
    threads.erase(threads.begin(), threads.end());
    ui->setupUi(this);
    this->folderPath = folderPath;
    this->deviceList = deviceList;
    this->pool = pool;
    this->parent = parent;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("开始下载"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
    loadFolderFiles(folderPath);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &MDownloadWidget::beginMDownload);
    connect(ui->checkBox, &QCheckBox::toggled, this, &MDownloadWidget::selectAllFilesOrNot);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, [=](){
        emit(sendLNSStatusDes(QString(tr("退出介质定义下载")), "default"));
        emit(MDownloadFinish());
        this->close();
    });
    for(int i = 0; i < deviceList->size(); i++){
        deviceList->at(i)->setProgress(0);
    }
}

MDownloadWidget::~MDownloadWidget()
{
    delete ui;
    //threads.erase(threads.begin(), threads.end());
}

void MDownloadWidget::loadFolderFiles(const QString &path)
{
    ui->listWidget->clear();
    this->mFilesList.clear();
    QDir dir(path);
    QFileInfoList filesInfo = dir.entryInfoList();
    for(int i = 0; i < filesInfo.size(); i++){
        if(filesInfo.at(i).isDir() || filesInfo.at(i).fileName() == ".." || filesInfo.at(i).fileName() == "."){
            continue;
        }
        qDebug() << filesInfo.at(i).fileName() << filesInfo.at(i).size();
        QFile *file = new QFile(filesInfo.at(i).absoluteFilePath(), this);
        FileInfoWidget *fileInfoWidget = new FileInfoWidget(file, this);
        mFilesList << fileInfoWidget;
        QListWidgetItem *widgetItem = new QListWidgetItem();
        ui->listWidget->addItem(widgetItem);
        widgetItem->setSizeHint(QSize(600, 70));
        ui->listWidget->setItemWidget(widgetItem, fileInfoWidget);
        connect(fileInfoWidget->ui->radioButton, &QRadioButton::toggled, this, &MDownloadWidget::radioChecked);
    }
}

void MDownloadWidget::beginMDownload()
{
    this->setEnabled(false);
    threadsCnt = 0;
    checkedFileList.clear();
    for(int i = 0; i < mFilesList.size(); i++){
        if(mFilesList.at(i)->getChecked()) checkedFileList.append(mFilesList.at(i)->getFileName());
    }
    for(int i = 0; i < deviceList->size(); i++){

        if(deviceList->at(i)->checkedOrNot()){
            MyThread* thread = new MDownloadThread(checkedFileList,
                                                   deviceList->at(i)->getDevice(),
                                                   new TftpRequest());
            threads.append(thread);
            deviceList->at(i)->setProgress(0);
            connect((MDownloadThread*)thread,
                    &MDownloadThread::mDownloadStatusMessage,
                    this, [=](QString msg){
                this->transmitLNSDes(msg, deviceList->at(i)->getDevice()->getName() + ':' +
                                     deviceList->at(i)->getDeviceIP());
            });
            connect((MDownloadThread*)thread,
                    &MDownloadThread::mDownloadRate,
                    deviceList->at(i), (newSetProgressType)&DeviceInfoWidget::setProgress);
            connect((MDownloadThread*)thread, &MDownloadThread::threadFinish, this, [=](){
                if(++finishedThreadCnt == threads.size()){
                    emit(MDownloadFinish());
                }
            });
            connect((MainWindow*)this->parent, &MainWindow::mainThreadExit, thread, &MyThread::mainThreadExited);
            pool->start(thread);
            thread->setAutoDelete(true);
            break;
        }
    }
    //this->close();
}

void MDownloadWidget::selectAllFilesOrNot(bool checked){
    for(int i = 0; i < ui->listWidget->count(); i++){
        FileInfoWidget* widget = (FileInfoWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        widget->setSelectRadio(checked);
    }
}

void MDownloadWidget::radioChecked(bool checked){
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


void MDownloadWidget::on_selectFloderBtn_clicked()
{
    QString fileFolder = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), tr("./files"));

    if(!fileFolder.isEmpty())
    {
        loadFolderFiles(fileFolder);
    }
    else
    {
        QMessageBox::warning(this, "请选择", "请选择文件夹!");
    }

}

void MDownloadWidget::transmitLNSDes(QString msg, QString deviceName)
{
    emit sendLNSStatusDes(msg, deviceName);
}

