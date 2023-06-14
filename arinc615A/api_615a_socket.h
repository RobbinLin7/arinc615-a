///*********************
// *名称：
// *描述：
// *
// *
// *作者: lcy_liuchangyang
// *日期: 2021.11
// *版本: V1.0.0
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

////函数声明

//#include "arinc615_struct_define.h"
//#include "arinc615_global_var_define.h"

///**
//* @brief 615A状态文件回调函数类型定义
//*
//* @param [out]     cb_status            615A协议层收到目标端发送的状态文件中断后，
//*										投递给上层应用的状态信息参数
//*										具体的状态信息详见a615_status_t结构体。
//*
//* @return          ::A615_target_find A pointer to the ::A615_target_find data
//*                                  structure or NULL in case of error.
//*
//*/
//typedef bool_t(*a615_status_t_callback_func_t)(void * input, a615_status_t *cb_status);

///**
//* @brief 初始化底层网络，申请615a操作相关资源。
//*		该函数接口作为615A加载端协议程序的驱动入口,负责整个协议运行时资源的初始化，
//*		必需在其他API接口函数之前被调用，否则将导致API功能异常。
//*
//* @param [in]      net_type：输入参数，底层通信网络类型。目前仅支持基于socket通信，基于AFDX通信两种。
//* @param [in]      network_info：输入参数，根据指定的底层通信网络类型，传入对应的网络配置相关数据结构
//*								例如通信网络类型选择AFDX网络，传入afdx_cfg_info_t结构体指针。
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：操作执行结果，成功或者失败。若失败，可通过err_code查看具体错误码类型。
//*
//*/
//PREG_API_EXPORT int a615_init(uint16_t net_type, void *network_info, int16_t* err_code);

///**
//* @brief 销毁操作中申请的资源。
//*		该函数接口作为615A加载端协议程序的退出接口，与初始化接口配套使用，
//*		负责整个协议运行时资源的释放和销毁，在协议程序退出时被调用，
//*		若程序退出时未正确调用该接口，可能会影响615A协议程序的再次使用。
//*
//* @param [in]      net_type：输入参数，底层通信网络类型。
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：操作执行结果，成功或者失败。若失败，可通过err_code查看具体错误码类型。
//*
//*/
//PREG_API_EXPORT uint32_t a615_exit(uint16_t net_type, int16_t *err_code);

///**
//* @brief 对初始化配置的目标端执行find操作。
//*		若初始化时选择AFDX通信网络，则该接口在使用时，仅支持find初始化配置的AFDX网络中的目标端设备。
//*		Find操作获取目标端设备信息，由应用层完成对应目标端设备的注册。
//*
//* @param [in]      net_type：输入参数，底层通信网络类型。
//* @param [in]	   dest_ip_addr:设备端IP,使用点分十进制表示IP地址
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：find操作成功，返回指向a615_targets_find_list_t结构体的指针
//*					若find失败，返回null.
//*
//*/
//PREG_API_EXPORT int a615_find_op(a615_targets_find_list_t *targets_info, int16_t *err_code);

///**
//* @brief 获取指定目标端设备的Information信息。
//*		该接口用于获取配置的目标端设备的Information信息。
//*		在upload、media_download、operator_download操作接口之前调用。
//*
//* @param [in]      fptr：输入参数，回调函数指针。用于向应用层传递LCS状态文件的相关信息。
//* @param [in]      client_id：输入参数，客户端标识由“identifier_position”构成。
//* @param [in]	   dest_ip_addr:设备端IP,使用点分十进制表示IP地址
//* @param [out]     target_info ：输出参数，指针指向information获取的设备信息列表a615_target_info_t结构体。
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：操作执行结果，成功或者失败。若失败，可通过err_code查看具体错误码类型。
//*
//*/
//PREG_API_EXPORT int a615_information_op(a615_status_t_callback_func_t fptr, char *client_id, char *dest_ip_addr,
//	a615_target_info_t *target_info, int16_t *err_code);

///**
//* @brief 选择初始化配置的目标端执行upload操作。
//*		该接口实现对指定设备上传指定的数据文件
//*		数据文件以a615_data_file_list_t结构体形式作为输入参数，传给该接口。
//*		协议层启动upload操作时，会先根据a615_data_file_list_t结构体中携带的数据文件信息生成对应的LUH文件，再启动upload交互过程。
//*		该接口暂不支持并发调用。即每次只能对一个目标端设备进行上传操作。
//*
//* @param [in]      fptr：输入参数，回调函数指针。用于向应用层传递LCS状态文件的相关信息。
//* @param [in]      client_id：输入参数，客户端标识由“identifier_position”构成。
//* @param [in]      load_file：输入参数，生成LUH文件时所需的数据文件列表信息。
//* @param [in]	   dest_ip_addr:设备端IP,使用点分十进制表示IP地址
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：操作执行结果，成功或者失败。若失败，可通过err_code查看具体错误码类型。
//*
//*/
//PREG_API_EXPORT int a615_upload_op(void *input, a615_status_t_callback_func_t fptr, char *dest_ip_addr,
//	a615_data_file_list_t load_file, char *client_id, int16_t *err_code);

///**
//* @brief 选择初始化配置的目标端执行介质定义下载操作。
//*
//* @param [in]      fptr：输入参数，回调函数指针。用于向应用层传递LNS状态文件的相关信息。
//* @param [in]      client_id：输入参数，客户端标识由“identifier_position”构成。
//* @param [in]	   dest_ip_addr:设备端IP,使用点分十进制表示IP地址
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：操作执行结果，成功或者失败。若失败，可通过err_code查看具体错误码类型。
//*
//*/
//PREG_API_EXPORT int arinc615_media_download_op(void *input, a615_status_t_callback_func_t fptr, char *client_id, char *dest_ip_addr,
//	a615_down_file_list_t file_list, int16_t *err_code);

///**
//* @brief 选择初始化配置的目标端执行介质定义下载操作。
//*
//* @param [in]      fptr：输入参数，回调函数指针。用于向应用层传递LNS状态文件的相关信息。
//* @param [in]      client_id：输入参数，客户端标识由“identifier_position”构成。
//* @param [in]      stage ：输入参数。指示当前是下载操作的哪个阶段。1表示初始化阶段；2表示数据传输阶段。
//* @param [in]      file_list：数据传输阶段时，作为输入参数，表示操作者选择需要下载的文件列表信息。
//* @param [in]	   dest_ip_addr:设备端IP,使用点分十进制表示IP地址
//* @param [out]     file_list：初始化阶段时，作为输出参数，表示目标端设备上可供下载的文件列表信息。
//* @param [out]     err_code：输出参数，错误码，详见2.1.1节宏定义。
//* @return          返回值：操作执行结果，成功或者失败。若失败，可通过err_code查看具体错误码类型。
//*
//*/
//PREG_API_EXPORT int arinc615_operator_download_op(void *input, a615_status_t_callback_func_t fptr, char *client_id,
//	char *dest_ip_addr, uint8_t stage, int16_t *err_code, a615_down_file_list_t *file_list);


///**
//* @brief 发送终止包。
//*
//* @param [in]      dest_ip_addr：目标端IP地址
//* @return          返回值：发送的数据包长度。
//*
//*/
//PREG_API_EXPORT int arinc615_abort_op(char abort_code);

///**
//* @brief 获取版本号
//*
//* @return          返回值：返回当前版本号。
//*
//*/
//PREG_API_EXPORT uint32_t arinc615_get_part_ver(void);

//#ifdef  __cplusplus
//}
//#endif

//#endif

