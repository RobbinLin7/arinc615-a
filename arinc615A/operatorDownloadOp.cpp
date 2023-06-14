#include "operatorDownloadOp.h"
#include "create_protocol.h"
#include "message_define.h"
#include "tftpClient.h"
#include "globalConfig.h"

#include <stdio.h>

//int global_abort_code = 0;
int request_pck_timeout = 0;

bool_t oDownloadInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{
	global_abort_code = 0;
	//在此为客户端
	char fileLNO[FILE_NAME_BUFFER_SIZE] = { 0 };       //存储文件名称

	int ret = 0;

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

    //发送用
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);
	if(dest_ip_addr != NULL)
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

	backflag = 1;

	//拼文件名
	memcpy(fileLNO, filename, strlen(filename));
	memcpy(fileLNO + strlen(fileLNO), ".LNO", strlen(".LNO") + 1);

	//将接收到的数据进行保存
	ret = A615NetClientGET(socketfdOp, operationNetAddr, fileLNO, getFolderPath, operationNetAddr);
	if (ret < 0)
	{
		*err_code = O_DOWNLOAD_ERR_CODE_502;
		memcpy(err_msg, O_DOWNLOAD_ERR_MSG_502, strlen(O_DOWNLOAD_ERR_MSG_502));

		return bool_t::ARINC615_FALSE;
	}
	
	return bool_t::ARINC615_TRUE;
}

/*
function : oDownloadList

Description : 列表传输阶段

return : 返回执行结果

*/
bool_t oDownloadList(char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{
	//global_abort_code = 0;
	int retval = 0;

	int i = 0;

	char fileLNL[FILE_NAME_BUFFER_SIZE] = { 0 };
	char fileLNLpath[FILE_PATH_BUFFER_SIZE] = { 0 };

	char recvbuf[FILE_NAME_BUFFER_SIZE] = { 0 };

#if AFDX_NETWORK
	//在此为服务器,接收LNL文件
	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!

	retval = A615NetSockAddrInit(&sendAddr, SERVER_TX, 1);

	if (retval != 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}

	retval = A615NetSockAddrInit(&recvAddr, SERVER_RX, 1);

	if (retval != 0)
	{
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}
#endif
	//接受LNL文件读请求
	//qDebug() << "oDownloadList begin recv LNL file request!";

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

	retval = A615NetRecvfrom(socketfdOp, recvbuf, MAXBUFLEN - 1, &operationNetAddr);

	if (-1 == retval)
	{
		for (int i = 0; i < RE_TRANS_TIMES * 10; i++)
		{
			retval = A615NetRecvfrom(socketfdOp, recvbuf, MAXBUFLEN - 1, &operationNetAddr);
			if (-1 != retval)
			{
				printf("oDownloadList  recv A LNL file request!\n");
				break;
			}
			Sleep(10);
		}
	}

	if (-1 == retval)
	{
		printf("oDownloadList not recv LNL file request! Timeout!");
		*err_code = O_DOWNLOAD_ERR_CODE_503;
		memcpy(err_msg, O_DOWNLOAD_ERR_MSG_503, strlen(O_DOWNLOAD_ERR_MSG_503));

		return bool_t::ARINC615_FALSE;
	}

	for (i = 2; recvbuf[i] != 0; i++)
	{
		fileLNL[i - 2] = recvbuf[i];
	}

	sprintf(fileLNLpath, "%s\\%s", getFolderPath, fileLNL);

	retval = A615NetServerPUT(socketfdOp, operationNetAddr, fileLNLpath, operationNetAddr);

	if (0 != retval)
	{
		printf("oDownloadList not recv LNL file");
		*err_code = O_DOWNLOAD_ERR_CODE_504;
		memcpy(err_msg, O_DOWNLOAD_ERR_MSG_504, strlen(O_DOWNLOAD_ERR_MSG_504));

		return bool_t::ARINC615_FALSE;
	}

	return bool_t::ARINC615_TRUE;
}

bool_t transLNAFile(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{
	//global_abort_code = 0;

	//接收LNL完毕变为客户端，发送LNA文件

	char fileLNA[FILE_NAME_BUFFER_SIZE] = { 0 };

	int retval = 0;

#if AFDX_NETWORK

	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!

	retval = A615NetSockAddrInit(&sendAddr, CLIENT_TX, 2);
	if (retval != 0) {
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}

	retval = A615NetSockAddrInit(&recvAddr, CLIENT_RX, 2);
	if (retval != 0) {
		*err_code = NETWORK_INIT_ERR_CODE_001;
		memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

		return bool_t::ARINC615_FALSE;
	}
#endif

    //发送用
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);
	if (dest_ip_addr != NULL)
		operationNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);
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

	//发送LNA文件写请求
	memcpy(fileLNA, filename, strlen(filename));
	memcpy(fileLNA + strlen(filename), ".LNA", strlen(".LNA") + 1);
    Sleep(1000);
	retval = A615NetClientPUT(socketfdOp, operationNetAddr,
		&operationNetAddr, fileLNA, sendFolderPath);

	if (retval != 0)
	{
		printf("oDownloadList:: send LNA file error!\n");
		*err_code = O_DOWNLOAD_ERR_CODE_504;
		memcpy(err_msg, O_DOWNLOAD_ERR_MSG_504, strlen(O_DOWNLOAD_ERR_MSG_504));

		return bool_t::ARINC615_FALSE;
	}

	return bool_t::ARINC615_TRUE;
}


/*
function : oDownloadFile

Description : 文件传输阶段

return : 返回执行结果

*/
void oDownloadFile(void *a) 
{
	//global_abort_code = 0;
	char recvbuf[WRITE_READ_REQUEST_BUFFER_SIZE] = { 0 };

	char fileLUP[FILE_NAME_BUFFER_SIZE] = { 0 };

	char fileLUPpath[FILE_PATH_BUFFER_SIZE] = { 0 };

	char fileLNSName[FILE_NAME_BUFFER_SIZE] = { 0 };
	char fileLNSPath[FILE_NAME_BUFFER_SIZE] = { 0 };

	int i = 0;
	
	int retval = 0;

#if AFDX_NETWORK
	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!

	//在此为服务器，接收传输的文件LUP

	retval = A615NetSockAddrInit(&sendAddr, SERVER_TX, 2);
	if (retval != 0) {
		//qDebug() << "oDownloadFile::  A615NetSockAddrInit error.";
		return;
	}
	retval = A615NetSockAddrInit(&recvAddr, SERVER_RX, 2);

	if (retval != 0) {
		printf("oDownloadFile::  A615NetSockAddrInit error.\n");
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

	strcpy(fileLNSName, (char *)a);

	sprintf(fileLNSPath, "%s\\%s%s", getFolderPath, fileLNSName, ".LNS");

    //remove(fileLNSPath);

	//解析LNS文件
	struct File_LNS lns;

	int lnsRet = 0;

	int whileLoopCount = 0;

    memset(&lns, 0, sizeof(struct File_LNS));

	while (1) 
	{
        if (lns.op_stat_code == 0x0003 || backflag == 0) 
		{
			printf("operator download p1 finish!\n\n");

            //free(lns.LNS_files);
			//结束该线程
            printf("oDownloadFile p1 run over thread dead.\n\n");
			return;
		}

        if(lns.op_stat_code == OPT_ABTED_DUETO_INIT ||
				lns.op_stat_code == OPT_ABTED_DUETO_DATALOADER ||
				lns.op_stat_code == OPT_ABT_BY_TAR)
        {
            //global_abort_code = 0;

            printf("oDownloadFile p2 code = 0x%x thread dead.\n\n", lns.op_stat_code);
            return;
        }
		memset(fileLUP, 0, 256);
		//接受LUP文件写请求
		retval = A615NetRecvfrom(socketfdOp, recvbuf, MAXBUFLEN - 1, &operationNetAddr);

        if (lns.op_stat_code == 0x0003 || backflag == 0) 
		{
			printf("operator download p3 finish!\n\n");

            //free(lns.LNS_files);
			//结束该线程
            printf("oDownloadFile run over thread dead.\n\n");
			return;
		}
        if (retval <= 0)
		{
			printf("oDownloadFile:: not recv LUP write REQ package!!\n");
			Sleep(100);

			whileLoopCount++;

            if (whileLoopCount == 10 * RE_TRANS_TIMES)
		    {
			    printf("oDownloadFile:: recv 6 times! while loop break !!\n");
                printf("oDownloadFile p4 thread dead!\n\n");
                request_pck_timeout = 1;
			    return;
		    }
			continue;
		}
		if (recvbuf[1] != 2)
		{
			//printf("oDownloadFile:: recv A error package!!\n");
			continue;
		}

		//拿出文件名
		for (i = 2; recvbuf[i] != 0; i++)
		{
			fileLUP[i - 2] = recvbuf[i];
		}

		sprintf(fileLUPpath, "%s\\%s", getFolderPath, fileLUP);

		retval = A615NetServerPUT(socketfdOp, operationNetAddr, fileLUPpath, operationNetAddr);

		if (backflag == 0) 
		{
			printf("operator download p5 finish!\n\n");

            //free(lns.LNS_files);
			//结束该线程
			return;
		}


	}

    //printf("oDownloadFile thread dead!\n\n");
    printf("oDownloadFile p6 thread dead!\n\n");

	return;
}


