#ifndef ARINC615_STRUCT_DEFINE_H
#define ARINC615_STRUCT_DEFINE_H

#include "arinc615_global_var_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// add define struct that you need

#define MAX_FILENAME_LEN			64			//最大文件名长度


/**
* @brief 该数据结构用于表示目标端设备的硬件信息。
*/
typedef struct find_packet {
	char identifier[ID_MAX_LEN];				/* 目标标识,用ASCII字符表示，最多包含16(含‘\0’字符串结束符)个字符 */
	char type_name[TYPE_NM_MAX_LEN];			/* 目标类型名称,用ASCII字符表示，最多包含9(含‘\0’字符串结束符)个字符 */
	char position[POS_MAX_LEN];					/* 目标位置,用ASCII字符表示，最多包含9(含‘\0’字符串结束符)个字符 */
	char literal_name[LITERAL_NM_MAX_LEN];		/* 文字名称,用ASCII字符表示，最多包含21(含‘\0’字符串结束符)个字符 */
	char manufacture_code[MNF_CODE_MAX_LEN];	/* 制造商代码,用ASCII字符表示，最多包含9(含‘\0’字符串结束符)个字符 */
	char terminal_code[TMN_CODE_MAX_LEN];		/* 终结符，‘\0’ */
	char ip_addr[IP_ADDR_MAX_LEN];				/* 点分十进制表示的IP地址,最多占16字节，包含终结符 */
}a615_target_find_info_t;

/**
* @brief 该数据结构用于存储find操作发现的目标端列表信息。
*/
typedef struct {
	unsigned int device_num; 						/* find操作发现的设备数量，上限为TAR_DEV_MAX_NUM */
	a615_target_find_info_t *targets_info;	/* 被发现目标设备的列表信息 */
}a615_targets_find_list_t;

/**
* @brief 该结构用于定义状态文件中文件列表信息。
*/
typedef struct {
	uint8_t file_name_len;					/* 指示文件名称的字符个数，取值范围1~255 */
	uint8_t file_name[FILE_NAME_MAX_LEN];	/* 文件名称，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
	a615_status_t_codes_e file_status; 		/* 文件状态，具体状态定义详见a615_status_t_codes_e结构 */
} a615_file_status_t;


/**
* @brief 该结构用于存储状态文件中的关键信息，用于协议层通过状态文件回调函数
*		 以输出参数的形式向应用层提交状态文件信息。
*/
typedef struct {
	uint8_t client_id[CLIENT_ID_MAX_LEN];	/* 客户端标识由“identifier_position”构成 */
	a615_status_t_codes_e status_code;		/* 状态码，详见a615_status_t_codes_e结构定义 */
	uint8_t description[FILE_NAME_MAX_LEN];	/* 状态描述符 */
	uint16_t counter;						/* 状态文件计数，表示当前是第几个状态文件 */
	uint8_t load_ratio[ARINC615_LOAD_RATIO_LENGTH]; /* 上传或者下载操作时，加载的比例 */
	uint16_t num_of_files;					/* 上传操作时包含的头文件数量，或者下载操作包含的数据文件数量 */
	a615_file_status_t *load_files;			/* 上传操作时头文件的状态列表信息，或者下载操作包含的数据文件数量 */
} a615_status_t;


/** A callback function for displaying the client status. */
/**
 * @brief This callback function is used by the information, upload
 * and download operations. Through the return value the 615A-3 master gives
 * the upper layer the possibility to abort the current operation if needed.
 *
 * @param [in]      cb_status           A pointer to the ::a615_status data structure.
 *
 * @return          ::bool_t
 * @retval          ::ARINC615_TRUE           Abort the current operation.
 * @retval          ::ARINC615_FALSE          Do not abort the current operation.
 *
 */
//typedef bool_t(*a615_status_func)(a615_status_t* cb_status);

/**
* @brief 该结构用于描述LCL文件中部件相关信息。
*/
typedef  struct {
	uint8_t parts_num_len; 							/* 指示部件编号的字符个数，取值范围1~255 */
	char parts_num[FILE_NAME_MAX_LEN];				/* 部件编号，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
	uint8_t amend_len;								/* 修订字符的长度 ，取值范围0~255*/
	char amend[FILE_NAME_MAX_LEN];					/* 修订字符的内容，用ASCII字符表示，包含0~255(含‘\0’字符串结束符)个字符  */
	uint8_t part_desig_text_len;					/* 指示部件名称的字符个数，取值范围1~255 */
	char part_desig_text[FILE_NAME_MAX_LEN];		/* 部件名称的内容，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
}a615_parts_info_t;

/**
* @brief 该结构体用于存储LCL文件中目标设备的硬件相关的信息。
*/
typedef  struct {
	uint8_t lit_name_len;							/* 文字名称字段包含的字符数量，取值范围1~255 */
	char literal_name[FILE_NAME_MAX_LEN];			/* 文字名称，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
	uint8_t se_num_len; 							/* 序列号字段字符数量，取值范围1~255 */
	char serial_num[FILE_NAME_MAX_LEN];				/* 硬件序列号，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
	uint16_t parts_num; 							/* 部件编号列表数量，表示a615_parts_info_t结构体的数量 */
	a615_parts_info_t *parts;								/* 部件编号列表，指针指向部件编号a615_parts_info_t结构体 */
}a615_hardware_info_t;

/**
* @brief 该结构体用于存储LCL文件的字段信息。在information操作接口中调用，作为输出参数
*		 向应用层传递目标端设备的information信息。
*/
typedef struct {
	uint32_t file_len;					/* 表示LCL文件的大小（字节为单位统计），含文件大小字段 */
	uint16_t pro_ver;					/* 协议版本，用2字节的ASCII码字符表示，615A-3对应的版本为“A4" */
	uint16_t hw_num; 					/* 目标硬件数量，表示a615_hardware_info_t结构体的数量 */
	a615_hardware_info_t *hws;			/* 目标硬件列表，指针指向目标硬件a615_hardware_info_t结构体 */
}a615_target_info_t;


//LUH文件 结构体
/**
* @brief 该结构体用于存储文件的详细信息
*/
typedef struct{
	char path[FILE_NAME_MAX_LEN];		/* 存放数据文件的存储路径，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
	char name[FILE_NAME_MAX_LEN];		/* 数据文件名称，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符 */
	short check_value_type;				/* 数据文件校验类型，《类型1：8-bit CRC校验，校验结果占2字节》；《类型2：16-bit CRC校验，校验结果占2字节》
										《类型3：32-bit CRC校验，校验结果占4字节》；《类型4：MD5校验，校验结果占16字节》;《类型5：SHA-1校验，校验结果占20字节》*/
	unsigned long file_len;				/* 数据文件大小，以字节为单位统计 */
}a615_data_file_t;

/**
* @brief 该数据结构用于定义data file列表信息
*		 应用层调用upload操作接口时，通过a615_data_file_list_t结构体指针
*		 传入upload操作涉及的数据文件基本信息
*		 协议层获取到数据文件列表信息后，生成对应的LUH文件，供协议层使用。
*/
typedef struct {
	int file_num;			/* 数据文件的数量 */
	a615_data_file_t data_file[LOAD_FILE_MAX_NUM];	/* 存储数据文件列表基本信息的结构体数组，一个目标端设备最多支持50个数据文件 */
}a615_data_file_list_t;


typedef struct LUH_FILE_INOF {
	a615_data_file_list_t file_list;

	uint16_t load_PN;
	uint16_t load_type_description;

	char head_file_name[FILE_NAME_BUFFER_SIZE];

	char target_hw_ID[FILE_NAME_BUFFER_SIZE];

}luh_info;

/**
* @brief 该结构体用于定义下载操作时，传输的数据文件列表信息
*		 操作者自定义下载时，协议层将获取的目标端设备的文件列表信息
*		 以a615_down_file_list_t结构体指针的形式传递给应用层
*		 用户选择好需要下载的文件后
*		 又以a615_down_file_list_t结构体指针的形式传回协议层，供协议层使用。
*/
typedef struct {
	uint16_t file_num;	/* 数据文件的数量 */
	char file_name[LOAD_FILE_MAX_NUM][FILE_NAME_MAX_LEN];	/* 存储数据文件名称，用ASCII字符表示，包含1~255(含‘\0’字符串结束符)个字符，最多存储50个数据文件 */
}a615_down_file_list_t;


typedef struct FILE_INFO
{
	char filename[FILE_NAME_BUFFER_SIZE];		//文件名
	char file_path[FILE_NAME_BUFFER_SIZE];	//文件路径
	uint32_t file_len;		//文件长度
}file_info;

typedef struct FILE_DOWNLOAD
{
	uint16_t file_number;
	file_info *file_list;
}file_download;

typedef struct {
	char path[FILE_NAME_BUFFER_SIZE];
	char name[FILE_NAME_BUFFER_SIZE];

	short check_value_type;     

	unsigned long file_len;
}OneFile;

typedef struct {
	int file_number;
	OneFile oneFile[MAX_TRANS_FILE_NUMBER];
}FileList;

/**
* @brief 该结构体用于定义初始化配置AFDX通信网络时的VLID的基本信息。
*/
typedef struct {
	uint16_t vlid_num;    		/* afdx初始化配置时，应用层传入的需要配置的vlid的数量 */
	uint16_t *vlid_list;		/* vlid列表信息 */
}afdx_vl_list_cfg_t;

/**
* @brief 该结构体用于存储基本的端口信息。在afdx初始化配置时使用。
*/
typedef struct
{
	uint32_t port_id;	/* afdx初始化配置时，应用层传入的需要配置的端口ID信息 */
	uint16_t vlid;		/* afdx初始化配置时，应用层传入的与端口ID绑定的vlid信息 */
}afdx_port_cfg_info_t;

/* port端口配置 */
typedef struct
{
	unsigned int port_id;			/* 端口Id，取任意值，各端口不可重复 */
	unsigned short vlId;			/* 端口使用的VLId，取值VL配置中的VLId */
}port_cfg_info;

/**
* @brief 该结构体用于存储afdx初始化配置时端口列表信息。
*/
typedef struct {
	uint16_t port_num;			/* afdx初始化配置时，应用层传入的需要配置的端口的数量 */
	port_cfg_info *port_list;	/* 端口列表信息 */
}afdx_port_list_cfg_t;

/**
* @brief 该结构体存储与所有目标端建立AFDX通信的基本信息。
*/
typedef struct
{
	uint32_t tar_ip;  					/* 目标端设备的IP地址信息 */
	afdx_vl_list_cfg_t tx_vl_info; 		/* afdx初始化配置时，应用层传入的发送vl列表信息 */
	afdx_vl_list_cfg_t rx_vl_info;		/* afdx初始化配置时，应用层传入的接收vl列表信息 */
	afdx_port_list_cfg_t tx_port_list;	/* afdx初始化配置时，应用层传入的发送端口列表信息 */
	afdx_port_list_cfg_t rx_port_list;	/* afdx初始化配置时，应用层传入的接收端口列表信息 */
}afdx_tar_cfg_t;

/**
* @brief 该结构体存储应用层下发的AFDX通信的初始化配置信息。
*		 应用层调用初始化接口时，使用该结构体作为输入参数，传递AFDX网络的配置参数
*/
typedef struct
{
	uint32_t laod_ip;  				/* 加载端设备的IP地址信息 */
	uint16_t targets_num;				/* 配置时目标端设备的数量 */
	afdx_tar_cfg_t *targets_list;		/* 与目标端设备建立AFDX通信的配置信息 */
}afdx_cfg_info_t;

/* vl列表配置 */
typedef struct{
    unsigned short vlid_num;    /* vlid的数量 */
    unsigned short * vlid;      /* vl列表 */
}vl_list_cfg_info;



typedef struct{
    unsigned short port_num;    /* 端口的数量 */
    port_cfg_info * port_list;      /* 端口列表 */
}port_list_cfg_info;

/* 目标端设备列表配置 */
typedef struct
{
    unsigned int tar_ip;            /* ip地址 */
    vl_list_cfg_info tx_vl_info;    /* 发送vl信息 */
    vl_list_cfg_info rx_vl_info;    /* 接收vl信息 */
    port_list_cfg_info tx_port_list;/* 端口列表 */
    port_list_cfg_info rx_port_list;/* 端口列表 */
}tar_cfg_info;

/* 全网设备列表配置 */
typedef struct
{
    unsigned int laod_ip;           /* 加载端ip地址 */
    unsigned int targets_num;		/* 目标端设备数量 */
    tar_cfg_info * targets_list;    /* 目标端设备列表 */
}afdx_cfg_info;
#pragma pack()

#ifdef  __cplusplus
}
#endif

#endif //ARINC615_STRUCT_DEFINE_H

