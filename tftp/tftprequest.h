#ifndef TFTPREQUEST_H
#define TFTPREQUEST_H

#include <QObject>
#include <QMutex>
#include "tftp.h"
#include "dir.h"

class TftpRequest
{
public:
    TftpRequest(){
        mutex.lock();
    }
    ~TftpRequest(){
        mutex.unlock();
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
        if(!mutex.tryLock(wait_time_ms)){
            ++wait_times;
            while(!(*mainThreadExitOrNot) && !mutex.tryLock(wait_time_ms) && wait_times++ < max_retrans_times)
            {

            }
            if(*mainThreadExitOrNot) return tRequest;
            if(wait_times == max_retrans_times + 1) flag = false;
        }
        if(flag){
            tRequest = this->request;
            mutex.unlock();
        }
        return tRequest;
    }
    quint16 getPort(){
        QMutexLocker locker(&mutex);
        return port;
    }
    void setRequestAndPort(QByteArray request, quint16 port){
        this->request = request;
        this->port = port;
        //if()
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
