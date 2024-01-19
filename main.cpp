#include "mainwindow.h"
#include <qdatastream.h>
#include <QApplication>
#ifdef Q_OS_WIN32
#include <windows.h>
#endif
char dir[256];
size_t max_retrans_times = 20;
size_t wait_time_ms = 2000;
size_t max_find_response_time_ms = 1000;
size_t state_file_send_interval = 10000;
unsigned short blksize = 512;
unsigned short timeout = 2;

bool checkOnly(HANDLE* m_hMutex)
{
#ifdef Q_OS_WIN32
    //  创建互斥量
    *m_hMutex  =  CreateMutex(NULL, FALSE,  L"arinc615a" );
    //  检查错误代码
    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
      //  如果已有互斥量存在则释放句柄并复位互斥量
     CloseHandle(m_hMutex);
     m_hMutex  =  NULL;
      //  程序退出
      return  false;
    }
    else
        return true;
#endif
}


int main(int argc, char *argv[])
{
    memset(dir, 0, sizeof(dir));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    HANDLE m_hMutex;
    if(checkOnly(&m_hMutex) == false){
        return 0;
    }
    //LOAD_DIR
    loadxml();
    MainWindow w;
    if(w.getPortOccupied() == true){
        return 0;
    }
    w.show();
    int res = a.exec();
    CloseHandle(m_hMutex);
    return res;
}
