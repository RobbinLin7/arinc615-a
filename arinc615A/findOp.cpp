#include "findOp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "message_define.h"
#include "create_protocol.h"
#include "utility.h"
#include "tftpClient.h"
#include "globalConfig.h"

char global_abort_code = 0;

/**
* @brief 无符号整形十进制的IP地址转点分十进制类型的字符串
*
* @param [in]      ip_value：输入参数，IP地址数值
* @param [out]     ip_str：保存IP地址字符串的地址
* @return          返回值：字符串长度。
*
*/
int ip_uint2string(unsigned int ip_value, char *ip_str)
{
	unsigned int tmp = 0;

	int ip_len = 0;

	char tmp_str[8] = { 0 };

	//第一段
	tmp = ip_value >> 24;

	itoa(tmp, tmp_str, 10);

	tmp_str[strlen(tmp_str)] = '.';

	strcpy(ip_str, tmp_str);

	ip_len += strlen(tmp_str);

	//第二段
	memset(tmp_str, 0, 8);

	tmp = (ip_value >> 16) & 0xff;

	itoa(tmp, tmp_str, 10);

	tmp_str[strlen(tmp_str)] = '.';

	sprintf(ip_str, "%s%s", ip_str, tmp_str);

	ip_len += strlen(tmp_str);

	//第三段
	memset(tmp_str, 0, 8);

	tmp = (ip_value >> 8) & 0xff;

	itoa(tmp, tmp_str, 10);

	tmp_str[strlen(tmp_str)] = '.';

	sprintf(ip_str, "%s%s", ip_str, tmp_str);

	ip_len += strlen(tmp_str);

	//第四段
	memset(tmp_str, 0, 8);

	tmp = ip_value & 0xff;

	itoa(tmp, tmp_str, 10);

	sprintf(ip_str, "%s%s", ip_str, tmp_str);

	ip_len += strlen(tmp_str);

	return ip_len;
}


int FINDSetRequest(char* requestbuffer/*请求包缓存*/)
{
	int length;
	requestbuffer[0] = 0;
	requestbuffer[1] = 1;
	requestbuffer[2] = 0;
	requestbuffer[3] = 0x10;
	length = 4;

	return length;
}


void  FINDClientRecData(char* recvbuffer, a615_targets_find_list_t *targets)
{
	//    struct FIND_Packet{
	//    char identifier[16];//目标标识
	//    char type_name[9];//目标类型名称
	//    char position[9];//位置
	//    char literal_name[21];//文字名
	//    char manufacture_code[4];//制造商码
	//    char Terminal_code[1];
	//    char IP_address[20];//IP地址
	//    }__attribute__((packed));
	int numf = targets->device_num;
	memset(&targets->targets_info[numf], 0, sizeof(a615_target_find_info_t));

	memcpy(targets->targets_info[numf].identifier, recvbuffer, strlen(recvbuffer));

	recvbuffer = recvbuffer + strlen(recvbuffer) + 1;
	memcpy(targets->targets_info[numf].type_name, recvbuffer, strlen(recvbuffer));

	recvbuffer = recvbuffer + strlen(recvbuffer) + 1;
	memcpy(targets->targets_info[numf].position, recvbuffer, strlen(recvbuffer));

	recvbuffer = recvbuffer + strlen(recvbuffer) + 1;
	memcpy(targets->targets_info[numf].literal_name, recvbuffer, strlen(recvbuffer));

	recvbuffer = recvbuffer + strlen(recvbuffer) + 1;
	memcpy(targets->targets_info[numf].manufacture_code, recvbuffer, strlen(recvbuffer));

	if (strcmp(targets->targets_info[0].type_name, "FMS") != 0) {
		BigOrLittel = 1;
	}
}

int FINDClientRetransmission(char* SendBuffer, int len, a615_targets_find_list_t *targets)
{
	int retry = 0, reSend = 0;
	unsigned int srcIp = 0;
	unsigned int srcUdp = 0;
	int retVal = 0;
    NetAddr findRecvNetAddr;         //find操作应答包用到的地址信息

#if AFDX_NETWORK
	SOCKET sockfd = 0;
	NetAddr find_addr;
	NetAddr find_recv_addr;

	find_addr.AFDX_addr.dstUdp = FIND_TX_PORT;
	find_addr.AFDX_addr.dstIp = TARGET_IP;
	find_addr.AFDX_addr.dstUdp = FIND_TX_PORT;

	find_recv_addr.AFDX_addr.dstUdp = FIND_RX_PORT;
#endif

	for (retry = 0; retry < RE_TRANS_TIMES; retry++)
	{
		Sleep(20);
		//retVal = AfdxSendMsg(FIND_TX_PORT, SendBuffer, len, TARGET_IP, FIND_TX_PORT);

		retVal = A615NetSendto(socketfdFind, SendBuffer, findNetAddr, len);

		if (retVal != 0)
		{
			continue;
		}

		int recvLen = 0;

		char recvbuffer[RECV_DATA_BUFFER_SIZE] = { 0 };//接收缓存
		int recvretval = 0;

		//recvretval = AfdxRecvMsg(FIND_RX_PORT, recvbuffer, &recvLen, &srcIp, &srcUdp);
		recvretval = A615NetRecvfrom(socketfdFind, recvbuffer, RECV_DATA_BUFFER_SIZE, &findRecvNetAddr);

		if (recvretval >= 0) 
		{
			if (recvbuffer[0] == 0 && recvbuffer[1] == 2 && recvbuffer[recvretval - 1] == 0x10)
			{
				FINDClientRecData(recvbuffer + 2, targets);
				int numf = (int)targets->device_num;
				memset(targets->targets_info[numf].ip_addr, 0, IP_ADDR_MAX_LEN);
                memcpy(targets->targets_info[numf].ip_addr, inet_ntoa(findRecvNetAddr.cAddr.sin_addr), strlen(inet_ntoa(findRecvNetAddr.cAddr.sin_addr)));
                //mmcpy(targets->targets_info[numf].ip_addr, inet_ntoa(srcIp), strlen(inet_ntoa(serveraddress.sin_addr)));
				//ip_uint2string(srcIp, targets->targets_info[numf].ip_addr);
				targets->device_num++;
				//return Find_Ok;
				continue;
			}
			else
			{
				return Find_Request_Error;
			}
		}
	}
	if (RE_TRANS_TIMES == retry)
	{
		return Connect_Timeout;
	}

	return 0;
}

int FINDRequest(a615_targets_find_list_t *targets)
{
	int Clientlen = 0;
	int retval = 0;
	struct timeval timeout = { 3,0 };
	// int RFileFlag = 0;
	int length = 0;
	int recvretval = 0;
	int error = 0;
	unsigned int srcIp = 0;
	unsigned int srcUdp = 0;
    NetAddr findRecvNetAddr;         //find操作应答包用到的地址信息
	unsigned int numf = 0;

	//time_t begintime, endtime;				//记录while循环的时间
	//struct timeval timeOut = {2, 0};
#if AFDX_NETWORK
	SOCKET sockfd = 0;
	NetAddr find_addr;
	NetAddr find_recv_addr;

	find_addr.AFDX_addr.dstUdp = FIND_TX_PORT;
	find_addr.AFDX_addr.dstIp = TARGET_IP;
	find_addr.AFDX_addr.dstUdp = FIND_TX_PORT;

	find_recv_addr.AFDX_addr.dstUdp = FIND_RX_PORT;
#endif
	char sendbuffer[SEND_REQUEST_BUFFER_SIZE] = { 0 };

	char recvbuffer[RECV_DATA_BUFFER_SIZE] = { 0 };

	//构造请求包
	length = FINDSetRequest(sendbuffer);

	for (int i = 0; i < RE_TRANS_TIMES; i++)
	{
		//retval = AfdxSendMsg(FIND_TX_PORT, sendbuffer, length, TARGET_IP, FIND_TX_PORT);
		retval = A615NetSendto(socketfdFind, sendbuffer, findNetAddr, length);

		if (-1 == retval)
		{
			printf("send request error!\n");
			Sleep(20);
		}
		else
		{
			break;
		}
	}
	//time(&begintime);

	//设置为非阻塞
	//if (SOCKET_ERROR == setsockopt(socketfdFind, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(timeOut)))
	//{
	//	printf("initFindOpAddr operation setsockopt Error!\n");

	//	return;
	//}

	while (1)
	{
#if 0		//2022-2-11 add   弃用
		time(&endtime);
		double time_out = difftime(begintime, endtime);
		printf("time out : %d.\n", time_out);
		if(time_out > 60)
		{
			printf("find op timeout(60s). find finish!\n");
			return Find_Ok;
		}
#endif
		
		//for (int i = 0; i < RE_TRANS_TIMES; i++)
		//{
			//recvretval = AfdxRecvMsg(FIND_RX_PORT, recvbuffer, &Clientlen, &srcIp, &srcUdp);
			//printf("Begin receive!\n");
			recvretval = A615NetRecvfrom(socketfdFind, recvbuffer, RECV_DATA_BUFFER_SIZE, &findRecvNetAddr);
			//printf("Receive over!\n");
			if(0 <= recvretval)
			{
				if (recvbuffer[0] == 0 && recvbuffer[1] == 2/*&&recvbuffer[recvretval-1]==0x10*/)
				{
					
					FINDClientRecData(recvbuffer + 2, targets);
					
					printf("FINDRequest numf = %d.\n", numf);
					if (numf > TAR_DEV_MAX_NUM)
					{
						return TAR_DEV_MAX_NUM;
					}
					memset(targets->targets_info[numf].ip_addr, 0, 20);
					memcpy(targets->targets_info[numf].ip_addr, inet_ntoa(findRecvNetAddr.cAddr.sin_addr), strlen(inet_ntoa(findRecvNetAddr.cAddr.sin_addr)));
					//ip_uint2string(srcIp, targets->targets_info[numf].ip_addr);
					//targets->device_num++;
					printf("target IP: %s.\n", targets->targets_info[numf].ip_addr);
					printf("target IP: No. %d, %s.\n", numf, targets->targets_info[numf].identifier);
					printf("target IP: No. %d, %s.\n", numf, targets->targets_info[numf].position);
					numf++;
					targets->device_num = numf;
					printf("FINDRequest targets->device_num = %d", targets->device_num);
					//return Find_Ok;
					//Sleep(20);
					continue;
				}
				else
				{
					printf("recv data error!");
				}
			}
			else
			{
				printf("find op timeout. find finish!\n");
				return Find_Ok;
			}
		//}
#if 0			//2022-2-11 add 不调用该函数执行的逻辑
		if (recvretval != 0)
		{
			//重新发送请求
			recvretval = FINDClientRetransmission(sendbuffer, length, targets);

            if(0 == recvretval)
            {
                printf("Find Succeed! quit!\n");

                return 0;
            }
            else
            {
                printf("Find failed! quit!\n");

                return recvretval;
            }
		}
#endif
		
	}

	return 0;
}
	

