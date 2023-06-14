#ifndef FIND_OP_H
#define FIND_OP_H

#pragma once

#include "arinc615_global_var_define.h"

#include "arinc615_struct_define.h"

/**
* @brief find操作构造请求包
*
* @param [out]     requestbuffer：请求包字符串的地址
* @return          返回值：字符串长度。
*
*/
int FINDSetRequest(char* requestbuffer/*请求包缓存*/);

/**
* @brief find操作解析加载端回复的数据
*
* @param [in]      recvbuffer:要解析的数据
* @param [out]     targets：数据解析后需要保存的结构体
* @return          返回值：none
*
*/
void  FINDClientRecData(char* recvbuffer, a615_targets_find_list_t *targets);

/**
* @brief find操作请求重新发送
*
* @param [in]      SendBuffer:要重新发送的数据
* @param [in]      len：待发送数据字符串长度
* @param [out]     targets：数据解析后需要保存的结构体
* @return          返回值：none
*
*/
int FINDClientRetransmission(char* SendBuffer, int len, a615_targets_find_list_t *targets);

/**
* @brief find操作发送请求，接收回复并解析
*
* @param [out]     targets：数据解析后需要保存的结构体
* @return          返回值：none
*
*/
int FINDRequest(a615_targets_find_list_t *targets);

#endif //FIND_OP_H

