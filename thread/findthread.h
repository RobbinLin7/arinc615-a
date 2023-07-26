#ifndef FINDTHREAD_H
#define FINDTHREAD_H
#include <QNetworkAddressEntry>
#include <QThread>
#include <QUdpSocket>
#include <QTimer>
#include "protocal/findRequest.h"
#include "dir.h"

class FindThread: public QThread
{
    Q_OBJECT
public:
    explicit FindThread(QNetworkAddressEntry* entry = nullptr, QObject *parent = nullptr):QThread(parent)
    {
        this->entry = entry;
        this->moveToThread(this);
        qDebug() << QThread::currentThreadId() << "FindThread constructor";
    }

    // QRunnable interface
public:
    void run();
    QByteArray makeFindRequest();
    void appendBigEndian(void *arg, size_t size, QByteArray &byteArray);
private:
    //这里的属性全是主线程所有，只有run方法里面创建的对象才是该线程所有
    QUdpSocket* uSock = nullptr;
    QNetworkAddressEntry* entry = nullptr;

    unsigned int steps = 0;

signals:
    void deviceFoundSignal(const QStringList& deviceInfo);
    //对内置类型而言(也就是C-like)类型，pass-by-value通常比pass-by-reference高效(pass-by-value可以利用CPU的缓存局部性原理，因为它处理的是连续的内存块。--chatgpt)
    //考虑了一下，对于多线程而言，最好还是用pass-by-reference吧。因为没有加同步机制，如果该值为一个局部变量，可能槽函数还没用，该变量作用域就已经结束了(比如局部变量)。???
    //Qt的信号和槽机制会在跨线程时进行数据的拷贝或移动，而不是直接传递引用。这是为了确保在目标线程中能够安全地访问参数。因此，如果参数是引用类型，Qt 会在内部进行拷贝操作，将引用的内容复制到目标线程中。(chatgpt)
    //由于用户自定义构造函数和析构函数的存在，pass-by-reference-to-const往往更高效，因为避免了复制(拷贝构造函数)。
    void timerTimeOutSignal(const unsigned int step);
private slots:
    void onUsockReadReady();
    void onTimerTimeOut();


public slots:
    void onRcvThreadExitSig();
};

#endif // FINDTHREAD_H
