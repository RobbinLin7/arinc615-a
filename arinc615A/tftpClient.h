#ifndef TFTPCLIENT_H
#define TFTPCLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#pragma comment(lib,"ws2_32.lib")		//º”‘ÿws2_32.dll

//typedef int SOCKET;
#include "utility.h"
#include "globalConfig.h"
//#include <WinSock2.h>

//int A615NetSockInit(SOCKET* sockfd);
int A615NetSockAddrInit(NetAddr* addrInfo, int portFlag, int addrFlag);
int A615NetSendto(SOCKET sockfd, char* data, NetAddr addrInfo, int dataLen);
int A615NetRecvfrom(SOCKET sockfd, char* buf, int buf_len, NetAddr* addrInfo);
int A615NetSockRecvWithTimeout(SOCKET sockfd, char* buf, NetAddr *addrInfo, int addr_len);
int A615NetSendRRQ(SOCKET sockfd, NetAddr addrInfo, char* fileName);
int A615NetSendWRQ(SOCKET sockfd, NetAddr addrInfo, char* fileName, char* message, int *rqLen);
int A615NetSendACK(SOCKET sockfd, NetAddr addrInfo, short block);
int A615NetSendERR(SOCKET sockfd, NetAddr addrInfo, short errcode, const char* errmsg);
int A615NetSendDataPack(SOCKET sockfd, NetAddr addrInfo, short block, char* data, char* message, int dataLen);
//int A615NetRecvFile(SOCKET sockfd, char* fileName, NetAddr addrInfo, NetAddr sendAddr, NetAddr recvAddr);
int A615NetRecvFile(SOCKET sockfd, char* fileName, NetAddr sendAddr, NetAddr recvAddr);
int A615NetClientGET(SOCKET sockfd, NetAddr sendInfo, char* fileName, const char *filePath, NetAddr recvInfo);
int A615NetTimeoutRetrans(SOCKET sockfd, char* buf, char* lastMessage, NetAddr addrInfo, NetAddr source, int sourceLen, int lastMsgLen);
int A615NetSendFile(SOCKET sockfd, char* fileName, NetAddr sendAddr, NetAddr *recvAddr);
int A615NetClientPUT(SOCKET sockfd, NetAddr sendAddr, NetAddr *recvAddr, char* fileName, const char *filePath);
void A615NetSockClose(SOCKET sockfd);

int A615NetServerPUT(SOCKET sockfd, NetAddr sendInfo, char* fileName, NetAddr recvInfo);

int A615NetServerGET(SOCKET sockfd, NetAddr sendAddr, NetAddr recvAddr, char* fileName);


//static int file_not_exist_flag;


void initUDPSocketAddr(void);

void close_socket_source(void);

#ifdef  __cplusplus
}
#endif


#endif // TFTPCLIENT_H
