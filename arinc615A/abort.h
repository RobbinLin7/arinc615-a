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
* @brief 创建终止包。
*
* @param [out]      pk：输出参数，保存abort包的地址。
* @param [in]      abort_code：终止码,取值为0x3/0x4/0x5
* @return          返回值：终止包长度；
*							-1：指针为空，-2：中止码错误。
*
*/
int create_abort_packet(char* pk, const unsigned char abort_code);


/**
* @brief 发送终止包。
*
* @param [in]      dest_ip_addr：目标端IP地址
* @return          返回值：发送的数据包长度。
*
*/
int send_abort_packet(SOCKET sockfd, NetAddr addrInfo, const unsigned char abort_code);

#ifdef  __cplusplus
}
#endif

#endif

