#pragma comment(lib,"Winmm.lib")

#include "tftpClient.h"

#include "globalConfig.h"
#include "abort.h"

#include <io.h>
#include <stdio.h>
#include <windows.h>

#include "utility.h"
#include "file_define.h"

#include "arinc615_global_var_define.h"

#include "create_protocol.h"

SOCKET socketfdFind = INVALID_SOCKET;           //find操作用到的socket

SOCKET socketfdOp = INVALID_SOCKET;           //4操作用到的socket

SOCKET socketfdStatus = INVALID_SOCKET;       //状态文件用到的socket

NetAddr findNetAddr;         //find操作用到的地址信息

NetAddr operationNetAddr;    //4操作用到的地址信息

NetAddr statusNetAddr;       //状态文件用到的地址信息

static int file_not_exist_flag = 0;

//初始化系统环境及socket
//-1 环境初始化失败
//-2 socket初始化失败
/*
int A615NetSockInit(SOCKET* sockfd)
{
    WSADATA wsadata;
    int nError = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (nError != 0)
    {
        //printf("SockInit ERROR\n");
        return -1;
    }

    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(*sockfd < 0)
    {
        perror("Create Socket Failed:");
        return -2;
    }

    return 0;
}
*/
//初始化地址
/**
 * @brief A615NetSockAddrInit   初始化地址
 * @param addrInfo
 * @param portFlag              端口标识，详见global宏定义
 * @return                      错误码
 */
int A615NetSockAddrInit(NetAddr* addrInfo, int portFlag, int addrFlag)
{
#if AFDX_NETWORK
    //qDebug() << "A615NetSockAddrInit begin!";
        //AFDX CODE
    memset(addrInfo,0, sizeof(struct sockaddr_in));

    switch(addrFlag)
    {
        case SERVER_ADDR:
        addrInfo->AFDX_addr.dstIp = TARGET_IP;
        break;
        case CLIENT_ADDR:
        addrInfo->AFDX_addr.dstIp = TARGET_IP;
        break;

    default:
        addrInfo->AFDX_addr.dstIp = TARGET_IP;
        //printf("use default addr: 127.0.0.1, self test!!!\n\n");
        break;
    }
    switch(portFlag)
    {
        case CLIENT_RX:
            addrInfo->AFDX_addr.dstUdp = CLIENT_RX_PORT;
            break;
        case CLIENT_TX:
            addrInfo->AFDX_addr.dstUdp = CLIENT_TX_PORT;
            break;
        case SERVER_TX:
            addrInfo->AFDX_addr.dstUdp = SERVER_TX_PORT;
            break;
        case SERVER_RX:
            addrInfo->AFDX_addr.dstUdp = SERVER_RX_PORT;
        break;
        case STATUS_RX:
            addrInfo->AFDX_addr.dstUdp = STATUS_RX_PORT;
        break;
        case STATUS_TX:
            addrInfo->AFDX_addr.dstUdp = STATUS_TX_PORT;
            break;
        default:
            return -1;
    }

    return 0;

#endif

#if SOCKET_NETWORK

	memset(addrInfo,0, sizeof(struct sockaddr_in));
	addrInfo->sAddr.sin_family = AF_INET;

	switch(addrFlag)
	{
		case SERVER_ADDR:
		addrInfo->sAddr.sin_addr.s_addr = inet_addr("192.168.5.5");
		break;
		case CLIENT_ADDR:
		addrInfo->sAddr.sin_addr.s_addr = inet_addr("192.168.5.6");
		break;

	default:
		addrInfo->sAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		printf("use default addr: 127.0.0.1, self test!!!\n\n");
		break;
	}

	switch(portFlag)
	{
		case CLIENT_RX:
			addrInfo->sAddr.sin_port = htons(CLIENT_RX_PORT);
			break;
		case CLIENT_TX:
			addrInfo->sAddr.sin_port = htons(CLIENT_TX_PORT);
			break;
		case SERVER_TX:
			addrInfo->sAddr.sin_port = htons(SERVER_TX_PORT);
			break;
		case SERVER_RX:
			addrInfo->sAddr.sin_port = htons(SERVER_RX_PORT);
		break;
		case STATUS_RX:
			addrInfo->sAddr.sin_port = htons(STATUS_RX_PORT);
		case STATUS_TX:
			addrInfo->sAddr.sin_port = htons(STATUS_TX_PORT);
			break;
		default:
			return -1;
	}

	return 0;
 #endif
}

//调用AFDX或SOCKET sentto 发送数据
int A615NetSendto(SOCKET sockfd, char* data, NetAddr addrInfo, int dataLen)
{
    int retval=0;
    //int dataLen = 0;

#if AFDX_NETWORK
    retval = AfdxSendMsg(addrInfo.AFDX_addr.dstUdp, data, dataLen, addrInfo.AFDX_addr.dstIp, addrInfo.AFDX_addr.dstUdp);
    //retval = dataLen;

#endif

#if SOCKET_NETWORK
//        int ipLength = sizeof (struct sockaddr);

	retval = sendto(sockfd, data, dataLen, 0, (struct sockaddr*)&addrInfo.cAddr, sizeof (struct sockaddr));

        //sendto(SOCKET s,const char *buf,int len,int flags,const struct sockaddr *to,int tolen);
#endif

    return retval;
}

/**
 * @brief A615NetRecvfrom
 * @param sockfd
 * @param buf
 * @param buf_len
 * @param addrInfo
 * @return
 */
int A615NetRecvfrom(SOCKET sockfd, char* buf, int buf_len, NetAddr* addrInfo)
{
    int retval = 0;
#if AFDX_NETWORK


    int length = 0;
    retval = AfdxRecvMsg(addrInfo->AFDX_addr.dstUdp, buf, &length, &addrInfo->AFDX_addr.pSrcIp, &addrInfo->AFDX_addr.pSrcUdp);
    if(retval == -1)
    {
        return retval;
    }
    return length;
#endif


#if 1
    struct timeval tv = {10, 0};

    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(sockfd, &rdfds);
    retval = select(sockfd + 1, &rdfds, NULL, NULL, &tv);
    if(retval < 0)
    {
        printf("select error, select return：%d.\n", retval);
        return -1;
    }
    if(0 == retval)
    {
        printf("select timeout!\n");
        return -1;
    }
#endif

#if SOCKET_NETWORK
    int sockLen = sizeof(sockaddr_in);

    //printf("TEST: socket: %d, bufaddr: 0x%x, buf_len: %d.\n\n\n", sockfd, buf, buf_len);
    retval = recvfrom(sockfd, buf, buf_len, 0, (struct sockaddr*)&addrInfo->cAddr, &sockLen);
    //retval = recvfrom(sockfd, buf, buf_len, 0, (struct sockaddr*)&addrInfo->sAddr, &sockLen);
    //recvfrom(SOCKET s,char *buf,int len,int flags,struct sockaddr *from,int *fromlen);
    return retval;
#endif
}

/**
 * @brief A615NetSockRecvWithTimeout
 * @param sockfd
 * @param buf
 * @param addrInfo
 * @param addr_len
 * @return
 */
int A615NetSockRecvWithTimeout(SOCKET sockfd, char* buf, NetAddr *addrInfo, int addr_len)
{
#if SOCKET_NETWORK
    //文件描述符设置结构
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = TIME_OUT;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(sockfd + 1, &fds, NULL, NULL, &tv);
    if (n == 0)
    {
        printf("Recvfrom timeout\n");
        return -2; // timeout!
    } else if (n == -1)
    {
        printf("Timeout Set Error\n");
        return -1; // error
    }

    return recvfrom(sockfd, buf, MAXBUFLEN - 1 , 0, (struct sockaddr *)&(addrInfo->sAddr), &addr_len);
#endif

#if AFDX_NETWORK

    int retval = 0;

    int length = 0;
    retval = AfdxRecvMsg(addrInfo->AFDX_addr.dstUdp, buf, &length, &addrInfo->AFDX_addr.pSrcIp, &addrInfo->AFDX_addr.pSrcUdp);

    //qDebug() << "A615NetSockRecvWithTimeout recv return: " << buf;
    //qDebug() << "A615NetSockRecvWithTimeout recv return: " << retval ;
    if(retval == -1)
    {

        for(int i = 0; i < RE_TRANS_TIMES; i++)
        {
            retval = AfdxRecvMsg(addrInfo->AFDX_addr.dstUdp, buf, &length, &addrInfo->AFDX_addr.pSrcIp, &addrInfo->AFDX_addr.pSrcUdp);

            if(0 == retval)
            {
                return length;
            }

            Sleep(5);
        }

        return -2;
    }

    return length;

#endif

}

//发送RRQ请求
/**
 * @brief A615NetSendRRQ    发送RRQ请求
 * @param sockfd            socket标识符
 * @param addrInfo          地址信息
 * @param fileName          请求文件名
 * @return                  发送字节数
 */
int A615NetSendRRQ(SOCKET sockfd, NetAddr addrInfo, char* fileName)
{
    char message[WRITE_READ_REQUEST_BUFFER_SIZE] = {0};
    int questLen = 0;

    questLen = setWRQ_RRQ(1, fileName, strlen(fileName), message);

    return A615NetSendto(sockfd, message, addrInfo, questLen);
}

/**
 * @brief A615NetSendWRQ    发送WRQ请求
 * @param sockfd            socket标识符
 * @param addrInfo          地址信息
 * @param fileName          请求文件名
 * @return                  发送字节数
 */
int A615NetSendWRQ(SOCKET sockfd, NetAddr addrInfo, char* fileName, char* message, int *rqLen)
{
    //message = make_wrq(fileName);
    char tmpBuf[WRITE_READ_REQUEST_BUFFER_SIZE] = {0};
    int questLen = 0;

    questLen = setWRQ_RRQ(2, fileName, strlen(fileName), tmpBuf);

    *rqLen = questLen;

    memcpy(message, tmpBuf, questLen);

    return A615NetSendto(sockfd, tmpBuf, addrInfo, questLen);
}

/**
 * @brief A615NetSendACK    发送WRQ请求
 * @param sockfd            socket标识符
 * @param addrInfo          地址信息
 * @param block             请求文件名
 * @param ack               ack信息储存
 * @return                  发送字节数
 */
int A615NetSendACK(SOCKET sockfd, NetAddr addrInfo, short block)
{
    int ret = 0;

    char ack[ACK_BUFFER_SIZE] = {0};
    ret = make_ack(block, ack);

    if(0 == ret)
    {
        return ret;
    }

    return A615NetSendto(sockfd, ack, addrInfo, 4);
}

/**
 * @brief A615NetSendERR    发送ERROR
 * @param sockfd            sock标识符
 * @param addrInfo          地址信息
 * @param errcode           错误码
 * @param errmsg            错误信息
 * @return                  发送字节数
 */
int A615NetSendERR(SOCKET sockfd, NetAddr addrInfo, short errcode, const char* errmsg)
{
    int errLen = 0;

    char *message = make_err(errcode, errmsg, &errLen);

    return A615NetSendto(sockfd, message, addrInfo, errLen);
}

/**
 * @brief A615NetSendDataPack   发送数据包
 * @param sockfd                sock标识符
 * @param addrInfo              地址信息
 * @param block                 块号
 * @param data                  数据
 * @return                      发送数据
 */
int A615NetSendDataPack(SOCKET sockfd, NetAddr addrInfo, short block, char* data, char* message, int dataLen)
{
    int packLen = 0;

    message = make_data_pack(block, data, dataLen);

    packLen = 4 + dataLen;

    return A615NetSendto(sockfd, message, addrInfo, packLen);
}

int A615NetRecvFile(SOCKET sockfd, char* fileName, NetAddr sendAddr, NetAddr recvAddr)
{
    //缓冲区
    char buf[MAXBUFLEN] = {0};

    FILE *fp = NULL;

    char last_recv_message[MAXBUFLEN] = {0};
    
    char last_sent_ack[MAX_ACK_BUFFER_SIZE] = {0};
    
    int numbytes = 0;

    short blockNum = 0;

    int c_written = 0;

    //printf("A615NetRecvFile recv file name: %s.\n", fileName);

    //创建文件准备写入
    fp = fopen(fileName, "wb");

    if(fp == NULL)
    {//ERROR CHECKING
        printf("CLIENT: error opening file: %s\n", fileName);
        return -4;
    }

    //RECEIVING ACTUAL FILE
    
    do{
        //RECEIVING FILE - PACKET DATA
        memset(buf, 0, MAXBUFLEN);
        //recvAddr

        numbytes = A615NetRecvfrom(sockfd, buf, MAXBUFLEN - 1, &recvAddr);


        //printf("A615NetRecvFile  recv len: %d.\n\n", numbytes);
        //printf("A615NetRecvFile socket:%d recv port: %d, addr: %d\n\n",sockfd, htons(recvAddr.cAddr.sin_port), recvAddr.cAddr.sin_addr.S_un.S_addr);
        //printf("A615NetRecvFile  recv port: %d, addr: %d\n\n", htons(recvAddr.sAddr.sin_port), recvAddr.cAddr.sin_addr.S_un.S_addr);

        if(0x2 == buf[1])
        {
            //printf("recv A request pck!\n\n");
            return (-2);
        }
        //if(0x3 == buf[1])
        //{
            //printf("recv A data pck!\n\n");
        //}
        if (-1 == numbytes)
        {
            for(int i = 0; i < RE_TRANS_TIMES*5; i++)
            {
                numbytes = A615NetRecvfrom(sockfd, buf, MAXBUFLEN - 1, &recvAddr);
                printf("A615NetRecvFile RE_TRANS_TIMES:%d, recv len: %d.\n\n",i, numbytes);
                if(-1 != numbytes)
                {

                    //blockNum = buf[2];
                    //blockNum = (blockNum << 8) | buf[3];
                    blockNum = *(short*)&buf[2];
                    break;
                }
                else
                {
                    Sleep(2);
                    
                    continue;
                }

                if((2 == i) && (-1 == numbytes))
                {
                    return -1;
                }

            }
        }
        else
        {
            blockNum = *(short*)&buf[2];

            //printf("block number: %d\n\n\n", blockNum);
        }

        if(-1 == numbytes)
        {
            return -1;
        }

        buf[numbytes] = '\0';

        //WRITING FILE - PACKET DATA
        c_written = numbytes - 4;
        //buf前两个字节为包标识，03标识数据包，第三第四个字节为数据包序号
        fwrite(buf + 4, sizeof(char), c_written, fp);

        //将最后收到的数据暂存
        strcpy(last_recv_message, buf);

        //printf("send ACK block number: %d.\n\n", blockNum);
        numbytes = A615NetSendACK(sockfd, sendAddr, blockNum);
        //printf("send ACK len: %d.\n\n", numbytes);
        if(-1 == numbytes)
        {//ack发送失败，return
            perror("CLIENT ACK: sendto");
            return -3;
        }

        //strcpy(last_sent_ack, ack);
        //sprintf(last_sent_ack, "%s", ack);

        //if(NULL != ack)
        //    free(ack);

    } while(c_written == MAX_READ_LEN);

    //printf("NEW FILE: %s SUCCESSFULLY MADE\n", fileName);
    fclose(fp);

    return 0;
}

/**
 * @brief A615NetClientGET      TFTP客户端下载
 * @param sockfd                sock标识符
 * @param addrInfo              地址信息
 * @param fileName              文件名
 * @return                      发送结果
 */
int A615NetClientGET(SOCKET sockfd, NetAddr sendInfo, char* fileName, const char * filePath, NetAddr recvInfo)
{
    //数据源地址
    NetAddr source_addr;

    int numbytes = 0;

    char pathBuf[FILE_PATH_BUFFER_SIZE] = {0};

    //A615NetSockAddrInit(&source_addr, SERVER_ADDR, CLIENT_RX);

    //printf("send %s request!\n", fileName);

    //发送读请求
    if((numbytes = A615NetSendRRQ(sockfd, sendInfo, fileName)) == -1)
    {
        for(int i = 0; i < RE_TRANS_TIMES; i++)
        {
            numbytes = A615NetSendRRQ(sockfd, sendInfo, fileName);
            if(numbytes != -1)
            {
                break;
            }
        }
    }

    if(numbytes == -1)
    {
        return -2;
    }
    //printf("CLIENT: sent %d bytes to server\n", numbytes);

    //拼文件路径
    sprintf(pathBuf, "%s\\%s", filePath, fileName);

    printf("A615NetClientGET file path: %s, %s.\n", filePath, pathBuf);

    //Sleep(DELAY_TIME_TMP);

    if((numbytes = A615NetRecvFile(sockfd, pathBuf, sendInfo, recvInfo)) < 0)
    {
        for(int i = 0; i < RE_TRANS_TIMES; i++)
        {
            numbytes = A615NetRecvFile(sockfd, pathBuf, sendInfo, recvInfo);

            if(numbytes >= 0)
            {
                break;
            }
            //Sleep(DELAY_TIME_TMP);
        }

        return numbytes;
    }

    return 0;
}

int A615NetTimeoutRetrans(SOCKET sockfd, char* buf, char* lastMessage, NetAddr sendAddr, NetAddr recvAddr, int sourceLen, int lastMsgLen)
{
    int times;
    int numbytes;

    int retVal = 0;

    for(times = 0; times <= MAX_TRIES; ++times)
    {
        if(times == MAX_TRIES)
        {// reached max no. of tries
            //达到超时重传最大次数
            printf("CLIENT: MAX NUMBER OF TRIES REACHED\n");
            return -3;
        }
        // checking if timeout has occurred or not

        numbytes = A615NetSockRecvWithTimeout(sockfd, buf, &recvAddr, sourceLen);
        if(numbytes == -1)
        {//error
            perror("CLIENT: recvfrom");
            continue;
            //return -1;
        }
        else if(numbytes == -2)
        {//timeout 重传
#if SOCKET_NETWORK
            printf("CLIENT: try no. %d\n", times + 1);
            int temp_bytes;
            if((temp_bytes = sendto(sockfd, lastMessage, strlen(lastMessage), 0, (struct sockaddr*)&sendAddr.cAddr, sizeof (struct sockaddr))) == -1)
            {
                perror("CLIENT ACK: sendto");
                return -1;
            }
            //printf("CLIENT: sent %d bytes AGAIN\n", temp_bytes);
#endif

#if AFDX_NETWORK
            retVal = A615NetSendto(sockfd, lastMessage, sendAddr, lastMsgLen);

            if(-1 != retVal)
            {
                for(int i = 0; i < RE_TRANS_TIMES; i++)
                {
                    retVal = A615NetSendto(sockfd, lastMessage, sendAddr, lastMsgLen);

                    if(0 == retVal)
                        break;
                }
            }
#endif
        }
        else
        { //valid
            return numbytes;
        }
    }

    return -1;
}

int A615NetSendFile(SOCKET sockfd, char* fileName, NetAddr sendAddr, NetAddr *recvAddr)
{
    int numbytes = 0;
    char* last_message = NULL;
    char buf[MAXBUFLEN] = {0};
    int retVal = 0;

    //int sockArgTmp = 0;

    char recvACKBuf[RECV_DATA_BUFFER_SIZE] = {0};

    FILE *fp = fopen(fileName, "rb");
    if(fp == NULL || access(fileName, 0) == -1)
    {//找不到文件
        fprintf(stderr,"CLIENT: file %s does not exist\n", fileName);
        return -9;
    }

    //calculating of size of file
    //计算文件大小
    short block = 1;

    short recvBlock = 0;

    int dataLen = 0;
    fseek(fp, 0, SEEK_END);
    int total = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    //文件总字节数
    int remaining = total;
    if(remaining == 0)  //如果文件为空，必须发一个数据包
        ++remaining;
    else if(remaining % MAX_READ_LEN == 0)
        --remaining;

    while(remaining > 0)
    {
        //READING FILE - DATA PACKET
        //最大传输字节512 + 空字符 1
        char temp[MAX_READ_LEN + 5];
        if(remaining > MAX_READ_LEN)
        {//未发送字节数大于包最大数据长度，取最大数据长度
            //fread(temp + 4, MAX_READ_LEN, sizeof(char), fp);
            fread(temp, MAX_READ_LEN, sizeof(char), fp);
            //temp[MAX_READ_LEN] = '\0';
            remaining -= (MAX_READ_LEN);
            dataLen = MAX_READ_LEN;
        }
        else
        {
            //fread(temp + 4, remaining, sizeof(char), fp);
            fread(temp, remaining, sizeof(char), fp);
            //temp[remaining] = '\0';
            dataLen = remaining;
            remaining = 0;
        }

//        char blockNum[3] = {0};

//        s_to_i(blockNum, block);

//        temp[0] = 0;
//        temp[1] = 3;
//        temp[2] = blockNum[0];
//        temp[3] = blockNum[1];

//        if((numbytes = sendto(sockfd, temp, dataLen + 4, 0, (struct sockaddr*)&addrInfo.sAddr, sizeof(struct sockaddr))) == -1)
//        {
//            perror("CLIENT: sendto");
//            return -2;
//        }

        //SENDING FILE - DATA PACKET
        //if((numbytes = A615NetSendDataPack(sockfd, sendAddr, block, temp, last_message, dataLen + 4)) == -1)
        if((numbytes = A615NetSendDataPack(sockfd, sendAddr, block, temp, last_message, dataLen)) == -1)
        {
            perror("CLIENT: sendto");
            return -2;
        }
        //printf("CLIENT: sent %d bytes to server\n", numbytes);

        //WAITING FOR ACKNOWLEDGEMENT - DATA PACKET
//        if((numbytes = A615NetTimeoutRetrans(sockfd, buf, last_message, sendAddr, recvAddr, sizeof (struct sockaddr), dataLen + 4)) < 0)
//        {
//            return numbytes;
//        }
        //等待接收ACK--若一直没收到ACK则将数据包进行重新发送
        retVal = A615NetRecvfrom(sockfd, recvACKBuf, MAX_READ_LEN, recvAddr);

        int recvACKTimes = 0;

        while(-1 == retVal)
        {
            //多收几次
            for(int i = 0; i < RE_TRANS_TIMES; i++)
            {
                retVal = A615NetRecvfrom(sockfd, recvACKBuf, MAX_READ_LEN, recvAddr);

                if(-1 != retVal)
                {

                    break;
                }

                Sleep(1000);
            }

            recvACKTimes++;

            if(-1 == retVal)
            {
                //未收到ACK，将数据包进行重新发送

                numbytes = A615NetSendDataPack(sockfd, sendAddr, block, temp, last_message, dataLen + 4);
            }
            else
            {

                break;
            }

            if(recvACKTimes == RE_TRANS_TIMES)
                break;
        }

        recvBlock = *(short *)&recvACKBuf[2];

        //大小端转换
        recvBlock = BigLittleSwap16(recvBlock);


        if(recvBlock != block)
        {

            return -2;
        }
        //printf("CLIENT: packet is %d bytes long\n", numbytes);
        buf[numbytes] = 0;
        printf("CLIENT: packet contains \"%s\"\n", buf);

        if(buf[0] == 0 && buf[1] == 5)
        {//if error packet received
            fprintf(stderr, "CLIENT: got error packet: %s\n", buf);
            return -3;
        }

        //更新数据包块号
        ++block;
        if(block > MAX_PACKETS)
            block = 1;
    }

    fclose(fp);

    free(last_message);

    return 0;
}

int A615NetClientPUT(SOCKET sockfd, NetAddr sendAddr, NetAddr *recvAddr, char* fileName, const char *filePath)
{
    //数据源地址
    NetAddr source_addr;
    //数据源地址长度
    int source_addr_len = 0;

    //缓冲区
    char buf[MAXBUFLEN] = {0};

    char pathBuf[FILE_PATH_BUFFER_SIZE] = {0};

    //每次接收到数据大小
    int numbytes = 0;

    //最后发送的信息
    char last_message[MAXBUFLEN] = {0};

    int rqLen = 0;

    int retVal = 0;

    //发送写请求
    if((numbytes = A615NetSendWRQ(sockfd, sendAddr, fileName, last_message, &rqLen)) == -1)
    {

        for(int i = 0; i < RE_TRANS_TIMES*5; i++)
        {
            if((numbytes = A615NetSendWRQ(sockfd, sendAddr, fileName, last_message, &rqLen)) != -1)
            {
                break;
            }
        }
    }
    if(numbytes == -1)
    {
        return -1;
    }
    printf("CLIENT: sent %d bytes to server\n", numbytes);

    //接收ACK
    //Sleep(100);
    retVal = A615NetRecvfrom(sockfd, buf, MAXBUFLEN, recvAddr);

    if(buf[0]==0 && buf[1]==3)
    {
        //如果接收到其他数据，清除缓冲区后再接收
        memset(buf, 0, sizeof(buf));
        Sleep(1000);
        retVal = A615NetRecvfrom(sockfd, buf, MAXBUFLEN, recvAddr);
    }

    int whileLoopCount = 0;

    while(-1 == retVal)
    {
        //接收ACK
        for(int i = 0; i < RE_TRANS_TIMES*5; i++)
        {
            retVal = A615NetRecvfrom(sockfd, buf, MAXBUFLEN, recvAddr);
            if(0 <= retVal)
            {
                break;
            }
            Sleep(50);
        }

        whileLoopCount++;

        Sleep(5);

        if(whileLoopCount >= RE_TRANS_TIMES)
            return -3;
        //若还未接收到,则重新发送写请求
        if(retVal < 0)
        {
            if((numbytes = A615NetSendWRQ(sockfd, sendAddr, fileName, last_message, &rqLen)) == -1)
            {
                for(int i = 0; i < RE_TRANS_TIMES; i++)
                {
                    if((numbytes = A615NetSendWRQ(sockfd, sendAddr, fileName, last_message, &rqLen)) != -1)
                    {
                        break;
                    }
                    Sleep(5);
                }

            }else
            {
                continue;
            }
        }
        else
        {
            break;
        }
    }

    if(numbytes == -1)
    {
        return -2;
    }

    buf[numbytes] = '\0';

    sprintf(pathBuf, "%s\\%s", filePath, fileName);


    if(buf[0]==0 && buf[1]==4)
    {
        if((numbytes = A615NetSendFile(sockfd, pathBuf, sendAddr, recvAddr)) < 0)
        {
            for(int i = 0; i < RE_TRANS_TIMES; i++)
            {
                if((numbytes = A615NetSendFile(sockfd, pathBuf, sendAddr, recvAddr)) != -1)
                {
                    break;
                }
            }

        }

        if(numbytes < 0)
        {
            //qDebug() << "A615NetClientPUT A615NetSendFile reTrans err 222\n";
            return -3;
        }

    }
    else
    {
        //收到了错误的ACK报文
        printf("CLIENT ACK: expecting but got: 0x%x 0x%x 0x%x 0x%x .\n", buf[0], buf[1], buf[2], buf[3]);
        return -4;
    }

    return 0;
}

void A615NetSockClose(SOCKET sockfd)
{
    //close(sockfd);
}

int A615NetServerPUT(SOCKET sockfd, NetAddr sendInfo, char* fileName, NetAddr recvInfo)
{
    int numbytes = 0;

    int16_t err_code = 0;

    int i = 0 ;

    int ack_retrans_num = 0;

    //数据源地址
    NetAddr source_addr;

    char last_sent_ack[10] = {0};
    
    //printf("\n\n*******global_abort_code = %c*****\n\n", global_abort_code);

    //printf("htons(sendInfo.cAddr.sin_port) = %d.\n\n", htons(sendInfo.cAddr.sin_port));
    //printf("htons(sendInfo.sAddr.sin_port) = %d.\n\n", htons(sendInfo.sAddr.sin_port));

    //若有终止命令
ACK_RETRANS:
#if 0
    if( (1 == file_not_exist_flag) &&
        (htons(sendInfo.cAddr.sin_port) == HOST_STATUS_SEND_PORT))
    {
        if(-1 == (send_abort_packet(sockfd, sendInfo, '4')))
        {
            for(int i = 0 ; i < RE_TRANS_TIMES; i++)
            {
                if((-1) != (send_abort_packet(sockfd, sendInfo, '4')))
                {
                    file_not_exist_flag = 0;
                    return 0;
                }
            }
        }
        else
        {
            file_not_exist_flag = 0;
            return 0;
        }
    }
#endif

    if(('4' == global_abort_code || '5' == global_abort_code )
        && (htons(sendInfo.cAddr.sin_port) == HOST_STATUS_SEND_PORT))
    {
        //printf("send abort packet!\n\n");
        if(-1 == (send_abort_packet(sockfd, sendInfo, global_abort_code)))
        {
            for(int i = 0 ; i < RE_TRANS_TIMES; i++)
            {
                if((-1) != (send_abort_packet(sockfd, sendInfo, global_abort_code)))
                {
                    global_abort_code = 0;
                    return 0;
                }
            }
        }
        else
        {
            global_abort_code = 0;
            return 0;
        }
    }
    else
    {
        numbytes = A615NetSendACK(sockfd, sendInfo, 0);

        //printf("A615NetServerPUT  send ACK packet, len: %d!\n\n", numbytes);
        if(-1 == numbytes)
        {
            for(i = 0 ; i < RE_TRANS_TIMES; i++)
            {
                numbytes = A615NetSendACK(sockfd, sendInfo, 0);

                if(numbytes == 0)
                {
                    break;
                }
                if(i == (RE_TRANS_TIMES - 1))
                {
                    printf("A615NetSendACK: reTrans err");

                    return (-2);
                }
            }
        }
        else
        {
            printf("A615NetServerPUT::::if(true)  sent ACK len: %d!\n\n", numbytes);
        }
    }

    //if(numbytes == -1)
    //{
        //perror("A615NetSendACK: reTrans err");
        //qDebug() << "A615NetSendACK: reTrans err";
        //return -2;
    //}

    //2022-7-12
    //Sleep(10);

    numbytes = A615NetRecvFile(sockfd, fileName, sendInfo, recvInfo);
    if(numbytes < 0)
    {
        //printf("A615NetServerPUT A615NetRecvFile recv file err, len: %d!\n\n", numbytes);

        //perror("errno: ");

        //printf("err: %d.\n\n", GetLastError());

        if(-2 == numbytes)
        {
            ack_retrans_num++;
            if(ack_retrans_num > 10)
            {
                return -3;
            }
            else
            {
                //若接收到重传的请求，重复发ACK
                goto ACK_RETRANS;
            }
        }

        return numbytes;

    }
    else
    {
        printf("A615NetServerPUT recv a file!\n");
    }

    return 0;
}


int A615NetServerGET(SOCKET sockfd, NetAddr sendAddr, NetAddr recvAddr, char* fileName)
{
    //数据源地址
    NetAddr source_addr;

    //每次接收到数据大小
    int numbytes = 0;

    numbytes = A615NetSendFile(sockfd, fileName, sendAddr, &recvAddr);

    if(-9 == numbytes)
    {
        fprintf(stderr,"SERVER: file '%s' does not exist, sending error packet\n", fileName);

        //A615NetSendERR(sockfd, sendAddr, 6, "ERROR_FILE_NOT_EXISTS");
#if 0
        file_not_exist_flag = 1;

        if(-1 == (send_abort_packet(sockfd, sendAddr, '4')))
        {
            for(int i = 0 ; i < RE_TRANS_TIMES; i++)
            {
                if((-1) != (send_abort_packet(sockfd, sendAddr, '4')))
                {
                    //global_abort_code = 0;
                    return 0;
                }
            }
        }
        else
        {
            //global_abort_code = 0;
            return 0;
        }
#endif
        return numbytes;
    }
    else if(numbytes < 0)
    {
        return numbytes;
    }

    return 0;
}


//find操作用的socket资源初始化
void initFindOpAddr(struct timeval timeOut)
{
	//Find操作作为客户端
	//不需要绑定
	//发送请求以广播的方式发送

	int retVal = 0;
    u_long iMode = 1;

	BOOL bBroadcast = TRUE;

	//struct timeval timeOut = { 5, 0 };

	//发送用
	findNetAddr.cAddr.sin_family = AF_INET;
	findNetAddr.cAddr.sin_port = htons(HOST_FIND_SEND_PORT);
	findNetAddr.cAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST); ;

	//接收用
	findNetAddr.sAddr.sin_family = AF_INET;
	findNetAddr.sAddr.sin_port = htons(HOST_FIND_BIND_PORT);
	findNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);

	//SOCKET初始化
	socketfdFind = socket(AF_INET, SOCK_DGRAM, 0);

	if (socketfdFind == INVALID_SOCKET)
	{
		printf("initFindOpAddr set up operation socket Error!\n");

		return;
	}
	//设置SOCKET相关属性
	retVal = setsockopt(socketfdFind, SOL_SOCKET, SO_BROADCAST, (const char *)&bBroadcast, sizeof(BOOL));
	
	if (SOCKET_ERROR == retVal)
	{
		printf("initFindOpAddr operation setsockopt BROADCAST Error!\n");

		return;
	}
    retVal = ioctlsocket(socketfdOp, FIONBIO, &iMode);

    if(NO_ERROR != retVal)
    {
        printf("initUDPSocketAddr operation ioctlsocket Error!\n");

		return;
    }
#if 0
    printf("sizeof(timeOut) = %d, sizeof(struct timeval) = %d.\n\n\n", sizeof(timeOut), sizeof(struct timeval));

	retVal = setsockopt(socketfdFind, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(struct timeval));

	if (SOCKET_ERROR == retVal)
	{
		printf("initFindOpAddr operation setsockopt RCVTIMEO Error!\n");

		return;
	}
#endif
	//绑定监听端口
	retVal = bind(socketfdFind,
		(struct sockaddr *)&findNetAddr.sAddr,
		sizeof(findNetAddr.sAddr));

	if (0 != retVal)
	{
		printf("initFindOpAddr operation bind Error!\n");

		return;
	}



	printf("Find SOCKET source init FINISH!\n");
	return;
}

//流程操作用的socket资源初始化
void initFlowOpAddr(struct timeval timeOut)
{

	int retVal = 0;

    u_long iMode = 1;

    DWORD dwTime = TRANSFER_DELAY;

    socketfdOp = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketfdOp == INVALID_SOCKET)
	{
		printf("initUDPSocketAddr set up operation socket Error!\n");

		return;
	}

	//发送用
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);
	operationNetAddr.cAddr.sin_addr.s_addr = inet_addr(TARGET_ADDRESS);

	//接收用
	operationNetAddr.sAddr.sin_family = AF_INET;
	operationNetAddr.sAddr.sin_port = htons(HOST_OPERATION_BIND_PORT);
	operationNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);


    retVal = ioctlsocket(socketfdOp, FIONBIO, &iMode);

    if(NO_ERROR != retVal)
    {
        printf("initUDPSocketAddr operation ioctlsocket Error!\n");

		return;
    }
#if 0

    	//设置SOCKET相关属性
	retVal = setsockopt(socketfdOp, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(struct timeval));
    //retVal = setsockopt(socketfdOp, SOL_SOCKET, SO_RCVTIMEO, (char *)&dwTime, sizeof(dwTime));

	if (SOCKET_ERROR == retVal)
	{
		printf("initUDPSocketAddr operation setsockopt Error!\n");

		return;
	}
#endif
    //绑定监听端口
	retVal = bind(socketfdOp,
		(struct sockaddr *)&operationNetAddr.sAddr,
		sizeof(operationNetAddr.sAddr));

	if (0 != retVal)
	{
		printf("initUDPSocketAddr operation bind Error!\n");

		return;
	}

	printf("Flow SOCKET source init FINISH, socket: %d!\n", socketfdOp);
	return;
}

//状态文件操作用的socket资源初始化
void initStatusOpAddr(struct timeval timeOut)
{

	int retVal = 0;
    u_long iMode = 1;

    //struct timeval opt_val = { 0, 0};

    //int opt_len = sizeof(struct timeval);

    DWORD dwTime = TRANSFER_DELAY;

	//发送用
	statusNetAddr.cAddr.sin_family = AF_INET;
	statusNetAddr.cAddr.sin_port = htons(HOST_STATUS_SEND_PORT);
	statusNetAddr.cAddr.sin_addr.s_addr = inet_addr(TARGET_ADDRESS);

	//接收用
	statusNetAddr.sAddr.sin_family = AF_INET;
	statusNetAddr.sAddr.sin_port = htons(HOST_STATUS_BIND_PORT);
	statusNetAddr.sAddr.sin_addr.s_addr = inet_addr(HOST_ADDRESS);

	socketfdStatus = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketfdStatus == INVALID_SOCKET)
	{
		printf("initStatusOpAddr set up Status socket Error!\n");

		return;
	}
    
    //绑定监听端口
	retVal = bind(socketfdStatus,
		(struct sockaddr *)&statusNetAddr.sAddr,
		sizeof(statusNetAddr.sAddr));

	if (0 != retVal)
	{
		printf("initStatusOpAddr Status bind Error!\n");

		return;
	}
    retVal = ioctlsocket(socketfdOp, FIONBIO, &iMode);

    if(NO_ERROR != retVal)
    {
        printf("initUDPSocketAddr operation ioctlsocket Error!\n");

		return;
    }
#if 0
	//设置SOCKET相关属性
	retVal = setsockopt(socketfdStatus, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(struct timeval));
    //retVal = setsockopt(socketfdStatus, SOL_SOCKET, SO_RCVTIMEO, (char *)&dwTime, sizeof(dwTime));

	if (SOCKET_ERROR == retVal)
	{
		printf("initStatusOpAddr Status setsockopt Error!\n");

		return;
	}
#endif
    //getsockopt(socketfdStatus, SOL_SOCKET, SO_RCVTIMEO, (char *)&opt_val, &opt_len);

    //printf("getsockopt: second = %d, ms = %d.\n\n", opt_val.tv_sec, opt_val.tv_usec);

	//printf("Status SOCKET source init FINISH!\n");
    printf("Status SOCKET source init FINISH, socket: %d!\n", socketfdStatus);
	return;
}

void initUDPSocketAddr(void)
{
	struct timeval timeOut = { 10, 0 };
	struct timeval findTimeOut = { 1000, 0 };

	//加载winSock资源
	WSADATA wsadata;
	int nError = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (nError != 0)
	{
		printf("initUDPSocketAddr Sock Init ERROR\n");
		return;
	}

	//find操作对应的初始化
	initFindOpAddr(findTimeOut);

	//流程操作对应的初始化
	initFlowOpAddr(timeOut);

	//状态文件操作对应的初始化
	initStatusOpAddr(timeOut);

	printf("initUDPSocketAddr SOCKET and NETWORK init finish!\n");

	return;
}

void close_socket_source(void)
{
	closesocket(socketfdFind);
	closesocket(socketfdOp);
	closesocket(socketfdStatus);
	WSACleanup();//释放win socket 的相关的资源

}


