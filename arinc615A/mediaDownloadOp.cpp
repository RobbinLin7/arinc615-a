#include "mediaDownloadOp.h"
#include "tftpClient.h"
#include "create_protocol.h"
#include "globalConfig.h"

#include <stdio.h>

//int global_abort_code = 0;

bool_t mediaDownloadInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{

	global_abort_code = 0;
	//在此为客户端

	char fileLND[FILE_NAME_BUFFER_SIZE] = { 0 };       //存储文件名称

	int ret = 0;

    backflag = 1;
#if AFDX_NETWORK
	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!
	

	if (A615NetSockAddrInit(&sendAddr, CLIENT_TX, SERVER_ADDR) < 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}

	if (A615NetSockAddrInit(&recvAddr, CLIENT_RX, SERVER_ADDR) < 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}
#endif

    //拼文件名
	memcpy(fileLND, filename, strlen(filename));
	memcpy(fileLND + strlen(filename), ".LND", strlen(".LND") + 1);

    //发送用
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);
	if (dest_ip_addr != NULL)
	{
		operationNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);
		statusNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);
	}
	else
	{
		printf("dest ip is invalid!\n");
		*err_code = IP_ADDR_INVALID_ERR_CODE;
		memcpy(err_msg, IP_ADDR_INVALID_ERR_MSG, strlen(IP_ADDR_INVALID_ERR_MSG));

		return bool_t::ARINC615_FALSE;
	}

	//接收用
	operationNetAddr.sAddr.sin_family = AF_INET;
	operationNetAddr.sAddr.sin_port = htons(HOST_OPERATION_BIND_PORT);
	operationNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);

	//将接收到的数据进行保存
	ret = A615NetClientGET(socketfdOp, operationNetAddr, fileLND, getFolderPath, operationNetAddr);
	if (ret < 0)
	{
		*err_code = M_DOWNLOAD_ERR_CODE_402;
		memcpy(err_msg, M_DOWNLOAD_ERR_MSG_402, strlen(M_DOWNLOAD_ERR_MSG_402));

		return bool_t::ARINC615_FALSE;
	}

	return bool_t::ARINC615_TRUE;
}

/*
* function : mediaDownloadList()
* Description :  介质下载文件列表阶段
* return : 操作返回结果
*
*/
bool_t mediaDownloadList(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{

	//global_abort_code = 0;
	//在此为客户端，发送LNR文件
	
	int retVal = 0;

	char fileLNR[FILE_NAME_BUFFER_SIZE] = { 0 };

#if AFDX_NETWORK
	NetAddr sendAddr;

	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!

	retVal = A615NetSockAddrInit(&sendAddr, CLIENT_TX, 1);

	if (retVal != 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}

	retVal = A615NetSockAddrInit(&recvAddr, CLIENT_RX, 1);

	if (retVal != 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}
#endif

    //发送用
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);
	if (dest_ip_addr != NULL)
	{
		operationNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);
		statusNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);
	}
	else
	{
		printf("dest ip is invalid!\n");
		*err_code = IP_ADDR_INVALID_ERR_CODE;
		memcpy(err_msg, IP_ADDR_INVALID_ERR_MSG, strlen(IP_ADDR_INVALID_ERR_MSG));

		return bool_t::ARINC615_FALSE;
	}

	//接收用
	operationNetAddr.sAddr.sin_family = AF_INET;
	operationNetAddr.sAddr.sin_port = htons(HOST_OPERATION_BIND_PORT);
	operationNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);

	memcpy(fileLNR, filename, strlen(filename));
	memcpy(fileLNR + strlen(filename), ".LNR", strlen(".LNR") + 1);

	retVal = A615NetClientPUT(socketfdOp, operationNetAddr, &operationNetAddr, fileLNR, sendFolderPath);

	if (retVal != 0)
	{
		*err_code = M_DOWNLOAD_ERR_CODE_400;
		memcpy(err_msg, M_DOWNLOAD_ERR_MSG_400, strlen(M_DOWNLOAD_ERR_MSG_400));

		return bool_t::ARINC615_FALSE;
	}

	return  bool_t::ARINC615_TRUE;
}

/*
* function: mediaDownloadFile
* Description: 介质下载文件传输阶段
* return :  操作返回结果
*
*/
void mediaDownloadFile(void *a) 
{

	//global_abort_code = 0;
	//在此为服务器，接收传输的文件 LUP文件
	
	int retval = 0;

	//在此为服务器，接收传输的文件LUP

	char recvbuf[WRITE_READ_REQUEST_BUFFER_SIZE] = { 0 };

	char fileLUP[FILE_PATH_BUFFER_SIZE] = { 0 };

	char fileLUPpath[FILE_PATH_BUFFER_SIZE] = { 0 };

	char fileLNSName[FILE_NAME_BUFFER_SIZE] = { 0 };
	char fileLNSPath[FILE_NAME_BUFFER_SIZE] = { 0 };

	int i = 0;

    int whileLoopCount = 0;

	//解析LNS文件
	struct File_LNS lns;
	int lnsRet = 0;

	strcpy(fileLNSName, (char*)a);

	sprintf(fileLNSPath, "%s\\%s%s", getFolderPath, fileLNSName, ".LNS");

    remove(fileLNSPath);

#if AFDX_NETWORK

	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!

	retval = A615NetSockAddrInit(&sendAddr, SERVER_TX, 2);
	if (retval != 0) {
		printf("mediaDownloadFile::  A615NetSockAddrInit error.\n");
		return;
	}
	retval = A615NetSockAddrInit(&recvAddr, SERVER_RX, 2);
	if (retval != 0) {
		printf("mediaDownloadFile::  A615NetSockAddrInit error.\n");
		return;
	}
#endif

    //发送用
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);
	//operationNetAddr.cAddr.sin_addr.s_addr = inet_addr(TARGET_ADDRESS);

	//接收用
	operationNetAddr.sAddr.sin_family = AF_INET;
	operationNetAddr.sAddr.sin_port = htons(HOST_OPERATION_BIND_PORT);
	operationNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);

	while (1) {

        //解析状态文件只是用来结束该线程的
		//lnsRet = parsing_LNS(fileLNSName, &lns);

		//if (-1 == lnsRet)
//		{
			//qDebug() << "mediaDownloadFile parsing_LNS error!\n\n";
			//continue;
//		}

        if (backflag == 0)
		{
            printf("media download p1 finish!\n\n");
			return;
		}

		//接收LUP文件写请求
		retval = A615NetRecvfrom(socketfdOp, recvbuf, MAXBUFLEN - 1, &operationNetAddr);
        if (backflag == 0)
		{
            printf("media download p2 finish!\n\n");
			return;
		}
		if (whileLoopCount >= 2 * RE_TRANS_TIMES)
		{
			printf("mediaDownloadFile:: recv 20 times! while loop break !!\n");
            printf("media download p3 finish!\n\n");
			break;
		}

		if (retval <= 0)
		{
			printf("mediaDownloadFile:: not recv package!!\n");

			Sleep(RE_RECV_QEQUEST_DELAY);

			whileLoopCount++;
			continue;
		}

		if (recvbuf[1] != 2)
		{
			printf("mediaDownloadFile:: recv A error package!!\n");
			continue;
		}

		//拿出文件名
		for (i = 2; recvbuf[i] != 0; i++)
		{
			fileLUP[i - 2] = recvbuf[i];
		}

		sprintf(fileLUPpath, "%s\\%s", getFolderPath, fileLUP);

		retval = A615NetServerPUT(socketfdOp, operationNetAddr, fileLUPpath, operationNetAddr);

		if (/*lns.op_stat_code == 0x0003 || */backflag == 0)
		{
            printf("media download p4 finish!\n\n");
			break;
		}
	}

	return;
}

