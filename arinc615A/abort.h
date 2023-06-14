#ifndef ABORT_H
#define ABORT_H


#ifdef __cplusplus
extern "C" {
#endif

#include<string.h>
#include <stdio.h>

#include "arinc615_global_var_define.h"
#include "utility.h"
#include "globalConfig.h"

#define MAX_BUF_16_SIZE_T			16

/**
* @brief ������ֹ����
*
* @param [out]      pk���������������abort���ĵ�ַ��
* @param [in]      abort_code����ֹ��,ȡֵΪ0x3/0x4/0x5
* @return          ����ֵ����ֹ�����ȣ�
*							-1��ָ��Ϊ�գ�-2����ֹ�����
*
*/
int create_abort_packet(char* pk, const unsigned char abort_code);


/**
* @brief ������ֹ����
*
* @param [in]      dest_ip_addr��Ŀ���IP��ַ
* @return          ����ֵ�����͵����ݰ����ȡ�
*
*/
int send_abort_packet(SOCKET sockfd, NetAddr addrInfo, const unsigned char abort_code);

#ifdef  __cplusplus
}
#endif

#endif

