/*-----------------------------------------------------------------------------------------
**
** 版权（Copyright）    : COPYRIGHT CARERI, 2011
**
** 文件名（File Name）  : AfdxHead.h
**
** 描述（Description）  : 针对全逻辑终端的HOST 端宏定义
**
** 作者 (Author)        : Chen Sheng
**
** 更改历史（Modification History）:
**
** $Log:
**
**---------------------------------------------------------------------------------------
*/


#ifndef __AFDX_HEAD_H__
#define __AFDX_HEAD_H__

/*用于测试的一些临时宏定义*/
/*#define TRTE_TEST 1*/
#define NEW_LOGIC 1

#define ENABLESEM 1

#define TEST_BUF_SIZE 64
/*正式宏定义*/
#define AFDX_INIT_SUCCESSFUL	       0
#define	AFDX_INIT_FAIL		           1

#define AFDX_CONFIGURE_TABLE_NEW     1
#define AFDX_CONFIGURE_TABLE_OLD     2
#define AFDX_CONFIGURE_TABLE_LOOP    3

#define AFDX_SAMPLE_PORT             0    /*采样端口类型*/
#define AFDX_QUEUE_PORT              1    /*队列端口类型*/            
#define AFDX_SAP_PORT                2    /*SAP端口类型*/

/*数据发送返回值*/
#define AFDX_SUCCESSFUL              0	  /*执行成功*/
#define AFDX_INPUT_DATA_TOO_LARGE    1	  /*输入数据太长*/
#define AFDX_INPUT_DATA_TOO_SMALL    2    /*输入数据太短*/
#define AFDX_PORT_NOT_VALID          3    /*端口无效*/
#define AFDX_TX_BUF_OVERFLOW         4    /*发送缓冲区溢出*/
#define AFDX_SUBVL_FULL	             51   /*发送子虚拟链路缓冲区已满*/

/*数据接收返回值*/
#define AFDX_MSG_NOT_READ_OVER       5    /*缓冲区中还有未读的消息*/
#define AFDX_NO_DATA_RECEIVED        6    /*缓冲区中无消息*/
#define AFDX_RX_BUF_TOO_SMALL        7    /*缓冲区中一桢数据长度大于所给接收数据区长度*/
#define AFDX_RX_BUF_OVERFLOW_OCCURED 8    /*在此读过程前，数据区发生过数据溢出*/
#define AFDX_IP_INVALID              9    /*IP地址错误*/
#define AFDX_REV_MSG_IS_OLD          10   /*接收数据为旧数据*/
#define AFDX_REV_RAM_INVALID         11   /*接收缓冲区不存在*/
#define AFDX_SEM_ERR 12 /*semTake调用失败*/

/*性能定义*/
#define MAX_CFG_TABLE_SIZE      0x80000
#define AfdxSendVlSum		       128
#define AfdxSendSubVlSum		       4
#define AfdxSendBufferSize			     0x2000
#define AfdxRecvVlSum    256
#define MAX_AFDX_SEND_PORT_NUM       1024 /*支持的最大发送端口数*/
#define MAX_AFDX_RECV_PORT_NUM       1024 /*支持的最大接收端口数*/
#define MAX_AFDX_PORT_ID             65535/*端口号最大为65535*/
#define ICMP_TASK_PRIORITY 100

#define AFDX_CFG_POS_1               0x01
#define AFDX_CFG_POS_2               0x04
#define AFDX_CFG_POS_3               0x07
#define AFDX_CFG_POS_4               0x02

#define MSG_HF_MAC                   0x0
#define MSG_HF_IP                    0x1
#define MSG_HF_UDP                   0x2
#define MSG_HF_EN_IP                 0x4
#define MSG_HF_EN_UDP_PORT           0x8
#define MSG_HF_EN_PROT               0x10
#define UDP_PROTO                    0x11
#define ICMP_PROTO    0x01

#define RAM_OFFSET_1      0x00000000

#define DMA_SEND_DATA  0x10000000
#define DMA_RECV_DATA  0x10004000
#define DMA_SEND_HEADER 0x10008000
#define DMA_SEND_RETURN 0x10009000
#define DMA_RECV_RETURN 0x1000A000


#define PUTSHORT(addr, s) \
({\
    *(unsigned char*)(addr) = ((s) >> 8) & 0xFF;\
    *(unsigned char*)(addr + 1) = (s) & 0xFF;\
})

/************add by lzx 2014-04-18*******************/
unsigned int AFDX_SENDUDP_PORT;
unsigned int AFDX_RECVUDP_PORT;
/*************************************************/

/*发送结构体,16字节*/
#define SEND_PARAM_HEAD_LEN          16   /*16字节的发送参数头部*/
#define MSG_MAX_LEN                  16384   /*消息主体*/
typedef struct
{
	unsigned int SubVl:16;                  /*子虚拟链路索引号*/
	unsigned int Len:16;                    /*包括头部16字节*/
	unsigned int PortIndex:16;
	unsigned int Flag:16;
	unsigned int DstIp:32;
	unsigned int DstPort:16;
	unsigned int Protocol:8;
	unsigned int Res:8;
	unsigned int UDPDataAddr:32;
}STR_UDP_SEND;

#if NEW_LOGIC

/*接收结构体*/
#define RECV_PARAM_HEAD_LEN          16   /*16字节的接收参数头部*/
typedef struct
{
	unsigned int Len:16;
	unsigned int Type:8;
	unsigned int Flag:8;
	unsigned int TimeStamp_High:32;
	unsigned int TimeStamp_Low:16;
	unsigned int SrcPort:16;
	unsigned int SrcIp:32;
}STR_UDP_REV;

#else

#define RECV_PARAM_HEAD_LEN          20   /*20字节的接收参数头部*/
typedef struct
{
	unsigned int Len:16;
	unsigned int Type:8;
	unsigned int Flag:8;
	unsigned int TimeStamp_High:32;
	unsigned int TimeStamp_Low:32;
    unsigned int res:16;
	unsigned int SrcPort:16;
	unsigned int SrcIp:32;
}STR_UDP_REV;

#endif

/*驱动自己定义的发送接收端口结构体*/
typedef struct
{
    unsigned int PortIndex;
    unsigned int PortId;
    unsigned int VlId;
    unsigned int VlIndex;
    unsigned int	SubVlIndex;
    unsigned int MsgNum;
    unsigned int	MaxLen;
    unsigned int	PortType;
}STR_SEND_PORT;

typedef struct
{
    unsigned int	VlNum;
    unsigned int	PortId;
    unsigned int	PortType;
    unsigned int	Size;
    unsigned int	BaseAddr;
    unsigned int	FrameLen;
    unsigned int	Head;
    unsigned int	Tail;
    unsigned int	MaxLen;
}STR_REV_PORT;

#if 0
typedef struct
{
	unsigned int rtc_high_word;
	unsigned int rtc_low_word;
} RTC_WORD_TYPE;
#endif


/********************************************************Table Config********************************************************************/
/*index0*/
typedef struct
{
    unsigned int 	TableIndex:16;
    unsigned int	Res0:16;
    unsigned int	MacSrcAddrA1:32;
    unsigned int	Res1:16;
    unsigned int 	MacSrcAddrA2:16;
    unsigned int	MacSrcAddrB1:32;
    unsigned int	Res2:16;
    unsigned int  MacSrcAddrB2:16;
    unsigned int  MacDstAddr:32;
    unsigned int	IpType:8;
    unsigned int	Setting:8;
    unsigned int  IpMultDstAddr:16;
    unsigned int	IpAddrCst:32;
}STR_CFG_ES;

/*index1*/
typedef struct
{
    unsigned int 	TableIndex:16;
    unsigned int	PortIndex:16;
    unsigned int	CfgDstIpAddr:32;
    unsigned int	CfgDstPortNum:16;
    unsigned int 	VlId:16;
    unsigned int	Setting:8;
    unsigned int  vl_index:8;
    unsigned int	Res1:16;
    unsigned int	BufferStartAddr:32;
    unsigned int	BufferEndAddr:32;
    unsigned int	BufferSize:32;
    unsigned int Res2:18;
    unsigned int	msg_max_len:14;
}STR_CFG_RxPortBuffer;


/*index2*/
typedef struct
{
	unsigned short 	TableIndex;
	unsigned short	VlIndex;
	unsigned int		DstMac1;
	unsigned int		DstMac2;
	unsigned int 	   Setting;
	unsigned int		BufferStartAddr;
	unsigned int		BufferEndAddr;
	unsigned int		BufferSize;
	unsigned int		Res3;
}STR_CFG_RxVlBuffer;

/*index3*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	VlIndex:16;
	unsigned int	SrcMacAddrA1:32;
	unsigned int	SrcMacAddrA2:16;
	unsigned int 	SrcMacAddrB1:16;
	unsigned int	SrcMacAddrB2:32;
	unsigned int	Setting1:32;
	unsigned int	SkewMax:16;
	unsigned int	Setting2:16;
	unsigned int	Res1:32;
	unsigned int	Res2:32;
}STR_CFG_RxVl;

/*index4*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	Res1:16;
	unsigned int	Setting:32;
	unsigned int	Res2:32;
	unsigned int	Res3:32;
	unsigned int	Res4:32;
	unsigned int	Res5:32;
	unsigned int	Res6:32;
	unsigned int	Res7:32;
}STR_CFG_RxES;

/*index5*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	PortIndex:16;
	unsigned int	Setting1:32;
	unsigned int	CfgDstPortNum:16;
	unsigned int 	CfgSrcPortNum:16;
	unsigned int	SubVl:9;
	unsigned int	Setting2:15;
	unsigned int  DstIp8:8;
	unsigned int	Res1:32;
	unsigned int	Res2:32;
	unsigned int	Res3:32;
	unsigned int	Res4:32;
}STR_CFG_TxComPort;

/*index6*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	PortIndex:16;
	unsigned int	BufferStartAddr:32;
	unsigned int	BufferEndAddr:32;
	unsigned int	BufferSize:32;
	unsigned int	CfgSpareSize:32;
	unsigned int  Res1:32;
	unsigned int  Res2:32;
	unsigned int	Res3:32;
}STR_CFG_TxPortBuffer;

/*index7*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	VlIndex:16;
	unsigned int	VlIdDstMacAddr:16;
	unsigned int	Setting1:16;
	unsigned int	TransTablePeriod:16;
	unsigned int	TransTableOffset:16;
	unsigned int	Res1:32;
	unsigned int	Res2:32;
	unsigned int	Res3:32;
	unsigned int	Res4:32;
	unsigned int	Res5:32;
}STR_CFG_Vl;

/*index8*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	Len:16;
	unsigned int	Setting:32;
	unsigned int	rtc_udphead1:32;
	unsigned int	rtc_udphead2:32;
	unsigned int      rtc_vl_index:32;
	unsigned int	Res2:32;
	unsigned int	Res3:32;
	unsigned int	Res4:32;
}STR_CFG_TxEs;



/*index9*/
typedef struct
{
	unsigned int 	TableIndex:16;
	unsigned int	Len:16;
	unsigned int	Setting:32;
	unsigned int	rtc_iphead1:32;
	unsigned int	rtc_iphead2:32;
	unsigned int	rtc_iphead3:32;
	unsigned int	rtc_iphead4:32;
	unsigned int	rtc_iphead5:32;
	unsigned int	Res1:32;
}STR_CFG_net;



typedef struct{
    unsigned short  replyvlid;
    unsigned short  replysubvl;
    unsigned short  requestvlid;
    unsigned short  requestsubvl;
    unsigned int replyvl_index;
    unsigned int requestvl_index;
}STR_CFG_ICMP;

typedef struct{
    unsigned char type;
    unsigned char code;
    unsigned char checksum[2];
    unsigned char indentifier[2];
    unsigned char sn[2];
}STR_ICMP_HEADER;
/****************************************************************************************************************************/




/******************************************************New Configure Table Structure****************************************************************************/
typedef struct
{
  unsigned char  empty[68];
	unsigned short port_speed;
	unsigned char  es_domain;
	unsigned char  es_side;
	unsigned char  es_location;
	unsigned char  snmp_mode;
	unsigned char  icmp_mode;
	unsigned char  arinc615a_mode;
	unsigned int   partion_num;
	unsigned int   partion_start_offset;
	unsigned int   tx_udp_num;
	unsigned int   tx_udp_start_offset;
	unsigned int   rx_udp_num;
	unsigned int   rx_udp_start_offset;
	unsigned int   tx_vl_num;
	unsigned int   tx_vl_start_offset;
	unsigned int   rx_vl_num;
	unsigned int   rx_vl_start_offset;
	unsigned int   icmp_cfg_num;
	unsigned int   icmp_cfg_start_offset;
	unsigned char  spare[8];
}CFG_ES_HEADER_TYPE_NEW;

typedef struct
{
  unsigned char  name[32];
  unsigned short partion_id;
  unsigned short res; 
}CFG_ES_PARTION_TYPE_NEW;

typedef struct
{
	unsigned short  comid:16;
	unsigned short  spare1:16;
	unsigned int    srcip:32;
	unsigned int    dstip:32;
	unsigned short  srcudp:16;
	unsigned short  dstudp:16;
	unsigned short  vlid:16;
	unsigned short  subvl:16;
	unsigned short  porttype:16;
	unsigned short  servicetype:16;
	unsigned short  msg_len:16;
	unsigned short  msg_buff_num:16;
	unsigned short  sampling_rate:16;
	unsigned short  fragment:16;
	unsigned short  snmpid:16;
	unsigned short  loaderid:16;
	unsigned int    res1:32;
	unsigned int    res2:32;
}CFG_ES_TX_UDP_TYPE_NEW;

typedef struct
{
	unsigned short  comid;
	unsigned short  vlid;
	unsigned int    srcip;
	unsigned int    dstip;
	unsigned short  srcudp;
	unsigned short  dstudp;
	unsigned short  port_type;
	unsigned short  servicetype;
	unsigned short  msg_len;
	unsigned short  msg_buff_num;
	unsigned short  sampling_rate;
	unsigned short  is_fen;
	unsigned short  snmpid;
	unsigned short  loaderid;
	unsigned char   spare[8];
}CFG_ES_RX_UDP_TYPE_NEW;

typedef struct
{
    unsigned short vlNum;
    unsigned short skewMax;
    unsigned short integrityAble;
    unsigned short redundantAble;
    unsigned int   swPort;     /*终端使用的交换机端口，仅适用于交换机终端*/
    unsigned short vlType;		/* 0普通，1RTC，2SNMP，4615A，8ICMP */
    unsigned short blank0;
    unsigned int  blank1;
}CFG_ES_RX_VL_TYPE_NEW;

typedef struct
{
    unsigned short vlNum;
    unsigned short bag;
    unsigned short subvlTotal;
    unsigned short maxLen;
    unsigned short minLen;
    unsigned short netSelect;
    unsigned short rmEnable;
    unsigned short vlType;		/* 0普通，1RTC，2SNMP，4615A，8ICMP */
    unsigned int   rtcPeriod;
    unsigned int   jitter;
    unsigned int   sw_port;
    unsigned int   period;
    unsigned int   offset;
}CFG_ES_TX_VL_TYPE_NEW;

typedef struct
{
	unsigned short  replyvlid;
	unsigned short  replysubvl;
	unsigned int    dstip;
	unsigned short  requestvlid;
	unsigned short  requestsubvl;
	unsigned short  icmpperiod;
	unsigned char   spare[6];
}CFG_ES_ICMP_TYPE_NEW;

typedef struct
{
    unsigned int send_cnt[MAX_AFDX_SEND_PORT_NUM];
    unsigned int send_err[MAX_AFDX_SEND_PORT_NUM];
    unsigned int recv_cnt[MAX_AFDX_RECV_PORT_NUM];
    unsigned int recv_err[MAX_AFDX_RECV_PORT_NUM];
    unsigned int recv_icmp_cnt;
    unsigned int send_icmp_cnt;
    unsigned int recv_icmp_err;
}ES_DRV_MIB;

typedef struct
{
	unsigned int  esId;                     /*硬件板卡号*/
	unsigned int  esVersion;                /*软件版本号*/
	unsigned int 	esStatus;                 /*终端工作状态*/
	unsigned int 	esPosition;               /*终端位置*/
	unsigned int 	esCfgID;                  /*配置表编号*/
	unsigned int  esTxCnt;                  /*发送总计数*/
	unsigned int 	esTxIpCnt;                /*发送IP   计数*/
	unsigned int 	esTxIcmpCnt;              /*发送ICMP 计数*/
	unsigned int 	esTxUdpCnt;               /*发送UDP  计数*/
	unsigned int 	esRxCnt;                  /*接收总计数*/
	unsigned int 	esRxIpCnt;                /*接收IP   计数*/
	unsigned int 	esRxIcmpCnt;              /*接收ICMP 计数*/
	unsigned int 	esRxUdpCnt;               /*接收UDP  计数*/
	unsigned int  afterRedundant;	          /*余度后接收帧计数*/
	unsigned int 	aPortRxCnt;               /*A端口接收总计数*/
	unsigned int  aPortCrcErrCnt;           /*A端口CRC 错误计数*/
	unsigned int	aPortAlignmentErrCnt;     /*A端口字节非对齐错误计数*/	
	unsigned int	aPortLengthExceedCnt64;   /*A端口帧长度小于64Bytes的计数*/
	unsigned int	aPortLengthExceedCnt1518; /*A端口帧长度大于1518Bytes的计数*/
	unsigned int	aPortSNErrCnt;            /*A端口帧序列号检测错误计数*/
	unsigned int	aPortVlIdErrCnt;          /*A端口接收的VL在配置中不存在*/
	unsigned int 	bPortRxCnt;               /*B端口接收总计数*/
	unsigned int  bPortCrcErrCnt;           /*B端口CRC 错误计数*/
	unsigned int  bPortAlignmentErrCnt;     /*B端口字节非对齐错误计数*/	
	unsigned int  bPortLengthExceedCnt64;   /*B端口帧长度小于64Bytes的计数*/
	unsigned int	bPortLengthExceedCnt1518;	/*B端口帧长度大于1518Bytes的计数*/
	unsigned int	bPortSNErrCnt;            /*B端口帧序列号检测错误计数*/
	unsigned int  bPortVlIdErrCnt;				  /*B端口接收的VL在配置中不存在*/
	unsigned int 	esRxIpErrCnt;					    /*IP头检验错误计数*/
	unsigned int  esUdpComPortErr;				  /*UDP接收端口配置中未定义*/
	unsigned long long equipmentOnline;     /*网络设备在线状态64bit */
	unsigned int 	esUpTime;					        /*设备工作总时间*/
	unsigned int  esException;              /*异常状态*/
	unsigned int  ESInitBIT;                /**/
	unsigned int  ESIPortDetect;            /**/
	unsigned int  ESPeriodBIT;              /**/
	unsigned int  ESMaintainBIT;            /**/
	unsigned int  ESMPortDetect;            /**/
	unsigned int  aPortRvbPortCnt;          /*A收B计数器*/
	unsigned int  aPortDropCnt;             /*A端口丢弃帧数*/
	unsigned int  aPortMacErrCnt;           /*A端口MAC检测错误计数*/
	unsigned int  aPortTxCnt;               /*A端口发送总计数*/
	unsigned int	bPortRvaPortCnt;          /*B收A计数器*/
	unsigned int	bPortDropCnt;             /*B端口丢弃帧数*/
	unsigned int	bPortMacErrCnt;           /*B端口MAC检测错误计数*/
	unsigned int	bPortTxCnt;               /*B端口发送总计数*/
}STR_ESMIB;

typedef struct
{
    unsigned char es_domain;
    unsigned char es_side;
    unsigned char es_location;
    unsigned char es_partition;
    unsigned short rtc_flag;
    unsigned short rtc_send_vl;
    unsigned short rtc_send_vl_index;
    unsigned short rtc_recv_vl1;
    unsigned short rtc_recv_vl2;
}ES_INFO;

typedef struct
{
    unsigned int rtc_high_word;
    unsigned int rtc_low_word;
} RTC_WORD_TYPE;
/****************************************************************************************************************************/

typedef struct
{
    unsigned int Port_Id;
    unsigned int SendData;
    unsigned int Data_Len;
    unsigned int port_type;
    unsigned int ip_dst;
    unsigned short udp_dst;
}SEND_PARA_TYPE;

typedef struct
{
    unsigned int Port_Id;
    unsigned int RecvData;
    unsigned int *Data_Len;
    unsigned int *low_timer;
    unsigned int *high_timer;
    unsigned int *flag;
    unsigned int *src_ip;
    unsigned short *src_port;
    unsigned int port_type;
}RECV_PARA_TYPE;

typedef struct 
{
    unsigned int *srcAddr;
    unsigned int *dstAddr;
    unsigned int DataLen;
}AFDX_COPY_TYPE;


/****************************************************************************************************************************/
typedef struct
{
    unsigned int vl_index;
    unsigned int addr;
    unsigned int  *len;
}VL_RECV_TYPE;

typedef struct
{
    unsigned int vl_index;
    unsigned int sub_vl_index;
}VL_SEND_TYPE;

/*******************************************************************************************************/
typedef struct STR_LoopData{
	unsigned int Loop_Recv_Cnt;
    	char tx_loopbuf[TEST_BUF_SIZE];
    	char rx_loopbuf[TEST_BUF_SIZE];
}LoopTestData;

/* IP defines*/
#define	IP_VER_LEN		0x45
#define	AFDX_IP_TOS		0x00
#define	AFDX_IP_TTL		0x01
#define	UDP_PROTOCOL		0x11
#define	ICMP_PROTOCOL		0x01
#define	IP_PROTOCOL		0x0800
#define	MAC_HDRL		14
#define	IP_HDRL			20
#define	UDP_HDRL		8
#define	HDRL			48
#define FRAGBUFFLIVETIME	128

/* icmp defines*/
#define	ICMP_ECHOREPLY		0x0
#define	ICMP_ECHO		0x8
#define	ICMP_CODE		0x0
#define	ICMP_HDRL		8

int esm_cfg_type;

/*functions*/
unsigned int StartAfdxDriver(unsigned int Addr ,int Len,unsigned int Type);
unsigned int StopAfdxDriver();
unsigned int Afdx_Port_Send(SEND_PARA_TYPE *SendPara);


unsigned int Afdx_port_Rev(RECV_PARA_TYPE *RecvPara);
/*unsigned int AfdxSapRev(unsigned int port_id, unsigned int addr,unsigned int *len,
                        unsigned int *src_ip ,unsigned short *src_port);*/

void ResetDrvMib();
void getdrvmib();
void getmacmib();
void getudpmib();

void macmib();

void txudpmib();
void rxudpmib();

unsigned int gettestloopviid(unsigned int CfgTableAddr);
unsigned int gettestloopportid(unsigned int CfgTableAddr);
/*API接口声明*/

unsigned int afdxComPortRcvMsg(unsigned int com_portid, char *data, unsigned int *datalen,unsigned int *RefreshFlag,unsigned int *timeStamp_l, unsigned int *timeStamp_h);
unsigned int afdxComPortSndMsg(unsigned int com_portid, char *data, unsigned int datalen);
unsigned int afdxSAPPortRcvMsg(unsigned int portid, char *data, unsigned int *datalen,unsigned int *ip_src,unsigned short *udp_src);
unsigned int afdxSAPPortSndMsg(unsigned int portid,char* data,unsigned int datalen,unsigned int ip_dst, unsigned short udp_dst);

void printPciAddr();
void output(void);
void AFDX_main(unsigned int addr);

void memcpy_sw(void *dest, void *src, unsigned int count);
void *memset_sw(void *buffer, char c, unsigned int count);

#endif


