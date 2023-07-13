#ifndef WAITTHREAD_H
#define WAITTHREAD_H

#include <QObject>
#include <QThread>
#include <QThreadPool>

class WaitThread : public QThread
{
    Q_OBJECT
public:
    WaitThread(QThreadPool &threadPool);

    // QThread interface
protected:
    void run();
private:
    QThreadPool& threadPool;
};

#endif // WAITTHREAD_H
