#ifndef MESSAGE_DEFINE_H
#define MESSAGE_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#define Information_Initialization_Ok (200)
#define Information_Initialization_Fail (201)
#define Information_Send_Ok (202)
#define Information_Send_Fail (203)
#define LCL_Open_Failed (204)

#define UPLOAD_Initialization_Ok (301)
#define UPLOAD_Initialization_Fail (302)
#define UPLOAD_LURSend_Ok (303)
#define UPLOAD_LURSend_Fail (304)
#define UPLOAD_FileSend_Ok (305)
#define UPLOAD_FileSend_Failed (306)
#define UPLOAD_TransThread_Failed (307)
#define UPLOAD_HFileSend_Failed (308)
#define UPLOAD_HFileSend_OK (309)

#define MEDIADOWNLOAD_Initialization_Ok (401)
#define MEDIADOWNLOAD_Initialization_Fail (402)
#define MEDIADOWNLOAD_LNRSend_Ok (403)
#define MEDIADOWNLOAD_LNRSend_Fail (404)
#define MEDIADOWNLOAD_FileSend_Ok (405)
#define MEDIADOWNLOAD_FileSend_Failed (406)
#define MEDIADOWNLOAD_TransThread_Failed (407)
#define MEDIADOWNLOAD_FilesNotExist (408)

#define OPDOWNLOAD_Initialization_Ok (501)
#define OPDOWNLOAD_Initialization_Fail (502)
#define OPDOWNLOAD_LNLSend_Ok (503)
#define OPDOWNLOAD_LNLSend_Fail (504)
#define OPDOWNLOAD_LNASend_OK (505)
#define OPDOWNLOAD_LNASend_Failed (506)
#define OPDOWNLOAD_FileSend_Ok (507)
#define OPDOWNLOAD_FileSend_Failed (508)
#define OPDOWNLOAD_TransThread_Failed (509)

#define Find_Ok (600)
#define Find_Fail (601)
#define Find_Request_Error (602)

#define Success (700)
#define Winsock_Initial_Fail (702)
#define Socket_Create_Failed (703)
#define Sock_Bind_Failed (704)
#define Protocol_File_Failed (705)
#define File_Unavailable (706)
#define Sock_Error (707)
#define Connect_Timeout (708)

#define Protocol_Ver_Error (801)
#define Init_Status_Refuse (802)

#define STATUS_RE_PARSE_NUM     (3)

//#include<windows.h>
#include <process.h>
#include "file_define.h"
extern int testValue;
/*
struct FIND_Packet{
    char identifier[16];                //目标标识
    char type_name[9];                  //目标类型名称
    char position[9];                   //位置
    char literal_name[21];              //文字名
    char manufacture_code[4];           //制造商码
    char Terminal_code[1];              //终结符
    char IP_address[20];                //IP地址
    //char MAC_address[20];//MAC地址
}__attribute__((packed));
typedef struct FIND_Packet findpacket;

struct FIND_PacketS{
    char device_num;                    //设备数量
    struct FIND_Packet *findpacket;     //设备信息
}__attribute__((packed));
typedef struct FIND_PacketS packets;

typedef struct{
    HANDLE hmutex;
    int flag;
    int rate = 0;
    bool isdebug = false;
    struct FIND_PacketS findpackets;
}Info_F;

typedef struct{
    HANDLE hmutex;
    int flag;
    float rate;
    uint16 fileno;
    bool isdebug;
    char mdown_path[256];
    struct File_LNR lnr;
    char ipaddress[20];
    char filename[256];
}Info_M;

typedef struct{
    HANDLE hmutex;
    int flag;
    float rate;
    int lur_num;
    bool isdebug;
    char ipaddress[20];
    char filename[256];
}Info_U;

typedef struct{
    HANDLE hmutex;
    int flag;
    int rate;
    struct File_LCL lcl;
    bool isdebug;
    char ipaddress[20];
    char filename[256];
}Info_I;
typedef struct{
    HANDLE hmutex;
    int flag;
    int conum;
    float rate;
    int fileno;
    bool isdebug;
    char odown_path[256];
    char ipaddress[20];
    char filename[255];
}LNA_P;

typedef struct{
    HANDLE hmutex;
    int flag;
    int rate;
    int fileno;
    bool isdebug;
    char odown_path[256];
    char ipaddress[20];
    char filename[256];
}Info_O;

typedef struct{
    int sock_flag;
    int errorno;
}Judge_flag;
*/
#ifdef  __cplusplus
}
#endif

#endif // MESSAGE_DEFINE_H
