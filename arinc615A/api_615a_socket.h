///*********************
// *���ƣ�
// *������
// *
// *
// *����: lcy_liuchangyang
// *����: 2021.11
// *�汾: V1.0.0
// *
// ********************/

//#ifndef _API_615A_SOCKET_H_
//#define _API_615A_SOCKET_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

//#ifdef ARINC_SOCKET_API
//#define PREG_API_EXPORT __declspec(dllexport)
//#else
//#define PREG_API_EXPORT __declspec(dllimport)
//#endif //PREG_API

////��������

//#include "arinc615_struct_define.h"
//#include "arinc615_global_var_define.h"

///**
//* @brief 615A״̬�ļ��ص��������Ͷ���
//*
//* @param [out]     cb_status            615AЭ����յ�Ŀ��˷��͵�״̬�ļ��жϺ�
//*										Ͷ�ݸ��ϲ�Ӧ�õ�״̬��Ϣ����
//*										�����״̬��Ϣ���a615_status_t�ṹ�塣
//*
//* @return          ::A615_target_find A pointer to the ::A615_target_find data
//*                                  structure or NULL in case of error.
//*
//*/
//typedef bool_t(*a615_status_t_callback_func_t)(void * input, a615_status_t *cb_status);

///**
//* @brief ��ʼ���ײ����磬����615a���������Դ��
//*		�ú����ӿ���Ϊ615A���ض�Э�������������,��������Э������ʱ��Դ�ĳ�ʼ����
//*		����������API�ӿں���֮ǰ�����ã����򽫵���API�����쳣��
//*
//* @param [in]      net_type������������ײ�ͨ���������͡�Ŀǰ��֧�ֻ���socketͨ�ţ�����AFDXͨ�����֡�
//* @param [in]      network_info���������������ָ���ĵײ�ͨ���������ͣ������Ӧ����������������ݽṹ
//*								����ͨ����������ѡ��AFDX���磬����afdx_cfg_info_t�ṹ��ָ�롣
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ������ִ�н�����ɹ�����ʧ�ܡ���ʧ�ܣ���ͨ��err_code�鿴������������͡�
//*
//*/
//PREG_API_EXPORT int a615_init(uint16_t net_type, void *network_info, int16_t* err_code);

///**
//* @brief ���ٲ������������Դ��
//*		�ú����ӿ���Ϊ615A���ض�Э�������˳��ӿڣ����ʼ���ӿ�����ʹ�ã�
//*		��������Э������ʱ��Դ���ͷź����٣���Э������˳�ʱ�����ã�
//*		�������˳�ʱδ��ȷ���øýӿڣ����ܻ�Ӱ��615AЭ�������ٴ�ʹ�á�
//*
//* @param [in]      net_type������������ײ�ͨ���������͡�
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ������ִ�н�����ɹ�����ʧ�ܡ���ʧ�ܣ���ͨ��err_code�鿴������������͡�
//*
//*/
//PREG_API_EXPORT uint32_t a615_exit(uint16_t net_type, int16_t *err_code);

///**
//* @brief �Գ�ʼ�����õ�Ŀ���ִ��find������
//*		����ʼ��ʱѡ��AFDXͨ�����磬��ýӿ���ʹ��ʱ����֧��find��ʼ�����õ�AFDX�����е�Ŀ����豸��
//*		Find������ȡĿ����豸��Ϣ����Ӧ�ò���ɶ�ӦĿ����豸��ע�ᡣ
//*
//* @param [in]      net_type������������ײ�ͨ���������͡�
//* @param [in]	   dest_ip_addr:�豸��IP,ʹ�õ��ʮ���Ʊ�ʾIP��ַ
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ��find�����ɹ�������ָ��a615_targets_find_list_t�ṹ���ָ��
//*					��findʧ�ܣ�����null.
//*
//*/
//PREG_API_EXPORT int a615_find_op(a615_targets_find_list_t *targets_info, int16_t *err_code);

///**
//* @brief ��ȡָ��Ŀ����豸��Information��Ϣ��
//*		�ýӿ����ڻ�ȡ���õ�Ŀ����豸��Information��Ϣ��
//*		��upload��media_download��operator_download�����ӿ�֮ǰ���á�
//*
//* @param [in]      fptr������������ص�����ָ�롣������Ӧ�ò㴫��LCS״̬�ļ��������Ϣ��
//* @param [in]      client_id������������ͻ��˱�ʶ�ɡ�identifier_position�����ɡ�
//* @param [in]	   dest_ip_addr:�豸��IP,ʹ�õ��ʮ���Ʊ�ʾIP��ַ
//* @param [out]     target_info �����������ָ��ָ��information��ȡ���豸��Ϣ�б�a615_target_info_t�ṹ�塣
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ������ִ�н�����ɹ�����ʧ�ܡ���ʧ�ܣ���ͨ��err_code�鿴������������͡�
//*
//*/
//PREG_API_EXPORT int a615_information_op(a615_status_t_callback_func_t fptr, char *client_id, char *dest_ip_addr,
//	a615_target_info_t *target_info, int16_t *err_code);

///**
//* @brief ѡ���ʼ�����õ�Ŀ���ִ��upload������
//*		�ýӿ�ʵ�ֶ�ָ���豸�ϴ�ָ���������ļ�
//*		�����ļ���a615_data_file_list_t�ṹ����ʽ��Ϊ��������������ýӿڡ�
//*		Э�������upload����ʱ�����ȸ���a615_data_file_list_t�ṹ����Я���������ļ���Ϣ���ɶ�Ӧ��LUH�ļ���������upload�������̡�
//*		�ýӿ��ݲ�֧�ֲ������á���ÿ��ֻ�ܶ�һ��Ŀ����豸�����ϴ�������
//*
//* @param [in]      fptr������������ص�����ָ�롣������Ӧ�ò㴫��LCS״̬�ļ��������Ϣ��
//* @param [in]      client_id������������ͻ��˱�ʶ�ɡ�identifier_position�����ɡ�
//* @param [in]      load_file���������������LUH�ļ�ʱ����������ļ��б���Ϣ��
//* @param [in]	   dest_ip_addr:�豸��IP,ʹ�õ��ʮ���Ʊ�ʾIP��ַ
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ������ִ�н�����ɹ�����ʧ�ܡ���ʧ�ܣ���ͨ��err_code�鿴������������͡�
//*
//*/
//PREG_API_EXPORT int a615_upload_op(void *input, a615_status_t_callback_func_t fptr, char *dest_ip_addr,
//	a615_data_file_list_t load_file, char *client_id, int16_t *err_code);

///**
//* @brief ѡ���ʼ�����õ�Ŀ���ִ�н��ʶ������ز�����
//*
//* @param [in]      fptr������������ص�����ָ�롣������Ӧ�ò㴫��LNS״̬�ļ��������Ϣ��
//* @param [in]      client_id������������ͻ��˱�ʶ�ɡ�identifier_position�����ɡ�
//* @param [in]	   dest_ip_addr:�豸��IP,ʹ�õ��ʮ���Ʊ�ʾIP��ַ
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ������ִ�н�����ɹ�����ʧ�ܡ���ʧ�ܣ���ͨ��err_code�鿴������������͡�
//*
//*/
//PREG_API_EXPORT int arinc615_media_download_op(void *input, a615_status_t_callback_func_t fptr, char *client_id, char *dest_ip_addr,
//	a615_down_file_list_t file_list, int16_t *err_code);

///**
//* @brief ѡ���ʼ�����õ�Ŀ���ִ�н��ʶ������ز�����
//*
//* @param [in]      fptr������������ص�����ָ�롣������Ӧ�ò㴫��LNS״̬�ļ��������Ϣ��
//* @param [in]      client_id������������ͻ��˱�ʶ�ɡ�identifier_position�����ɡ�
//* @param [in]      stage �����������ָʾ��ǰ�����ز������ĸ��׶Ρ�1��ʾ��ʼ���׶Σ�2��ʾ���ݴ���׶Ρ�
//* @param [in]      file_list�����ݴ���׶�ʱ����Ϊ�����������ʾ������ѡ����Ҫ���ص��ļ��б���Ϣ��
//* @param [in]	   dest_ip_addr:�豸��IP,ʹ�õ��ʮ���Ʊ�ʾIP��ַ
//* @param [out]     file_list����ʼ���׶�ʱ����Ϊ�����������ʾĿ����豸�Ͽɹ����ص��ļ��б���Ϣ��
//* @param [out]     err_code����������������룬���2.1.1�ں궨�塣
//* @return          ����ֵ������ִ�н�����ɹ�����ʧ�ܡ���ʧ�ܣ���ͨ��err_code�鿴������������͡�
//*
//*/
//PREG_API_EXPORT int arinc615_operator_download_op(void *input, a615_status_t_callback_func_t fptr, char *client_id,
//	char *dest_ip_addr, uint8_t stage, int16_t *err_code, a615_down_file_list_t *file_list);


///**
//* @brief ������ֹ����
//*
//* @param [in]      dest_ip_addr��Ŀ���IP��ַ
//* @return          ����ֵ�����͵����ݰ����ȡ�
//*
//*/
//PREG_API_EXPORT int arinc615_abort_op(char abort_code);

///**
//* @brief ��ȡ�汾��
//*
//* @return          ����ֵ�����ص�ǰ�汾�š�
//*
//*/
//PREG_API_EXPORT uint32_t arinc615_get_part_ver(void);

//#ifdef  __cplusplus
//}
//#endif

//#endif

