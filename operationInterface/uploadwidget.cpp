#include "uploadwidget.h"
#include "ui_uploadwidget.h"
#include "ui_fileinfowidget.h"
#include "progressdialog.h"
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QErrorMessage>
#include "globalDefine.h"

UploadWidget::UploadWidget(QThreadPool *pool, QList<MyThread*>& threads, unsigned int &threadsCnt, QVector<DeviceInfoWidget *> *deviceList, QStringList files, QWidget *parent):
    QWidget(parent),
    ui(new Ui::UploadWidget),
    threadsCnt(threadsCnt),
    pool(pool),
    threads(threads),
    deviceList(deviceList)
{
    ui->setupUi(this);
    this->parent = parent;
    threads.erase(threads.begin(), threads.end());

    this->files = files;
    this->numFilesSelected = 0;
    this->threadsCnt = threadsCnt;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("开始上传"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, [=](){
        progressDialog = new ProgressDialog(fileSelected);
        progressDialog->setWindowTitle(QString("上传进度"));
        progressDialog->show();
        beginUpload();
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, [=](){
        emit(uploadStatusMsg(QString(tr("退出上传操作")), "default"));
        emit(uploadFinish());
        this->close();
    });
    for(int i = 0; i < deviceList->size(); i++){
        deviceList->at(i)->setProgress(0);
    }
    addFiles(files);
}

UploadWidget::~UploadWidget()
{
    delete ui;
    for(int i = 0; i < widgetItemList.size(); i++){
        delete widgetItemList.at(i);
    }
}

void UploadWidget::addFiles(QStringList files){
    this->files.append(files);
    for(int i = 0; i < files.size(); i++){
        QFile* file = new QFile(files.at(i), this);
        FileInfoWidget *fileInfoWidget = new FileInfoWidget(file, this);
        mFilesList << fileInfoWidget;
        QListWidgetItem *widgetItem = new QListWidgetItem();
        widgetItemList.append(widgetItem);
        ui->listWidget->addItem(widgetItem);
        widgetItem->setSizeHint(QSize(600, 70));
        ui->listWidget->setItemWidget(widgetItem, fileInfoWidget);
        connect(fileInfoWidget, &FileInfoWidget::selectRadioChecked, this, &UploadWidget::radioChecked);
    }
}

bool UploadWidget::beginUpload()
{
    //threadsCnt = 0;
    this->setEnabled(false);

    for(int i = 0; i < deviceList->size(); i++){
        if(deviceList->at(i)->checkedOrNot()){
            MyThread* thread = new UploadThread(fileSelected, deviceList->at(i)->getDevice(), new TftpRequest());
            threads.append(thread);
            //threads[threadsCnt] = new UploadThread(fileSelected, deviceList->at(i)->getDevice(), new TftpRequest(), this);
            //这里可以连接信号槽
            const Device *device = deviceList->at(i)->getDevice();
            QString name = device->getName();
            deviceList->at(i)->setProgress(0);
            //void (DeviceInfoWidget::*setProgress1)(const int&) = &DeviceInfoWidget::setProgress;
            //connect((UploadThread*)thread, &UploadThread::uploadRate, deviceList->at(i), (newSetProgressType)&DeviceInfoWidget::setProgress);
            connect((UploadThread*)thread, &UploadThread::uploadStatusMessage, this, [=](QString message){
                emit(uploadStatusMsg(message, deviceList->at(i)->getDevice()->getName() + ':' +
                                     deviceList->at(i)->getDeviceIP()));
            });
            connect((UploadThread*)thread, &UploadThread::threadFinish, this, [=](){
                if(++finishedThreadCnt == (unsigned int)threads.size()){
                    emit(uploadFinish());
                }
            });
            connect((MainWindow*)parent, &MainWindow::mainThreadExit, thread, &MyThread::mainThreadExited);
            pool->start(thread);
            thread->setAutoDelete(true);
        }
    }
    return true;
}

void UploadWidget::radioChecked(bool checked, QString fileName){
    ui->checkBox->blockSignals(true);
    if(checked){
        numFilesSelected++;
        if(numFilesSelected == ui->listWidget->count()){
            ui->checkBox->setChecked(true);
        }
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        fileSelected.append(fileName);
    }
    else{
        ui->checkBox->setChecked(false);
        numFilesSelected--;
        if(numFilesSelected == 0) ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        fileSelected.removeOne(fileName);
    }
    ui->checkBox->blockSignals(false);
}




void UploadWidget::on_pushButton_clicked()
{
    GET_DIR
    QStringList pathList = QFileDialog::getOpenFileNames(this, tr("选择上传文件"), path, "*.bin");
    if(pathList.size() > 0){
        addFiles(pathList);
        UPDATE_DIR(pathList)
    }
}


void UploadWidget::on_pushButton_2_clicked()
{
    int i = 0;
    for(; i < ui->listWidget->count(); i++){
        FileInfoWidget* widget = (FileInfoWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        if(widget->getChecked()){
            QString fileAbsolutePath = widget->getFileAbsolutePath();
            files.removeOne(fileAbsolutePath);
            if(fileSelected.contains(fileAbsolutePath)){
                fileSelected.removeOne(fileAbsolutePath);
            }
            ui->listWidget->takeItem(i);
            i = -1;
        }
    }
}


void UploadWidget::on_checkBox_toggled(bool checked)
{
    for(int i = 0; i < ui->listWidget->count(); i++){
        FileInfoWidget* widget = (FileInfoWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        widget->setSelectRadio(checked);
    }
}

void UploadWidget::on_LUS_received(File_LUS *LUS)
{
    int progress = 0;
    for(int i = 0; i < 3; i++){
        progress = progress * 10 + LUS->load_list_ratio[i] - '0';
    }
    deviceList->at(0)->setProgress(progress);
    if(LUS->hfile_num > 0){
        progressDialog->setProgress(LUS->hfiles, LUS->hfile_num);
    }
    free(LUS);
}

