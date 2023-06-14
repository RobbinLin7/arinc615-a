#ifndef DIR_H
#define DIR_H
#define MAX_DIR_LEN 256
#include<string.h>
#include<QXmlStreamReader>
#include<QFile>
#include<QDebug>
extern char dir[MAX_DIR_LEN];
extern size_t max_retrans_times;
extern size_t wait_time_ms;
#define INIT "./615A.ini"
#define LOAD_DIR {QFile file(INIT);if(file.open(QIODevice::ReadOnly)){QDataStream in(&file); in.readRawData(dir, MAX_DIR_LEN); file.close();}};

#define GET_DIR QString path = dir;

#define UPDATE_DIR(pathList) QString path = pathList.at(0); strcpy(dir, path.left(path.lastIndexOf('/')).toStdString().data());

#define SAVE_DIR    {\
                    QFile file(INIT);\
                    if(file.open(QIODevice::ReadWrite | QIODevice::Truncate)){\
                       file.write(dir, strlen(dir) + 1);\
                       file.close();\
                    }\
                    };
bool loadxml();     //从XML文件中加载用户目录，最大重传次数，重传等待时间等参数
bool savexml();     //将用户目录，最大重传次数，重传等待时间等参数写到XML文件中


#endif // DIR_H
