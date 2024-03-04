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
        return request;
    }
    quint16 getPort(){
        return port;
    }
    void setRequestAndPort(QByteArray request, quint16 port){
        this->request = request;
        this->port = port;
    }
private:
    QByteArray request;
    quint16 port;
public:
    mutable QMutex mutex;
};

#endif // TFTPREQUEST_H
