#include "informationOp.h"

#include "message_define.h"
#include "create_protocol.h"
#include "tftpClient.h"

#include <stdio.h>

bool_t informationInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{
	char fileLCI[FILE_NAME_BUFFER_SIZE] = { 0 };       //存储文件名称

	int ret = 0;

#if AFDX_NETWORK

	NetAddr addrSend;

	NetAddr addrRecv;
	SOCKET sockfd = 0;              // point001--- is will problem!!!
	//在此为客户端

	if (A615NetSockAddrInit(&addrSend, CLIENT_TX, CLIENT_ADDR) < 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}

	if (A615NetSockAddrInit(&addrRecv, CLIENT_RX, CLIENT_ADDR) < 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}
#endif
	//拼文件名
	memcpy(fileLCI, filename, strlen(filename));
	memcpy(fileLCI + strlen(fileLCI), ".LCI", strlen(".LCI") + 1);

    printf("getFolderPath: %s.\n", getFolderPath);
	//发送用	   2022-3-2 add
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);

	//接收用
	operationNetAddr.sAddr.sin_family = AF_INET;
	operationNetAddr.sAddr.sin_port = htons(HOST_OPERATION_BIND_PORT);
	operationNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);
	// add end
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

    //system("pause");

	//将接收到的数据进行保存
	if (A615NetClientGET(socketfdOp, operationNetAddr, fileLCI, getFolderPath, operationNetAddr) < 0)
	{
		*err_code = INFORMATION_ERR_CODE_200;
		memcpy(err_msg, INFORMATION_ERR_MSG_200, strlen(INFORMATION_ERR_MSG_200));

		return bool_t::ARINC615_FALSE;
	}

	return bool_t::ARINC615_TRUE;
}

bool_t informationFile(int16_t* err_code, uint8_t* err_msg)
{
	int ret = 0;

	char recvBuf[RECV_DATA_BUFFER_SIZE] = { 0 };

	char fileNameBuf[FILE_NAME_BUFFER_SIZE] = { 0 };

	char filePathBuf[FILE_PATH_BUFFER_SIZE] = { 0 };

	int recvWRQLen = 0;
#if AFDX_NETWORK
	NetAddr sendAddr;

	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!

	//在此为服务器
	ret = A615NetSockAddrInit(&sendAddr, SERVER_TX, CLIENT_ADDR);

	if (ret != 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}

	ret = A615NetSockAddrInit(&recvAddr, SERVER_RX, SERVER_ADDR);

	if (ret != 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}
#endif

	//接收写文件请求  LCL写文件请求
	int tmpRecvCount = 0;

	Sleep(1000);
	while (1)
	{
		recvWRQLen = A615NetRecvfrom(socketfdOp, recvBuf, MAXBUFLEN - 1, &operationNetAddr);

		if (-1 != recvWRQLen)
			break;

		tmpRecvCount++;

		if (tmpRecvCount == 5)
		{
			*err_code = INFORMATION_ERR_CODE_202;

			memcpy(err_msg, INFORMATION_ERR_MSG_202, strlen(INFORMATION_ERR_MSG_202));
			return bool_t::ARINC615_FALSE;

		}
		Sleep(200);
	}

	int i = 0;

	for (i = 2; recvBuf[i] != 0; i++)
	{
		fileNameBuf[i - 2] = recvBuf[i];
	}

	sprintf(filePathBuf, "%s\\%s", getFolderPath, fileNameBuf);

	ret = A615NetServerPUT(socketfdOp, operationNetAddr, filePathBuf, operationNetAddr);

	if (ret < 0)
	{
		*err_code = INFORMATION_ERR_CODE_203;

		memcpy(err_msg, INFORMATION_ERR_MSG_203, strlen(INFORMATION_ERR_MSG_203));
		return bool_t::ARINC615_FALSE;
	}

	return bool_t::ARINC615_TRUE;
}


