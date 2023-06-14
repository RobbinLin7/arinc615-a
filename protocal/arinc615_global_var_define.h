#ifndef ARINC615_GLOBAL_VAR_DEFINE_H
#define ARINC615_GLOBAL_VAR_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;

/*
 * �汾��¼��
 *		before:
 *		20211214: ��ӻ�ȡ�汾�ŵĽӿ�; �޸�find������֧��һ�Զ�ĳ���
 *					�޸������в����Ľӿڲ����������Ŀ��IP��ַ���������
 */


enum bool_t {
    ARINC615_TRUE = 0,
    ARINC615_FALSE = 1
};

//#define AFDX_NETWORK		0

#define ARINC615_PART_VER			(0x20211214)		/*����汾��*/

extern char getFolderPath[512];

extern char sendFolderPath[512];

extern char fileFolderPath[512];

#define ARINC615_LOAD_RATIO_LENGTH              3                   //���洫����ʵĻ�������С

#define ERROR_MESSAGE_BUFFER                    512                //���������Ϣ�Ļ�������С

#define FILENAME_BUFFER_SIZE                    128                 //�����ļ����Ļ�������С

#define SEND_REQUEST_BUFFER_SIZE				32  				//����Find���󻺳�����С
#define RECV_DATA_BUFFER_SIZE					1024				//�������ݻ�������С

#define FILE_NAME_BUFFER_SIZE					256					//�洢�ļ����Ļ�������С
#define FILE_PATH_BUFFER_SIZE					512					//�洢�ļ�·���Ļ�������С

#define WRITE_READ_REQUEST_BUFFER_SIZE			128					//�洢����д����Ļ�������С
#define ACK_BUFFER_SIZE							8					//�洢ACK�Ļ�������С

#define MAX_ACK_BUFFER_SIZE						16					//�洢ACK�Ļ�������С

#define STATUS_DES_BUFFER_SIZE                  256                 //״̬������������С

#define MAX_BUFFER_16_SIZE							16					//16�ֽڴ�С������

#define MAX_BUFFER_64_SIZE							64					//64�ֽڴ�С������

#define MAX_FIND_DEVICE_NUMBER						32					//find���֧�ֵ��豸����

#define EXEC_SUCCEED_CODE						0
#define EXEC_SUCCEED_MSG						"Succeed!"

#define NETWORK_INIT_ERR_CODE_001				1
#define NETWORK_INIT_ERR_MSG_001				"A615NetAddrInit ERROR."

#define OP_DOWNLOAD_INIT_STAGE					1
#define OP_DOWNLOAD_DATA_STAGE					2

#define FILE_OPERATION_ERR_CODE_020		20
#define FILE_OPERATION_ERR_MSG_020		"file open/close/remove error!"



#define FIND_ERR_CODE_100               100
#define FIND_ERR_MSG_100                "Find:: find target device failed!"

#define INFORMATION_ERR_CODE_200		200
#define INFORMATION_ERR_MSG_200 		"Information:: Initialization failed!"

#define INFORMATION_ERR_CODE_202		202
#define INFORMATION_ERR_MSG_202 		"Information:: Not recv LCL file Request!"

#define INFORMATION_ERR_CODE_203		203
#define INFORMATION_ERR_MSG_203 		"Information:: Trans LCL failed!"

#define UPLOAD_ERR_CODE_300             300
#define UPLOAD_ERR_MSG_300              "Upload:: Initialization failed!"

#define UPLOAD_ERR_CODE_302             302
#define UPLOAD_ERR_MSG_302              "upload Init phase failed."

#define UPLOAD_ERR_CODE_303             303
#define UPLOAD_ERR_MSG_303              "upload List phase failed."

#define UPLOAD_ERR_CODE_304             304
#define UPLOAD_ERR_MSG_304              "upload recv LUH request failed."

#define UPLOAD_ERR_CODE_305             305
#define UPLOAD_ERR_MSG_305              "upload send LUH file failed."

#define UPLOAD_ERR_CODE_306             306
#define UPLOAD_ERR_MSG_306              "upload recv data file request failed."

#define UPLOAD_ERR_CODE_307             307
#define UPLOAD_ERR_MSG_307              "upload send data file failed."

#define UPLOAD_ERR_CODE_308             308
#define UPLOAD_ERR_MSG_308              "upload recv ERROR request."

#define M_DOWNLOAD_ERR_CODE_400         400
#define M_DOWNLOAD_ERR_MSG_400          "media download:: Initialization failed!"

#define M_DOWNLOAD_ERR_CODE_402         402
#define M_DOWNLOAD_ERR_MSG_402          "media download:: recv LND file failed!"

#define M_DOWNLOAD_ERR_CODE_403         403
#define M_DOWNLOAD_ERR_MSG_403          "media download:: recv data file request failed."

#define M_DOWNLOAD_ERR_CODE_404         404
#define M_DOWNLOAD_ERR_MSG_404          "media download:: recv data file failed."

#define M_DOWNLOAD_ERR_CODE_405         405
#define M_DOWNLOAD_ERR_MSG_405          "media download:: recv ERROR request."

#define O_DOWNLOAD_ERR_CODE_500         500
#define O_DOWNLOAD_ERR_MSG_500          "operator download:: Initialization failed!"

#define O_DOWNLOAD_ERR_CODE_502         502
#define O_DOWNLOAD_ERR_MSG_502          "operator download:: recv LNO file failed!"

#define O_DOWNLOAD_ERR_CODE_503			503
#define	O_DOWNLOAD_ERR_MSG_503			"operator download:: not recv LNL file request!"

#define O_DOWNLOAD_ERR_CODE_504			504
#define	O_DOWNLOAD_ERR_MSG_504			"operator download:: not recv LNA file!"

#define O_DOWNLOAD_ERR_CODE_505         505
#define O_DOWNLOAD_ERR_MSG_505          "operator download:: list trans stage failed!"

#define O_DOWNLOAD_ERR_CODE_506         506
#define O_DOWNLOAD_ERR_MSG_506          "operator download:: recv data file request failed."

#define O_DOWNLOAD_ERR_CODE_507         507
#define O_DOWNLOAD_ERR_MSG_507          "operator download:: recv data file failed."

#define O_DOWNLOAD_ERR_CODE_508         508
#define O_DOWNLOAD_ERR_MSG_508          "operator download:: recv ERROR request."



#define STATUS_THREAD_FAILED_CODE		600
#define STATUS_THREAD_FAILED_MSG		"recv status file thread not running!"

#define STATUS_FILE_ANA_ERR_CODE		601
#define STATUS_FILE_ANA_ERR_MSG			"status file analysis Error!"

#define IP_ADDR_INVALID_ERR_CODE		777
#define IP_ADDR_INVALID_ERR_MSG			"dest IP is invalid!"


#define ANALYSIS_STATUS_FILE_PERIOD		200				//����״̬�ļ��Ĵ�������(ms)

#define WAIT_RECV_STATUS_FILE_TIME		7000			//�ȴ�����״̬�ļ���ʱ��(ms)

#define DELAY_TIME_TMP                  50

#define RE_RECV_QEQUEST_DELAY			150				//��û�յ���������ʱʱ��


//2021-10 add New Definitions and Declarations

/*API������궨�弰˵��*/

#define A615_SUCC						0               /*615A�����ɹ�*/

#define A615_ERROR						-999            /*615A����ʧ��*/

#define A615_ABORT_CODE                 0x55            /*615A������ֹ*/


#define PROTOCOL_FILE_ERROR				0xAA55AA		//Э���ļ��������󷵻ص��ļ�����

/*���÷�����궨�弰˵��*/
//�û������������ָ��Ϊ��
#define A615_CMN_ERROR01				-1
//Э������Ŀ���TFTP������Ӧ��ʱ
#define A615_CMN_ERROR02				-2
//Э�������ļ�·��������
#define A615_CMN_ERROR03				-3

/*��ʼ����ط�����궨�弰˵��*/
//�û��������󣺴����������Ͳ�����Ч
#define A615_INIT_ERROR01				-101
//�û��������󣺴����������ýṹ�������Ч
#define A615_INIT_ERROR02				-102
//Э������������Դʱ���ռ䲻�㣬��Դ����ʧ��
#define A615_INIT_ERROR03				-103
//Э�������豸�򿪴����Ѵ����ޣ����ܼ������豸
#define A615_INIT_ERROR04				-104
//Э�����󣺴��ͷŵ��豸�������δ����
#define A615_INIT_ERROR05				-105
//Э�������豸����ʹ�ã�����������
#define A615_INIT_ERROR06				-106
//�ײ������������������ʼ������ʧ��
#define A615_INIT_ERROR07				-107

#define A615_THREAD_ERROR08             -108            /*�ײ��߳��������󣺴������߳�ʧ��*/

/*������ط�����궨�弰˵��*/

#define A615_OPT_ERROR01				-201            /*Э������Ŀ���豸�ظ���findӦ��*/

#define A615_OPT_ERROR02				-202            /*Э������find ����ʧ��*/

#define A615_OPT_ERROR03				-203            /*�û��������󣺴����client id ������Ч*/

#define A615_OPT_ERROR04				-204            /*Э������information ��ʼ���׶β���ʧ��*/

#define A615_OPT_ERROR05				-205            /*Э������information ���ݴ���׶β���ʧ��*/

#define A615_OPT_ERROR06				-206            /*Э������load�ļ�У�����*/

#define A615_OPT_ERROR07				-207            /*Э������LUH�ļ�У�����*/

#define A615_OPT_ERROR08				-208            /*Э������upload ��ʼ���׶β���ʧ��*/

#define A615_OPT_ERROR09				-209            /*Э������upload �б���׶β���ʧ��*/

#define A615_OPT_ERROR09_1              -2091

#define A615_OPT_ERROR09_2              -2092

#define A615_OPT_ERROR10				-210            /*Э������upload ���ݴ���׶β���ʧ��*/

#define A615_OPT_ERROR11				-211            /*Э������media download ��ʼ���׶β���ʧ��*/

#define A615_OPT_ERROR12				-212            /*Э������media download �б���׶β���ʧ��*/

#define A615_OPT_ERROR13				-213            /*Э������media download ���ݴ���׶β���ʧ��*/

#define A615_OPT_ERROR14				-214            /*�û���������operator download���������stage������Ч*/

#define A615_OPT_ERROR15				-215            /*�û���������operator download��������ļ����ļ��б������Ч*/

#define A615_OPT_ERROR16				-216            /*�û���������operator download��������ļ����ļ��б������Ч*/

#define A615_OPT_ERROR17				-217            /*Э������operator download ��ʼ���׶β���ʧ��*/

#define A615_OPT_ERROR18				-218            /*Э������operator download �б����ؽ׶β���ʧ��*/

#define A615_OPT_ERROR19				-219            /*Э������operator download �б��ϴ��׶β���ʧ��*/

#define A615_OPT_ERROR20				-220            /*Э������operator download ���ݴ���׶β���ʧ��*/

#define A615_OPT_ERROR21				-221            /*Э������״̬�ļ������������н׶���ֹ*/

#define A615_OPT_ERROR22                -222            /*�û��������󣺴���ļ����ļ��б��е��ļ�������Ч*/

/*Э���޶��궨�弰˵��*/
//���ض�����֧�ֵ�Ŀ�����������
#define TAR_DEV_MAX_NUM					16
//���ض�����֧�ֵ�Ŀ��˿ɼ��ص�load�ļ�������
#define LOAD_FILE_MAX_NUM				50
//Ŀ���ʶ���֧���ַ�����
#define ID_MAX_LEN						16
//�����������֧���ַ�����
#define TYPE_NM_MAX_LEN					9
//Ŀ��λ�����֧���ַ�����
#define POS_MAX_LEN						9
//�����������֧���ַ�����
#define LITERAL_NM_MAX_LEN				21
//�����������֧���ַ�����
#define MNF_CODE_MAX_LEN				4
//�ս��ռ�ַ�����
#define TMN_CODE_MAX_LEN				1
//���ʮ���Ʊ�ʾ��IP��ַ�����ռ16�ֽڣ������ս��
#define IP_ADDR_MAX_LEN					16
//�ļ���������ַ�����
#define FILE_NAME_MAX_LEN				512
//�ͻ��˱�ʶ���ȣ��ͻ��˱�ʶ�ɡ�identifier_position������
#define CLIENT_ID_MAX_LEN				32
//�ϴ��������صı�������ASCII���ַ���ʾ
#define A615_LOAD_RATIO_LEN				4
//����TFTPЭ�鳬ʱʱ�䣬��λΪ��S
#define A615_TFTP_TIMEOUT				3
//����TFTP�ش�����
#define A615_TFTP_RETRY_CNTS			5


/**
* ��ö�����Ͷ���615AЭ��ײ�ͨ����������
* Ӧ�ò��ڶ�Э����г�ʼ��ʱ����Ҫ������������
* �����Ӧͨ����������ò�����ʵ�ֶԵײ�ͨ������ĳ�ʼ����
*/
typedef enum
{
	SOCKET_NET = 1,				/* Э���ͨ���������socketͨ�� */
	AFDX_NET = 2,        		/* Э���ͨ���������afdxͨ�� */
	UNKNOWN_NET
}a615_cmm_net_type_e;

/*
* ��ö���������ڶ���״̬�ļ��е�״̬��
* ���ض˸���Ŀ��˴����״̬�룬�ж�Ŀ��˵�ǰ�����Ľ��ȡ�
*/
typedef enum
{
	ACCEPT_OPT = 0x0001,        			/*Ŀ��˽��ܲ������� */
	OPT_IN_PRO = 0x0002,        			/*�������ڽ����� */
	OPT_IN_CMPT = 0x0003,        			/*�����Ѿ���� */
	OPT_IN_PRO_DETAILS = 0x0004,			/*�������ڽ��У�ϸ�ڼ�״̬���� */
	OPT_ABT_BY_TAR = 0x1003,        		/*Ŀ��˷������� */
	OPT_ABTED_DUETO_DATALOADER = 0x1004,	/*Ŀ����Ѿ�����������ԭ�����յ��������ݼ���Э���abort������Ϣ */
	OPT_ABTED_DUETO_INIT = 0x1005,			/* Ŀ����Ѿ�����������ԭ�����յ������߳�ʼ����abort������Ϣ */
	OPT_ABTED_DUETO_FAILURE = 0x1007			/* �ϴ�����ʱ��ʾͷ�ļ�����ʧ�ܣ����ز���ʱ��ʾ��ǰ�ļ�����ʧ�� */
}a615_status_t_codes_e;


#define TRANSFER_DELAY              5000                //recvfrom������ʱʱ�䣬��λms

//#define HOST_ADDRESS            "169.254.5.140"      //����IP
//#define TARGET_ADDRESS          "169.254.5.122"      //�Զ�IP
//#define HOST_ADDRESS            "192.168.5.5"      //����IP
extern char HOST_ADDRESS[64];					//����������ַ
#define TARGET_ADDRESS          "192.168.5.6"      //�Զ�IP

#define HOST_FIND_BIND_PORT      1002                //find���������˿�
#define HOST_FIND_SEND_PORT      1001                //find�������Ͷ˿�

//#define HOST_OPERATION_BIND_PORT 69                  //���̲��������˿�		��ά����	
#define HOST_OPERATION_BIND_PORT 8888                  //���̲��������˿�		��ά����	
#define HOST_OPERATION_SEND_PORT 69                //���̲������Ͷ˿�

//#define HOST_STATUS_BIND_PORT    55555               //״̬�ļ������˿�		��ά����	
#define HOST_STATUS_BIND_PORT    8888               //״̬�ļ������˿�			503
//#define HOST_STATUS_SEND_PORT    1022                //״̬�ļ����Ͷ˿�		��ά����	
#define HOST_STATUS_SEND_PORT    55556                //״̬�ļ����Ͷ˿�			503


#undef DEBUG_503_TEST

#ifdef  __cplusplus
}
#endif

#endif

