#ifndef TFTPSERVERTHREAD_H
#define TFTPSERVERTHREAD_H

#include <QThread>
#include <QUdpSocket>
#include <QThreadPool>
#include "tftp/tftprequest.h"

class TftpServerThread : public QThread
{
    Q_OBJECT
public:
    explicit TftpServerThread(QObject *parent = nullptr);

    // QThread interface
protected:
    void run();
private:
    QUdpSocket* tftpServer = nullptr;

signals:
    void requestReceivedSignal(const QByteArray& tftpRequest, const QHostAddress& remote, const quint16 port);

private slots:
    void onTftpServerReadyRead();
public slots:
    void onRcvThreadExitSig();
};

#endif // TFTPSERVERTHREAD_H
