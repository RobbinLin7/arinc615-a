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
 * 版本记录：
 *		before:
 *		20211214: 添加获取版本号的接口; 修改find操作，支持一对多的场景
 *					修改了所有操作的接口参数，添加了目标IP地址的输入参数
 */


enum bool_t {
    ARINC615_TRUE = 0,
    ARINC615_FALSE = 1
};

//#define AFDX_NETWORK		0

#define ARINC615_PART_VER			(0x20211214)		/*软件版本号*/

extern char getFolderPath[512];

extern char sendFolderPath[512];

extern char fileFolderPath[512];

#define ARINC615_LOAD_RATIO_LENGTH              3                   //保存传输比率的缓冲区大小

#define ERROR_MESSAGE_BUFFER                    512                //保存错误信息的缓冲区大小

#define FILENAME_BUFFER_SIZE                    128                 //保存文件名的缓冲区大小

#define SEND_REQUEST_BUFFER_SIZE				32  				//发送Find请求缓冲区大小
#define RECV_DATA_BUFFER_SIZE					1024				//接收数据缓冲区大小

#define FILE_NAME_BUFFER_SIZE					256					//存储文件名的缓冲区大小
#define FILE_PATH_BUFFER_SIZE					512					//存储文件路径的缓冲区大小

#define WRITE_READ_REQUEST_BUFFER_SIZE			128					//存储读、写请求的缓冲区大小
#define ACK_BUFFER_SIZE							8					//存储ACK的缓冲区大小

#define MAX_ACK_BUFFER_SIZE						16					//存储ACK的缓冲区大小

#define STATUS_DES_BUFFER_SIZE                  256                 //状态描述缓冲区大小

#define MAX_BUFFER_16_SIZE							16					//16字节大小缓冲区

#define MAX_BUFFER_64_SIZE							64					//64字节大小缓冲区

#define MAX_FIND_DEVICE_NUMBER						32					//find最大支持的设备数量

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


#define ANALYSIS_STATUS_FILE_PERIOD		200				//解析状态文件的大致周期(ms)

#define WAIT_RECV_STATUS_FILE_TIME		7000			//等待接收状态文件的时间(ms)

#define DELAY_TIME_TMP                  50

#define RE_RECV_QEQUEST_DELAY			150				//若没收到请求后的延时时间


//2021-10 add New Definitions and Declarations

/*API返回码宏定义及说明*/

#define A615_SUCC						0               /*615A操作成功*/

#define A615_ERROR						-999            /*615A操作失败*/

#define A615_ABORT_CODE                 0x55            /*615A操作中止*/


#define PROTOCOL_FILE_ERROR				0xAA55AA		//协议文件解析错误返回的文件长度

/*公用返回码宏定义及说明*/
//用户操作错误：输出指针为空
#define A615_CMN_ERROR01				-1
//协议层错误：目标端TFTP传输响应超时
#define A615_CMN_ERROR02				-2
//协议层错误：文件路径不存在
#define A615_CMN_ERROR03				-3

/*初始化相关返回码宏定义及说明*/
//用户操作错误：传入网络类型参数无效
#define A615_INIT_ERROR01				-101
//用户操作错误：传入网络配置结构体参数无效
#define A615_INIT_ERROR02				-102
//协议层错误：申请资源时，空间不足，资源申请失败
#define A615_INIT_ERROR03				-103
//协议层错误：设备打开次数已达上限，不能继续打开设备
#define A615_INIT_ERROR04				-104
//协议层错误：待释放的设备操作句柄未被打开
#define A615_INIT_ERROR05				-105
//协议层错误：设备正在使用，不允许销毁
#define A615_INIT_ERROR06				-106
//底层网络驱动错误：网络初始化配置失败
#define A615_INIT_ERROR07				-107

#define A615_THREAD_ERROR08             -108            /*底层线程启动错误：创建新线程失败*/

/*操作相关返回码宏定义及说明*/

#define A615_OPT_ERROR01				-201            /*协议层错误：目标设备回复的find应答*/

#define A615_OPT_ERROR02				-202            /*协议层错误：find 操作失败*/

#define A615_OPT_ERROR03				-203            /*用户操作错误：传入的client id 参数无效*/

#define A615_OPT_ERROR04				-204            /*协议层错误：information 初始化阶段操作失败*/

#define A615_OPT_ERROR05				-205            /*协议层错误：information 数据传输阶段操作失败*/

#define A615_OPT_ERROR06				-206            /*协议层错误：load文件校验出错*/

#define A615_OPT_ERROR07				-207            /*协议层错误：LUH文件校验出错*/

#define A615_OPT_ERROR08				-208            /*协议层错误：upload 初始化阶段操作失败*/

#define A615_OPT_ERROR09				-209            /*协议层错误：upload 列表传输阶段操作失败*/

#define A615_OPT_ERROR09_1              -2091

#define A615_OPT_ERROR09_2              -2092

#define A615_OPT_ERROR10				-210            /*协议层错误：upload 数据传输阶段操作失败*/

#define A615_OPT_ERROR11				-211            /*协议层错误：media download 初始化阶段操作失败*/

#define A615_OPT_ERROR12				-212            /*协议层错误：media download 列表传输阶段操作失败*/

#define A615_OPT_ERROR13				-213            /*协议层错误：media download 数据传输阶段操作失败*/

#define A615_OPT_ERROR14				-214            /*用户操作错误：operator download操作传入的stage参数无效*/

#define A615_OPT_ERROR15				-215            /*用户操作错误：operator download操作传入的加载文件列表参数无效*/

#define A615_OPT_ERROR16				-216            /*用户操作错误：operator download操作传入的加载文件列表参数无效*/

#define A615_OPT_ERROR17				-217            /*协议层错误：operator download 初始化阶段操作失败*/

#define A615_OPT_ERROR18				-218            /*协议层错误：operator download 列表下载阶段操作失败*/

#define A615_OPT_ERROR19				-219            /*协议层错误：operator download 列表上传阶段操作失败*/

#define A615_OPT_ERROR20				-220            /*协议层错误：operator download 数据传输阶段操作失败*/

#define A615_OPT_ERROR21				-221            /*协议层错误：状态文件解析错误，运行阶段终止*/

#define A615_OPT_ERROR22                -222            /*用户操作错误：传入的加载文件列表中的文件数量无效*/

/*协议限定宏定义及说明*/
//加载端最多可支持的目标端连接数量
#define TAR_DEV_MAX_NUM					16
//加载端最多可支持的目标端可加载的load文件的数量
#define LOAD_FILE_MAX_NUM				50
//目标标识最多支持字符数量
#define ID_MAX_LEN						16
//类型名称最多支持字符数量
#define TYPE_NM_MAX_LEN					9
//目标位置最多支持字符数量
#define POS_MAX_LEN						9
//文字名称最多支持字符数量
#define LITERAL_NM_MAX_LEN				21
//制造商码最多支持字符数量
#define MNF_CODE_MAX_LEN				4
//终结符占字符数量
#define TMN_CODE_MAX_LEN				1
//点分十进制表示的IP地址，最多占16字节，包含终结符
#define IP_ADDR_MAX_LEN					16
//文件名称最大字符数量
#define FILE_NAME_MAX_LEN				512
//客户端标识长度，客户端标识由“identifier_position”构成
#define CLIENT_ID_MAX_LEN				32
//上传或者下载的比例，用ASCII码字符表示
#define A615_LOAD_RATIO_LEN				4
//配置TFTP协议超时时间，单位为：S
#define A615_TFTP_TIMEOUT				3
//配置TFTP重传次数
#define A615_TFTP_RETRY_CNTS			5


/**
* 该枚举类型定义615A协议底层通信网络类型
* 应用层在对协议进行初始化时，需要根据网络类型
* 传输对应通信网络的配置参数，实现对底层通信网络的初始化。
*/
typedef enum
{
	SOCKET_NET = 1,				/* 协议层通信网络基于socket通信 */
	AFDX_NET = 2,        		/* 协议层通信网络基于afdx通信 */
	UNKNOWN_NET
}a615_cmm_net_type_e;

/*
* 该枚举类型用于定义状态文件中的状态码
* 加载端根据目标端传输的状态码，判断目标端当前操作的进度。
*/
typedef enum
{
	ACCEPT_OPT = 0x0001,        			/*目标端接受操作请求 */
	OPT_IN_PRO = 0x0002,        			/*操作正在进行中 */
	OPT_IN_CMPT = 0x0003,        			/*操作已经完成 */
	OPT_IN_PRO_DETAILS = 0x0004,			/*操作正在进行，细节见状态描述 */
	OPT_ABT_BY_TAR = 0x1003,        		/*目标端放弃操作 */
	OPT_ABTED_DUETO_DATALOADER = 0x1004,	/*目标端已经放弃操作，原因是收到来自数据加载协议的abort错误信息 */
	OPT_ABTED_DUETO_INIT = 0x1005,			/* 目标端已经放弃操作，原因是收到操作者初始化的abort错误信息 */
	OPT_ABTED_DUETO_FAILURE = 0x1007			/* 上传操作时表示头文件加载失败，下载操作时表示当前文件下载失败 */
}a615_status_t_codes_e;


#define TRANSFER_DELAY              5000                //recvfrom函数超时时间，单位ms

//#define HOST_ADDRESS            "169.254.5.140"      //本机IP
//#define TARGET_ADDRESS          "169.254.5.122"      //对端IP
//#define HOST_ADDRESS            "192.168.5.5"      //本机IP
extern char HOST_ADDRESS[64];					//保存主机地址
#define TARGET_ADDRESS          "192.168.5.6"      //对端IP

#define HOST_FIND_BIND_PORT      1002                //find操作监听端口
#define HOST_FIND_SEND_PORT      1001                //find操作发送端口

//#define HOST_OPERATION_BIND_PORT 69                  //流程操作监听端口		云维智联	
#define HOST_OPERATION_BIND_PORT 8888                  //流程操作监听端口		云维智联	
#define HOST_OPERATION_SEND_PORT 69                //流程操作发送端口

//#define HOST_STATUS_BIND_PORT    55555               //状态文件监听端口		云维智联	
#define HOST_STATUS_BIND_PORT    8888               //状态文件监听端口			503
//#define HOST_STATUS_SEND_PORT    1022                //状态文件发送端口		云维智联	
#define HOST_STATUS_SEND_PORT    55556                //状态文件发送端口			503


#undef DEBUG_503_TEST

#ifdef  __cplusplus
}
#endif

#endif

