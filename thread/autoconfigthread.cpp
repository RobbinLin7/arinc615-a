#include "autoconfigthread.h"
#include "mythread.h"

void AutoConfigThread::run(){
    qDebug() << "AutoConfigThread::run";
    this->tftpClient = new QUdpSocket();
    this->tftpServer = new QUdpSocket();

    this->tftpClient->connectToHost(device->getHostAddress(), 69);
    connect((UploadThread*)uploadThread, &UploadThread::uploadResult, this, [=](bool result){
        if(result) uploadComplete = 1;
        else uploadComplete = -1;
    }, Qt::QueuedConnection);
    connect((UploadThread*)uploadThread, &UploadThread::uploadStatusMessage, this, [=](QString message){
        emit(autoConfigStatusMessage(message));
    });
    connect(this, &AutoConfigThread::sendLUSInfSignal, (UploadThread*)uploadThread, &UploadThread::rcvStatusCodeAndMessageSlot);
    //connect((uploadThread*)uploadThread, &UploadThread::)
    //uploadThread->setAutoDelete(true);
    //uploadThread->run();
    uploadThread->setAutoDelete(true);
    pool->start(uploadThread);
    this->tftpRequest->lockMutex();
    while(uploadComplete == 0 && !mainThreadExitedOrNot){
        QThread::msleep(200);
    }
    if(mainThreadExitedOrNot){
        emit(threadFinish(true, QString(tr("主线程已退出"))));
        return;
    }
    if(uploadComplete != 1){
        emit(autoConfigStatusMessage(QString("上传操作出现异常，自动化操作异常退出")));
        emit(autoConfigRate(0, false));
        emit(threadFinish(true, QString(tr("自动化操作异常结束"))));
        return;
    }
    else{
        emit(autoConfigStatusMessage(QString("自动化操作-上传操作完成")));
        emit(autoConfigRate(33, true));
        startDownload();
    }
}

void AutoConfigThread::startDownload()
{
    //uploadThread->disconnect();
    connect((ODownloadThread*)oDownloadThread, &ODownloadThread::threadFinish, this, [=](bool result){
        if(result)  downloadComplete = 1;
        else downloadComplete = -1;
    }, Qt::QueuedConnection);
    connect(this, &AutoConfigThread::sendLNSInfSignal, (ODownloadThread*)oDownloadThread, &ODownloadThread::rcvStatusCodeAndMessageSlot);
    connect((ODownloadThread*)oDownloadThread, &ODownloadThread::oDownloadStatusMessage, this, [=](QString message){
        emit(autoConfigStatusMessage(message));
    });
    oDownloadThread->setAutoDelete(true);
    pool->start(oDownloadThread);
    while(downloadComplete == 0 && !mainThreadExitedOrNot){
        QThread::msleep(200);
    }
    //oDownloadThread->disconnect();
    if(mainThreadExitedOrNot){
        emit(threadFinish(true, QString(tr("主线程已退出"))));
        return;
    }
    if(downloadComplete != 1){
        emit(autoConfigStatusMessage(QString("下载操作出现异常，自动化操作异常退出")));
        emit(threadFinish(true, QString(tr("自动化操作异常结束"))));
        emit(autoConfigRate(0, false));
        return;
    }
    else{
        emit(autoConfigStatusMessage(QString("自动化操作-下载操作完成")));
        emit(autoConfigRate(66, true));
        startCompareMd5();
        emit(autoConfigRate(100, true));
        emit(threadFinish(true, QString(tr("自动化操作正常结束"))));
    }
}

void AutoConfigThread::startCompareMd5()
{
    QStringList errroFiles;
    for(int i = 0; i < filesPath.size(); i++){
        QFile file1(filesPath.at(i));
        QFile file2(dir.dirName() + '/' + filesName.at(i));
        if(file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly)){
            QCryptographicHash hash1(QCryptographicHash::Md5), hash2(QCryptographicHash::Md5);
            hash1.reset();
            hash2.reset();
            hash1.addData(&file1);
            hash2.addData(file2.read(file1.size()));
            if(hash1.result() != hash2.result()){
//                qDebug() << dir.dirName() + '/' + filesName.at(i) << filesPath.at(i) << "上传无误";
//                emit(autoConfigStatusMessage(QString("自动化操作-MD5验证无误")));
                emit(autoConfigStatusMessage(QString("自动化操作-文件%1 校验有误").arg(filesName.at(i))));
                errroFiles.append(filesName.at(i));
            }
            else
            {
                if(file1.size() == file2.size())
                {
                    emit(autoConfigStatusMessage(QString("自动化操作-文件%1 校验无误").arg(filesName.at(i))));
                }
                else
                {
                    emit(autoConfigStatusMessage(QString("自动化操作-上传下载的文件大小不一致，只针对前%1字节进行对比且对比一致").arg(file1.size())));
                }
            }
        }
        file1.close();
        file2.close();
    }

    emit(autoConfigStatusMessage(QString(tr("自动化操作完成"))));
}

void AutoConfigThread::rcvLUSInfSlot(quint16 statusCode, QString statusMessage, bool error, QString errorMessage)
{
    emit(sendLUSInfSignal(statusCode, statusMessage, error, errorMessage));
}

void AutoConfigThread::rcvLNSInfSlot(quint16 statusCode, quint16 totalFileNum, QString statusMessage, bool error, QString errorMessage)
{
    emit(sendLNSInfSignal(statusCode, totalFileNum, statusMessage, error, errorMessage));
}



