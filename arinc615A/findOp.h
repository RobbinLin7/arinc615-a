#ifndef FIND_OP_H
#define FIND_OP_H

#pragma once

#include "arinc615_global_var_define.h"

#include "arinc615_struct_define.h"

/**
* @brief find�������������
*
* @param [out]     requestbuffer��������ַ����ĵ�ַ
* @return          ����ֵ���ַ������ȡ�
*
*/
int FINDSetRequest(char* requestbuffer/*���������*/);

/**
* @brief find�����������ض˻ظ�������
*
* @param [in]      recvbuffer:Ҫ����������
* @param [out]     targets�����ݽ�������Ҫ����Ľṹ��
* @return          ����ֵ��none
*
*/
void  FINDClientRecData(char* recvbuffer, a615_targets_find_list_t *targets);

/**
* @brief find�����������·���
*
* @param [in]      SendBuffer:Ҫ���·��͵�����
* @param [in]      len�������������ַ�������
* @param [out]     targets�����ݽ�������Ҫ����Ľṹ��
* @return          ����ֵ��none
*
*/
int FINDClientRetransmission(char* SendBuffer, int len, a615_targets_find_list_t *targets);

/**
* @brief find�����������󣬽��ջظ�������
*
* @param [out]     targets�����ݽ�������Ҫ����Ľṹ��
* @return          ����ֵ��none
*
*/
int FINDRequest(a615_targets_find_list_t *targets);

#endif //FIND_OP_H

