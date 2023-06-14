#include "getStatus.h"

#include "tftpClient.h"

#include <stdio.h>
#include <string.h>
#include <io.h>

void getStatusFile(void *n)
{
	//����LUS���൱�ڷ�����
	

	int retVal = 0;

	//int fileNum = 0;

	int recvWRQLen = 0;

    int access_ret = 0;

	char recvBuf[RECV_DATA_BUFFER_SIZE] = { 0 };

	char fileStatus[FILE_NAME_BUFFER_SIZE] = { 0 };

	char fileStatusPath[FILE_PATH_BUFFER_SIZE] = { 0 };

	if (n != NULL)
	{
		//������
		statusNetAddr.cAddr.sin_family = AF_INET;
		statusNetAddr.cAddr.sin_port = htons(HOST_STATUS_SEND_PORT);
		statusNetAddr.cAddr.sin_addr.s_addr = inet_addr((char *)n);
	}
	else
	{
		printf("dest ip is invalid!\n");

		return ;
	}

#if AFDX_NETWORK
	NetAddr sendAddr;
	NetAddr recvAddr;
	SOCKET sockfd = 0;              // point001--- is will problem!!!
	retVal = A615NetSockAddrInit(&sendAddr, STATUS_TX, SERVER_ADDR);

	if (retVal != 0)
	{
		printf("getStatusFile:: A615NetSockAddrInit error.");
		return;
	}

	retVal = A615NetSockAddrInit(&recvAddr, STATUS_RX, SERVER_ADDR);

	if (retVal != 0)
	{
		printf("getStatusFile:: A615NetSockAddrInit error.");
		return;
	}
#endif
	while (1)
	{

        if(backflag == 0)
        {
            printf("file trans finished, recv L*S file STOP!\n");
            break;

        }
		//����L*Sд����
		recvWRQLen = A615NetRecvfrom(socketfdStatus, recvBuf, MAXBUFLEN - 1, &statusNetAddr);

		if (-1 != recvWRQLen)
		{

			if (recvBuf[1] == 2)
			{
				//�ó��ļ���
				for (int i = 2; recvBuf[i] != 0; i++)
				{
					fileStatus[i - 2] = recvBuf[i];
				}

				//ƴ�ļ�·��
				//strcpy(fileStatusPath, getFolderPath);

				//fileStatusPath[strlen(fileStatusPath)] = '/';

				sprintf(fileStatusPath, "%s\\%s", getFolderPath, fileStatus);

                printf("status file psth and name: %s.\n", fileStatusPath);

                //���ļ�����,��ɾ�����ļ�
                access_ret = _access(fileStatusPath, 0);
                if(0 == access_ret)
                {
                    if(0 == remove(fileStatusPath))
                        printf("remove file: %s OK.\n\n\n", fileStatusPath);
                    else
                        printf("remove file: %s ERROR.\n\n\n", fileStatusPath);
                }
                else
                {
                    printf(" file: %s not exist!\n\n", fileStatusPath);
                }
                //�ڸú������ж��Ƿ���abort����,������abort���ݰ�
				retVal = A615NetServerPUT(socketfdStatus, statusNetAddr, fileStatusPath, statusNetAddr);

				if (0 > retVal)
				{
					printf("getStatusFile:: recv Status file error!\n");
				}
			}
		}
		//Sleep(10);
        if(backflag == 0)
        {
            printf("L*S thread ID: 0x%x.\n\n", GetCurrentThread());
            printf("file trans finished, recv L*S file STOP!\n");
            break;

		}

        //��д����ʱ�������÷�֧
        if(1 == request_pck_timeout)
        {
            printf("L*S thread ID: 0x%x.\n\n", GetCurrentThread());
            printf("because TIMEOUT, recv L*S file STOP!\n");
            return;
        }
	}

	return;
}



