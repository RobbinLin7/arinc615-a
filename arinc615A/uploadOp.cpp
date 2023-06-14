#include "uploadOp.h"
#include "globalConfig.h"
#include "tftpClient.h"
        
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int backflag = 1;

//int global_abort_code = 0;

HANDLE mutex;

/**
 * @brief The function executes the operator download operation for downloading files
 * from configured DLCs.
 *
 * @param [out]     err_code       Success or error code
 * @param [out]     err_msg        Success or error message. Array of 1024
 *                                 characters (uint8_t).
 * @param [in]      filename       upload init operation LUI file name
 *
 * @return          ::bool_t
 * @retval          ::TT_TRUE      Download operation successful.
 * @retval          ::TT_FALSE     Download operation failed.
 *
 */
bool_t uploadInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg) {

	//global_abort_code = 0;

    //bool_t retVal = bool_t::ARINC615_TRUE;
    char fileLUI[FILE_NAME_BUFFER_SIZE] = { 0 };       //存储文件名称

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
	backflag = 1;

	//拼文件名
	//memcpy(filename, ptr->filename, strlen(ptr->filename));
	//memcpy(filename + strlen(ptr->filename), ".LUI", strlen(".LUI") + 1);
	sprintf(fileLUI, "%s%s", filename, ".LUI");

    //qDebug() << "uploadInitP filename: " << filename;

    //将接收到的数据进行保存
    if (A615NetClientGET(socketfdOp, operationNetAddr, fileLUI, getFolderPath, operationNetAddr) < 0)
    {
        *err_code = UPLOAD_ERR_CODE_302;
        memcpy(err_msg, UPLOAD_ERR_MSG_302, strlen(UPLOAD_ERR_MSG_302));

        return bool_t::ARINC615_FALSE;
    }

    return bool_t::ARINC615_TRUE;
}

bool_t uploadList(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg)
{
	//global_abort_code = 0;
    int retVal = 0;

    char fileLUR[FILE_NAME_BUFFER_SIZE] = { 0 };

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

#if AFDX_NETWORK
	//在此为客户端
	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!
    retVal = A615NetSockAddrInit(&sendAddr, CLIENT_TX, SERVER_ADDR);

    if (retVal != 0)
    {
        *err_code = NETWORK_INIT_ERR_CODE_001;
        memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

        return bool_t::ARINC615_FALSE;
    }

    retVal = A615NetSockAddrInit(&recvAddr, CLIENT_RX, SERVER_ADDR);

    if (retVal != 0)
    {
        *err_code = NETWORK_INIT_ERR_CODE_001;
        memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

        return bool_t::ARINC615_FALSE;
    }

#endif
    //发送LUR文件写请求

    //拼文件名
    memcpy(fileLUR, filename, strlen(filename));
    memcpy(fileLUR + strlen(filename), ".LUR", strlen(".LUR") + 1);

    retVal = A615NetClientPUT(socketfdOp, operationNetAddr, &operationNetAddr, fileLUR, sendFolderPath);

    if (retVal != 0)
    {
        *err_code = UPLOAD_ERR_CODE_303;
        memcpy(err_msg, UPLOAD_ERR_MSG_303, strlen(UPLOAD_ERR_MSG_303));

        return bool_t::ARINC615_FALSE;
    }

    return bool_t::ARINC615_TRUE;
}

/*
    function : uploadFile

    Description: upload文件传输阶段

    return :返回执行结果

*/

//发送LUH文件
bool_t uploadFileHeader(int16_t* err_code, uint8_t* err_msg)
{

	//global_abort_code = 0;

	char fileLUH[FILE_NAME_BUFFER_SIZE] = { 0 };

    int recvWRQLen = 0;

    char recvBuf[RECV_DATA_BUFFER_SIZE] = { 0 };

    char fileLUHPath[FILE_PATH_BUFFER_SIZE] = { 0 };

    int i = 0;
	
	int retVal = 0;

#if AFDX_NETWORK
    //在此为服务器
    NetAddr sendAaddr;

    NetAddr recvAddr;
    SOCKET sockfd = 0;              // point001--- is will problem!!!

    

    retVal = A615NetSockAddrInit(&sendAaddr, SERVER_TX, 2);

    if (retVal != 0)
    {
        *err_code = NETWORK_INIT_ERR_CODE_001;
        memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

        return bool_t::ARINC615_FALSE;
    }

    retVal = A615NetSockAddrInit(&recvAddr, SERVER_RX, 2);

    if (retVal != 0)
    {
        *err_code = NETWORK_INIT_ERR_CODE_001;
        memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));

        return bool_t::ARINC615_FALSE;
    }
#endif

    //接收LUH文件读请求
    recvWRQLen = A615NetRecvfrom(socketfdOp, recvBuf, MAXBUFLEN - 1, &operationNetAddr);

    //printf("uploadFileHeader 1:: recv LUH read req: %d!\n\n", recvWRQLen);

    if (0 >= recvWRQLen)
    {
        Sleep(10);
        for (i = 0; i < RE_TRANS_TIMES * 3; i++)
        {
            recvWRQLen = A615NetRecvfrom(socketfdOp, recvBuf, MAXBUFLEN - 1, &operationNetAddr);
            if (0 < recvWRQLen)
            {
                break;
            }
            Sleep(10);
        }
    }

    //printf("uploadFileHeader 2:: recv LUH read req: %d!\n\n", recvWRQLen);
    if (-1 == recvWRQLen)
    {
        *err_code = UPLOAD_ERR_CODE_304;
        memcpy(err_msg, UPLOAD_ERR_MSG_304, strlen(UPLOAD_ERR_MSG_304));

        return bool_t::ARINC615_FALSE;
    }
    for (i = 2; recvBuf[i] != 0; i++)
    {
        fileLUH[i - 2] = recvBuf[i];
    }

    sprintf(fileLUHPath, "%s\\%s", sendFolderPath, fileLUH);
    //printf("fileLUHPath:%s\n", fileLUHPath);

    //发送LUH文件
    retVal = A615NetServerGET(socketfdOp, operationNetAddr, operationNetAddr, fileLUHPath);

    if (retVal != 0)
    {
        *err_code = UPLOAD_ERR_CODE_305;
        memcpy(err_msg, UPLOAD_ERR_MSG_305, strlen(UPLOAD_ERR_MSG_305));

        return bool_t::ARINC615_FALSE;
    }

    return bool_t::ARINC615_TRUE;
}


void uploadFileData(void* a)
{

	//global_abort_code = 0;
    //流程
    //发送文件中的内容

    //接收LUP文件读请求

    //发送读请求对应的文件
	int recvWRQLen = 0;

	char recvBuf[RECV_DATA_BUFFER_SIZE] = { 0 };

	char fileLUP[FILE_NAME_BUFFER_SIZE] = { 0 };

	char fileLUPPath[FILE_PATH_BUFFER_SIZE] = { 0 };

	int i = 0;

	int loopCount = 0;
	
	int retVal = 0;

    //u_long iMode = 0;

#if AFDX_NETWORK
    //在此为服务器
    SOCKET sockfd = 0;
    NetAddr sendAddr;

    NetAddr recvAddr;

    //int fileNum = 0;

    retVal = A615NetSockAddrInit(&sendAddr, SERVER_TX, 2);

    if (retVal != 0)
    {
        //qDebug() << "uploadList:: A615NetSockAddrInit error.";
        return;
    }

    retVal = A615NetSockAddrInit(&recvAddr, SERVER_RX, 2);

    if (retVal != 0)
    {
        //qDebug() << "uploadList:: A615NetSockAddrInit error.";
        return;
    }
#endif
    
    while (1)
    {
        WaitForSingleObject(mutex, 100);

        if (backflag == 0) {
            printf("UPLOAD thread ID: 0x%x.\n\n", GetCurrentThread());
            printf("UPLOAD thread p1 STOP!\n\n");
            return;
        }
        ReleaseMutex(mutex);

        //接收LUP文件读请求
        recvWRQLen = A615NetRecvfrom(socketfdOp, recvBuf, MAXBUFLEN - 1, &operationNetAddr);

        printf("uploadFileHeader 1:: recv LUP read req: %d!\n\n", recvWRQLen);
        WaitForSingleObject(mutex, 100);
        if (backflag == 0) {
            printf("UPLOAD thread ID: 0x%x.\n\n", GetCurrentThread());
            printf("UPLOAD thread p2 STOP!\n\n");
            return;
        }
        ReleaseMutex(mutex);
        if (0 >= recvWRQLen)
        {
            for (int i = 0; i < 5 * RE_TRANS_TIMES; i++)
            {
                recvWRQLen = A615NetRecvfrom(socketfdOp, recvBuf, MAXBUFLEN - 1, &operationNetAddr);
                if (0 < recvWRQLen)
                {
                    break;
                }

                //Sleep(10);
            }
        }
        printf("uploadFileHeader 2:: recv LUP read req: %d, loop count: %d!\n\n", recvWRQLen, loopCount);
        if (loopCount >= 100)
        {
            //printf("uploadFileData while loop quit! loop count = 50");
            break;
        }

        if (-1 == recvWRQLen)
        {

            loopCount++;
            Sleep(500);
            continue;
        }

        memset(fileLUP, 0, FILE_NAME_BUFFER_SIZE);

        for (i = 2; recvBuf[i] != 0; i++)
        {
            fileLUP[i - 2] = recvBuf[i];
        }

        sprintf(fileLUPPath, "%s\\%s", fileFolderPath, fileLUP);
        /* 20211202：修改数据文件存储路径为用户指定路径，by liuli  */
        //sprintf(fileLUPPath, "%s", fileLUP);

        //发送LUP文件
        retVal = A615NetServerGET(socketfdOp, operationNetAddr, operationNetAddr, fileLUPPath);

        if (retVal != 0)
        {
            //printf("uploadFileData send error!!");

            continue;
        }

        WaitForSingleObject(mutex, 100);
        if (backflag == 0) {
            printf("UPLOAD thread ID: 0x%x.\n\n", GetCurrentThread());
            printf("UPLOAD thread p3 STOP!\n\n");
            return;
        }
        ReleaseMutex(mutex);

        
    }
    return;
}




