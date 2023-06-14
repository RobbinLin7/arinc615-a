#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

//#include <winsock.h>
//#include <Windows.h>
#include <WinSock2.h>

extern int backflag;

extern char global_abort_code;       //用于记录终止码，并在适当位置终止程序


extern int request_pck_timeout;

//终止操作码
#define AOORT_CODE			0x55

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define FUNC_SUCCESS		1		//函数执行成功
#define FUNC_FAILURE		-1		//函数执行错误

#define SOCKET_NETWORK      1
#define AFDX_NETWORK        0


#define MAX_FRAME_LENGTH    516     //数据包的最大字节长度

//不同操作对应的阶段代号
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

	
#define FILE_MODE			"OCTET"     //发送模式为octet
#define FLLE_MODE_LEN		5           //FILE_MODE长度

#define MYPORT				"69" // port to be opened on server
#define SERVERPORT			"69" // the port users will be connecting to
#define MAXBUFLEN			550 // get sockaddr, IPv4 or IPv6:
#define MAX_READ_LEN		512 // maximum data size that can be sent on one packet
#define MAX_FILENAME_LEN	100 // maximum length of file name supported
#define MAX_PACKETS			65535 // maximum number of file packets
#define MAX_TRIES			3 // maximum number of tries if packet times out
#define TIME_OUT			5 // in seconds


#define FIND_TX_PORT (1001)
#define FIND_RX_PORT (1002)
#define CLIENT_TX_PORT (59)
#define CLIENT_RX_PORT (3000)
#define SERVER_TX_PORT (3000)
#define SERVER_RX_PORT (59)
#define STATUS_TX_PORT (2001)
#define STATUS_RX_PORT (2000)

#define CLIENT_TX (1)
#define CLIENT_RX (2)
#define SERVER_TX (3)
#define SERVER_RX (4)
#define STATUS_TX (5)
#define STATUS_RX (6)

#define SERVER_ADDR (1)

#define CLIENT_ADDR (2)

typedef struct afdxAddr
{

    int portId;
    unsigned int dstIp;
    unsigned int dstUdp;
    unsigned int pSrcIp;
    unsigned int pSrcUdp;

}AFDXAddr;

typedef struct
{
    AFDXAddr AFDX_addr;
	struct sockaddr_in sAddr;       //server端的地址信息
	struct sockaddr_in cAddr;       //client端的地址信息
}NetAddr;

extern SOCKET socketfdFind;           //find操作用到的socket

extern SOCKET socketfdOp;           //4操作用到的socket

extern SOCKET socketfdStatus;       //状态文件用到的socket

extern NetAddr findNetAddr;         //find操作用到的地址信息

extern NetAddr operationNetAddr;    //4操作用到的地址信息

extern NetAddr statusNetAddr;       //状态文件用到的地址信息

#ifdef  __cplusplus
}
#endif

#endif	//GLOBAL_CONFIG_H
