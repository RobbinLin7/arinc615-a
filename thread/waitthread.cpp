#include "waitthread.h"

WaitThread::WaitThread(QThreadPool &threadPool):
    threadPool(threadPool)
{

}

void WaitThread::run()
{
    threadPool.waitForDone();
}
