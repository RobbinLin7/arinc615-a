#ifndef FILE_DEFINE_H
#define FILE_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#define RRQ_LCI (1)
#define RRQ_LCS (2)
#define RRQ_LUI (3)
#define WRQ_LCL (11)
#define WRQ_LUS (12)
#define FILE_PATH_BUFFER_SIZE 200
#define Protocol_ver "A3"            //503test
#include "utility.h"

#define BigLittleSwap64(A)  ((((uint64)(A) & 0xff00000000000000) >> 56)|(((uint64)(A) & 0x00ff000000000000) >> 40)|(((uint64)(A) & 0x0000ff0000000000) >> 24)|(((uint64)(A) & 0x000000ff00000000) >> 8) \
    | (((uint64)(A) & 0x00000000ff000000) << 8)|(((uint64)(A) & 0x0000000000ff0000) << 24)|(((uint64)(A) & 0x000000000000ff00) << 40)|(((uint64)(A) & 0x00000000000000ff) << 56))
#define BigLittleSwap32(A)  ((((uint32)(A) & 0xff000000) >> 24)|(((uint32)(A) & 0x00ff0000) >> 8)|(((uint32)(A) & 0x0000ff00) << 8)|(((uint32)(A) & 0x000000ff) << 24))
#define BigLittleSwap16(A)  ((((uint16)(A) & 0xff00) >> 8)|(((uint16)(A) & 0x00ff) << 8))

#pragma pack(1)

typedef struct parts_info_new{
    unsigned char parts_num_len;//部件号的字符个数
    char partsNo[255];//部件号
    unsigned char Amendment_len;//修正字符个数
    char Amendment[255];//修正值
    unsigned char Part_Designation_Text_Length;//部件描述长度
    char Part_Designation_Text[255];//维护信息
}PARTS_INFOR;

typedef struct Hardware{
    unsigned char name_len;//文字名称长度
    char file_name[255];//文字名称
    unsigned char se_num_len;//序列号长度
    char serial_num[255];//序列号
    uint16 parts_num;//部件个数
    PARTS_INFOR *parts;//部件信息
}HARDWARE;

typedef struct{
    unsigned char part_number_length;
    char part_number[255];
    unsigned char amend_length;
    char amend[255];
    unsigned char part_design_length;
    char part_design[255];
}partInfo;

typedef struct{
    unsigned char target_code_length;
    char target_code[255];
    unsigned char serial_length;
    char serial_code[255];
    unsigned short number_part_number;
    partInfo* parts;
}targetInfo;




typedef struct Hfile_info{
    unsigned char len_name;//头文件名长度
    char name[256];//头文件名
    unsigned char load_part_len_name;//加载部分长度
    char load_part_name[256];//加载部分文件名
}HFILE_INFO;
/*------------------INFORMATION------------------*/
struct File_LCI{
    uint32 file_len;//文件大小
    uint16 Pro_ver;//协议版本号
    uint16 status_flag;//状态标志码
    unsigned char stat_des_len;//状态描述长度
    char stat_des[255];//状态描述
};

typedef struct File_LCL{
    uint32 file_len;//文件大小
    char Pro_ver[2];//协议版本号
    uint16 Hw_num;//硬件数
    targetInfo *Hws;//硬件信息
}FILE_LCL;

#pragma pack()

struct File_LCS{
    uint32 file_len;//文件大小
    uint16 Pro_ver;//协议版本号
    uint16 count;//计数
    uint16 stat_no;//信息操作状态码
    uint16 wait_time;//等待时长
    uint16 estimate_time;//估计时长
    unsigned char stat_des_len;//状态描述长度
    unsigned char stat_des[255];//状态描述
};
/*---------------------UPLAOD---------------------*/


typedef struct Hfile_info_LUS{
    unsigned char Hfile_name_len;//头文件名长度
    char Hfile_name[255];//头文件名
    unsigned char load_part_num_name_len;//加载部分名字长度
    char load_part_num_name[255];//加载部分名字
    char load_ritio[3];//加载率
    uint16 load_stat;//加载状态
    unsigned char load_stat_des_len;//加载状态描述长度
    char load_stat_des[255];//加载状态描述
}HINFO_LUS;

struct File_LUI{
    uint32 file_len;//文件大小
    uint16 Pro_ver;//协议版本号
    uint16 status_flag;//状态标志码
    unsigned char stat_des_len;//状态描述长度
    char stat_des[255];//状态描述
};

typedef  struct File_LUR{
    uint32 file_len;//文件长度
    char Pro_ver[2];//协议版本号
    uint16 Header_num;//头文件个数
    struct Hfile_info *Hfile;//头文件信息
}FILE_LUR;

struct File_LUS{
    uint32 file_len;//文件长度
    uint16 Pro_ver;//协议版本号
    uint16 op_stat_code;//操作状态吗
    unsigned char stat_des_len;//状态描述长度
    char stat_des[255];//状态描述
    uint16 counter;//计数器
    uint16 excep_timer;//
    uint16 estim_timer;//估计时长
    char load_list_ratio[3];//列表加载比率
    uint16 hfile_num;//头文件数量
    struct Hfile_info_LUS *LUS_files;//头文件列表
};

typedef struct TargetIDs{
    uint16 TargetID_Num;//*目标HW ID长度
    char *TargetID;//*目标HW ID(1)
}TargetIDs;

typedef struct TIDs_Pos{
    uint16 Pos_Len;//%&位置长度
    char *Pos;//%&位置(1)
}TIDs_Pos;

typedef struct TargetIDs_Pos{
    uint16 TargetID_Pos_Len;//%目标HW ID的位置长度
    char *TargetID_Pos;//%目标HW ID的位置(1)
    uint16 TargetID_Pos_Num;//%目标HW ID位置数量
    TIDs_Pos *Tid_Pos;
}TargetIDs_Pos;

typedef struct DataFile{
    uint16 Ptr_DataFile;//+数据文件指针
    uint16 DataFile_Name_Len;//+数据文件名称长度
    char DataFile_Name[255];//+数据文件名称(1)
    uint16 DataFile_PN_Len;//+数据文件PN长度
    char DataFile_PN[255];//+数据文件PN(1)
    uint32 DataFile_Len;//+数据文件长度
    uint16 DataFile_CRC;//+数据文件CRC
    uint64 DataFile_LenInBytes;//+数据文件长度（以字节为单位）
    uint16 DataFile_CheckVal_Len;//+数据文件校验值长度
    uint16 DataFile_CheckVal_Type;//+数据文件校验值类型
    char DataFile_CheckVal[255];//+数据文件校验值(1)
    //+扩展点NO.6
}DataFile;

typedef struct SupFile{
    uint16 Ptr_SupFile;//#支持文件指针
    uint16 SupFile_Name_Len;//#支持文件名称长度
    uint16 *SupFile_Name;//#支持文件名称(2)
    uint16 SupFile_PN_Len;//#支持文件PN长度
    uint16 *SupFile_PN;//#支持文件PN(2)
    uint16 SupFile_Len;//#支持文件长度
    uint16 SupFile_CRC;//#支持文件CRC
    uint16 SupFile_CheckVal_Len;//#支持文件校验值长度
    uint16 SupFile_CheckVal_Type;//#支持文件校验值类型
    uint16 *SupFile_CheckVal;//#支持文件校验值(1)
    //+扩展点NO.7
}SupFile;


typedef struct File_LUH{
    uint32 file_len;//文件长度
    uint16 LoadFile_Ver;//加载文件格式版本
    uint16 Part_Flag;//Part标志
    uint32 Ptr_PN_Len;//加载PN长度的指针
    uint32 Ptr_TargetID_Num;//目标HW ID数量指针
    uint32 Ptr_DataFile_Num;//数据文件数量指针
    uint32 Ptr_SupFile_Num;//支持文件数量指针
    uint32 Ptr_UserDef_Data;//用户自定义数据指针
    uint32 Ptr_LoadTypeDes_Len;//加载类型描述长度指针
    uint32 Ptr_TargetIDPosition_Num;//目标HW ID的位置数量指针
    uint32 Ptr_LoadCheckVal_Len;//加载校验值长度指针
    //扩展点NO.1
    //uint16 Load_PN_LEN;//加载PN长度
    //char *Load_PN;//加载PN(1)
    //扩展点NO.2
    //uint16 LoadTypeDes_Len;//加载类型描述长度
    //char *LoadTypeDes;//加载类型描述(1)
    //uint16 LoadTypeID;//加载类型ID
    //扩展点NO.3
    //uint16 TargetIDs_Num;//目标HW ID数量
    //TargetIDs *TargetID;
    //扩展点NO.4
    //uint16 TargetIDPosition_Num;//目标HW ID的位置数量
    //TargetIDs_Pos *TargetIDs_Pos;
    //扩展点NO.5
    uint16 DataFile_Num;//数据文件数量
    DataFile *datafile;
    //uint16 SupFile_Num;//支持文件数量
    //SupFile *supfile;

    //+扩展点NO.8
    //char *userdef_data;//用户自定义数据(2)
    //+扩展点NO.9
    //uint16 LoadCheckVal_Len;//加载校验值长度
    uint16 LoadCheckVal_Type;//加载校验值类型
    char *LoadCheckVal;//加载校验值(1)
    uint16 HFile_CRC;//头文件CRC
    uint32 Load_CRC;//加载CRC
}File_LUH;

/*---------------------MEDIA_DEFINED_DOWNLOAD---------------------*/

typedef struct file_info_LNS{
    unsigned char file_name_len;//文件名长度
    char file_name[255];//文件名
    uint16 file_stat;//文件状态
    unsigned char file_stat_des_len;//文件状态描述长度
    char file_stat_des[255];//文件状态描述
}HINFO_LNS;

struct File_LND{
    uint32 file_len;//文件大小
    uint16 Pro_ver;//协议版本号
    uint16 status_flag;//状态标志码
    unsigned char stat_des_len;//状态描述长度
    char stat_des[255];//状态描述
};

struct File_LNR{
    uint32 file_len;//文件长度
    char Pro_ver[2];//协议版本号
    uint16 file_num;//文件个数
    unsigned char user_def_len;//用户定义的数据长度
    char user_def[255];//用户定义的数据
    struct Hfile_info *hfile;
};

struct File_LNS{
    uint32 file_len;//文件长度
    uint16 Pro_ver;//协议版本号
    uint16 op_stat_code;//下载状态码
    unsigned char stat_des_len;//下载状态描述长度
    char stat_des[255];//下载状态描述
    uint16 counter;//计数器
    uint16 excep_timer;//等待时长
    uint16 estim_timer;//估计时长
    char down_list_ratio[3];//列表下载完成比率
    uint16 file_num;//头文件数量
    struct file_info_LNS *LNS_files;//头文件列表
};

/*-----------------------OPERATOR_DEFINED_DOWNLOAD-------------------------*/


typedef struct file_info_LNL{
    unsigned char file_name_len;//文件名长度
    char file_name[255];//文件名
    unsigned char file_stat_des_len;//文件状态描述长度
    char file_stat_des[255];//文件状态描述
}HINFO_LNL;

struct File_LNO{
    uint32 file_len;//文件大小
    uint16 Pro_ver;//协议版本号
    uint16 status_flag;//状态标志码
    unsigned char stat_des_len;//状态描述长度
    char stat_des[255];//状态描述
};

struct File_LNL{
    uint32 file_len;//文件长度
    uint16 Pro_ver;//协议版本号
    uint16 file_num;//文件个数
    struct file_info_LNL *file;//文件信息
};

struct File_LNA{
    uint32 file_len;//文件长度
    char Pro_ver[2];//协议版本号
    uint16 file_num;//文件个数
    struct Hfile_info *file;//文件信息
};

#pragma pack()

#ifdef  __cplusplus
}
#endif
#endif // FILE_DEFINE_H
