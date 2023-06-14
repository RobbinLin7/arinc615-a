#ifndef ARINC615_STRUCT_DEFINE_H
#define ARINC615_STRUCT_DEFINE_H

#include "arinc615_global_var_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// add define struct that you need

#define MAX_FILENAME_LEN			64			//����ļ�������


/**
* @brief �����ݽṹ���ڱ�ʾĿ����豸��Ӳ����Ϣ��
*/
typedef struct find_packet {
	char identifier[ID_MAX_LEN];				/* Ŀ���ʶ,��ASCII�ַ���ʾ��������16(����\0���ַ���������)���ַ� */
	char type_name[TYPE_NM_MAX_LEN];			/* Ŀ����������,��ASCII�ַ���ʾ��������9(����\0���ַ���������)���ַ� */
	char position[POS_MAX_LEN];					/* Ŀ��λ��,��ASCII�ַ���ʾ��������9(����\0���ַ���������)���ַ� */
	char literal_name[LITERAL_NM_MAX_LEN];		/* ��������,��ASCII�ַ���ʾ��������21(����\0���ַ���������)���ַ� */
	char manufacture_code[MNF_CODE_MAX_LEN];	/* �����̴���,��ASCII�ַ���ʾ��������9(����\0���ַ���������)���ַ� */
	char terminal_code[TMN_CODE_MAX_LEN];		/* �ս������\0�� */
	char ip_addr[IP_ADDR_MAX_LEN];				/* ���ʮ���Ʊ�ʾ��IP��ַ,���ռ16�ֽڣ������ս�� */
}a615_target_find_info_t;

/**
* @brief �����ݽṹ���ڴ洢find�������ֵ�Ŀ����б���Ϣ��
*/
typedef struct {
	unsigned int device_num; 						/* find�������ֵ��豸����������ΪTAR_DEV_MAX_NUM */
	a615_target_find_info_t *targets_info;	/* ������Ŀ���豸���б���Ϣ */
}a615_targets_find_list_t;

/**
* @brief �ýṹ���ڶ���״̬�ļ����ļ��б���Ϣ��
*/
typedef struct {
	uint8_t file_name_len;					/* ָʾ�ļ����Ƶ��ַ�������ȡֵ��Χ1~255 */
	uint8_t file_name[FILE_NAME_MAX_LEN];	/* �ļ����ƣ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
	a615_status_t_codes_e file_status; 		/* �ļ�״̬������״̬�������a615_status_t_codes_e�ṹ */
} a615_file_status_t;


/**
* @brief �ýṹ���ڴ洢״̬�ļ��еĹؼ���Ϣ������Э���ͨ��״̬�ļ��ص�����
*		 �������������ʽ��Ӧ�ò��ύ״̬�ļ���Ϣ��
*/
typedef struct {
	uint8_t client_id[CLIENT_ID_MAX_LEN];	/* �ͻ��˱�ʶ�ɡ�identifier_position������ */
	a615_status_t_codes_e status_code;		/* ״̬�룬���a615_status_t_codes_e�ṹ���� */
	uint8_t description[FILE_NAME_MAX_LEN];	/* ״̬������ */
	uint16_t counter;						/* ״̬�ļ���������ʾ��ǰ�ǵڼ���״̬�ļ� */
	uint8_t load_ratio[ARINC615_LOAD_RATIO_LENGTH]; /* �ϴ��������ز���ʱ�����صı��� */
	uint16_t num_of_files;					/* �ϴ�����ʱ������ͷ�ļ��������������ز��������������ļ����� */
	a615_file_status_t *load_files;			/* �ϴ�����ʱͷ�ļ���״̬�б���Ϣ���������ز��������������ļ����� */
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
* @brief �ýṹ��������LCL�ļ��в��������Ϣ��
*/
typedef  struct {
	uint8_t parts_num_len; 							/* ָʾ������ŵ��ַ�������ȡֵ��Χ1~255 */
	char parts_num[FILE_NAME_MAX_LEN];				/* ������ţ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
	uint8_t amend_len;								/* �޶��ַ��ĳ��� ��ȡֵ��Χ0~255*/
	char amend[FILE_NAME_MAX_LEN];					/* �޶��ַ������ݣ���ASCII�ַ���ʾ������0~255(����\0���ַ���������)���ַ�  */
	uint8_t part_desig_text_len;					/* ָʾ�������Ƶ��ַ�������ȡֵ��Χ1~255 */
	char part_desig_text[FILE_NAME_MAX_LEN];		/* �������Ƶ����ݣ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
}a615_parts_info_t;

/**
* @brief �ýṹ�����ڴ洢LCL�ļ���Ŀ���豸��Ӳ����ص���Ϣ��
*/
typedef  struct {
	uint8_t lit_name_len;							/* ���������ֶΰ������ַ�������ȡֵ��Χ1~255 */
	char literal_name[FILE_NAME_MAX_LEN];			/* �������ƣ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
	uint8_t se_num_len; 							/* ���к��ֶ��ַ�������ȡֵ��Χ1~255 */
	char serial_num[FILE_NAME_MAX_LEN];				/* Ӳ�����кţ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
	uint16_t parts_num; 							/* ��������б���������ʾa615_parts_info_t�ṹ������� */
	a615_parts_info_t *parts;								/* ��������б�ָ��ָ�򲿼����a615_parts_info_t�ṹ�� */
}a615_hardware_info_t;

/**
* @brief �ýṹ�����ڴ洢LCL�ļ����ֶ���Ϣ����information�����ӿ��е��ã���Ϊ�������
*		 ��Ӧ�ò㴫��Ŀ����豸��information��Ϣ��
*/
typedef struct {
	uint32_t file_len;					/* ��ʾLCL�ļ��Ĵ�С���ֽ�Ϊ��λͳ�ƣ������ļ���С�ֶ� */
	uint16_t pro_ver;					/* Э��汾����2�ֽڵ�ASCII���ַ���ʾ��615A-3��Ӧ�İ汾Ϊ��A4" */
	uint16_t hw_num; 					/* Ŀ��Ӳ����������ʾa615_hardware_info_t�ṹ������� */
	a615_hardware_info_t *hws;			/* Ŀ��Ӳ���б�ָ��ָ��Ŀ��Ӳ��a615_hardware_info_t�ṹ�� */
}a615_target_info_t;


//LUH�ļ� �ṹ��
/**
* @brief �ýṹ�����ڴ洢�ļ�����ϸ��Ϣ
*/
typedef struct{
	char path[FILE_NAME_MAX_LEN];		/* ��������ļ��Ĵ洢·������ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
	char name[FILE_NAME_MAX_LEN];		/* �����ļ����ƣ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ� */
	short check_value_type;				/* �����ļ�У�����ͣ�������1��8-bit CRCУ�飬У����ռ2�ֽڡ���������2��16-bit CRCУ�飬У����ռ2�ֽڡ�
										������3��32-bit CRCУ�飬У����ռ4�ֽڡ���������4��MD5У�飬У����ռ16�ֽڡ�;������5��SHA-1У�飬У����ռ20�ֽڡ�*/
	unsigned long file_len;				/* �����ļ���С�����ֽ�Ϊ��λͳ�� */
}a615_data_file_t;

/**
* @brief �����ݽṹ���ڶ���data file�б���Ϣ
*		 Ӧ�ò����upload�����ӿ�ʱ��ͨ��a615_data_file_list_t�ṹ��ָ��
*		 ����upload�����漰�������ļ�������Ϣ
*		 Э����ȡ�������ļ��б���Ϣ�����ɶ�Ӧ��LUH�ļ�����Э���ʹ�á�
*/
typedef struct {
	int file_num;			/* �����ļ������� */
	a615_data_file_t data_file[LOAD_FILE_MAX_NUM];	/* �洢�����ļ��б������Ϣ�Ľṹ�����飬һ��Ŀ����豸���֧��50�������ļ� */
}a615_data_file_list_t;


typedef struct LUH_FILE_INOF {
	a615_data_file_list_t file_list;

	uint16_t load_PN;
	uint16_t load_type_description;

	char head_file_name[FILE_NAME_BUFFER_SIZE];

	char target_hw_ID[FILE_NAME_BUFFER_SIZE];

}luh_info;

/**
* @brief �ýṹ�����ڶ������ز���ʱ������������ļ��б���Ϣ
*		 �������Զ�������ʱ��Э��㽫��ȡ��Ŀ����豸���ļ��б���Ϣ
*		 ��a615_down_file_list_t�ṹ��ָ�����ʽ���ݸ�Ӧ�ò�
*		 �û�ѡ�����Ҫ���ص��ļ���
*		 ����a615_down_file_list_t�ṹ��ָ�����ʽ����Э��㣬��Э���ʹ�á�
*/
typedef struct {
	uint16_t file_num;	/* �����ļ������� */
	char file_name[LOAD_FILE_MAX_NUM][FILE_NAME_MAX_LEN];	/* �洢�����ļ����ƣ���ASCII�ַ���ʾ������1~255(����\0���ַ���������)���ַ������洢50�������ļ� */
}a615_down_file_list_t;


typedef struct FILE_INFO
{
	char filename[FILE_NAME_BUFFER_SIZE];		//�ļ���
	char file_path[FILE_NAME_BUFFER_SIZE];	//�ļ�·��
	uint32_t file_len;		//�ļ�����
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
* @brief �ýṹ�����ڶ����ʼ������AFDXͨ������ʱ��VLID�Ļ�����Ϣ��
*/
typedef struct {
	uint16_t vlid_num;    		/* afdx��ʼ������ʱ��Ӧ�ò㴫�����Ҫ���õ�vlid������ */
	uint16_t *vlid_list;		/* vlid�б���Ϣ */
}afdx_vl_list_cfg_t;

/**
* @brief �ýṹ�����ڴ洢�����Ķ˿���Ϣ����afdx��ʼ������ʱʹ�á�
*/
typedef struct
{
	uint32_t port_id;	/* afdx��ʼ������ʱ��Ӧ�ò㴫�����Ҫ���õĶ˿�ID��Ϣ */
	uint16_t vlid;		/* afdx��ʼ������ʱ��Ӧ�ò㴫�����˿�ID�󶨵�vlid��Ϣ */
}afdx_port_cfg_info_t;

/* port�˿����� */
typedef struct
{
	unsigned int port_id;			/* �˿�Id��ȡ����ֵ�����˿ڲ����ظ� */
	unsigned short vlId;			/* �˿�ʹ�õ�VLId��ȡֵVL�����е�VLId */
}port_cfg_info;

/**
* @brief �ýṹ�����ڴ洢afdx��ʼ������ʱ�˿��б���Ϣ��
*/
typedef struct {
	uint16_t port_num;			/* afdx��ʼ������ʱ��Ӧ�ò㴫�����Ҫ���õĶ˿ڵ����� */
	port_cfg_info *port_list;	/* �˿��б���Ϣ */
}afdx_port_list_cfg_t;

/**
* @brief �ýṹ��洢������Ŀ��˽���AFDXͨ�ŵĻ�����Ϣ��
*/
typedef struct
{
	uint32_t tar_ip;  					/* Ŀ����豸��IP��ַ��Ϣ */
	afdx_vl_list_cfg_t tx_vl_info; 		/* afdx��ʼ������ʱ��Ӧ�ò㴫��ķ���vl�б���Ϣ */
	afdx_vl_list_cfg_t rx_vl_info;		/* afdx��ʼ������ʱ��Ӧ�ò㴫��Ľ���vl�б���Ϣ */
	afdx_port_list_cfg_t tx_port_list;	/* afdx��ʼ������ʱ��Ӧ�ò㴫��ķ��Ͷ˿��б���Ϣ */
	afdx_port_list_cfg_t rx_port_list;	/* afdx��ʼ������ʱ��Ӧ�ò㴫��Ľ��ն˿��б���Ϣ */
}afdx_tar_cfg_t;

/**
* @brief �ýṹ��洢Ӧ�ò��·���AFDXͨ�ŵĳ�ʼ��������Ϣ��
*		 Ӧ�ò���ó�ʼ���ӿ�ʱ��ʹ�øýṹ����Ϊ�������������AFDX��������ò���
*/
typedef struct
{
	uint32_t laod_ip;  				/* ���ض��豸��IP��ַ��Ϣ */
	uint16_t targets_num;				/* ����ʱĿ����豸������ */
	afdx_tar_cfg_t *targets_list;		/* ��Ŀ����豸����AFDXͨ�ŵ�������Ϣ */
}afdx_cfg_info_t;

/* vl�б����� */
typedef struct{
    unsigned short vlid_num;    /* vlid������ */
    unsigned short * vlid;      /* vl�б� */
}vl_list_cfg_info;



typedef struct{
    unsigned short port_num;    /* �˿ڵ����� */
    port_cfg_info * port_list;      /* �˿��б� */
}port_list_cfg_info;

/* Ŀ����豸�б����� */
typedef struct
{
    unsigned int tar_ip;            /* ip��ַ */
    vl_list_cfg_info tx_vl_info;    /* ����vl��Ϣ */
    vl_list_cfg_info rx_vl_info;    /* ����vl��Ϣ */
    port_list_cfg_info tx_port_list;/* �˿��б� */
    port_list_cfg_info rx_port_list;/* �˿��б� */
}tar_cfg_info;

/* ȫ���豸�б����� */
typedef struct
{
    unsigned int laod_ip;           /* ���ض�ip��ַ */
    unsigned int targets_num;		/* Ŀ����豸���� */
    tar_cfg_info * targets_list;    /* Ŀ����豸�б� */
}afdx_cfg_info;
#pragma pack()

#ifdef  __cplusplus
}
#endif

#endif //ARINC615_STRUCT_DEFINE_H

