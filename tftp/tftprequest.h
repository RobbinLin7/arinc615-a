#ifndef TFTPREQUEST_H
#define TFTPREQUEST_H

#include <QObject>
#include <QMutex>
#include "tftp.h"

class TftpRequest
{
public:
    TftpRequest(){
        mutex.lock();
    }
    QByteArray getRequest(){
        QByteArray request;
        return request;
    }
    QByteArray getRequest(bool* mainThreadExitOrNot){
        unsigned int wait_times = 0;
        QByteArray tRequest;
        if(*mainThreadExitOrNot) return tRequest;
        bool flag = true;
        if(!mutex.tryLock(WAIT_TIME_MS)){
#ifdef  QT_DEBUG
            qDebug() << "等了一次";
#endif
            ++wait_times;
            while(!(*mainThreadExitOrNot) && !mutex.tryLock(WAIT_TIME_MS) && wait_times++ < 10){
#ifdef  QT_DEBUG
                qDebug() << "又等了一次";
#endif
            }
            if(*mainThreadExitOrNot) return tRequest;
            if(wait_times == 11) flag = false;
        }
        if(flag){
            tRequest = this->request;
            mutex.unlock();
        }
        return tRequest;
//        while(!mutex.tryLock(WAIT_TIME_MS))
//        if(mutex.tryLock(WAIT_TIME_MS * 10)){
//            request = this->request;
//            mutex.unlock();
//        }
//        //QMutexLocker locker(&mutex);
//        return request;
    }
    quint16 getPort(){
        QMutexLocker locker(&mutex);
        return port;
    }
    void setRequestAndPort(QByteArray request, quint16 port){
        this->request = request;
        this->port = port;
        mutex.tryLock();
        mutex.unlock();
    }

    void lockMutex(){
        //mutex.lock();
        mutex.tryLock();
    }
private:
    QByteArray request;
    quint16 port;
    mutable QMutex mutex;
};

#endif // TFTPREQUEST_H
