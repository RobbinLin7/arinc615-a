#ifndef PROTOCALFILE_H
#define PROTOCALFILE_H


class ProtocalFile
{
public:
    ProtocalFile();
    typedef struct File_LUH_Content {
        unsigned short Ptr_DataFile;//+数据文件指针
        unsigned short DataFile_Name_Len;//+数据文件名称长度
        char DataFile_Name[255];//+数据文件名称(1)
        unsigned short DataFile_PN_Len;//+数据文件PN长度
        char DataFile_PN[255];//+数据文件PN(1)
        unsigned int DataFile_Len;//+数据文件长度
        unsigned short DataFile_CRC;//+数据文件CRC
        long long DataFile_LenInBytes;//+数据文件长度（以字节为单位）
        unsigned short DataFile_CheckVal_Len;//+数据文件校验值长度
        unsigned short DataFile_CheckVal_Type;//+数据文件校验值类型
        char DataFile_CheckVal[255];//+数据文件校验值(1)
    }file_luh_content;

    // LUH文件
    typedef struct File_LUH {
        unsigned int file_len;//文件长度
        unsigned short LoadFile_Ver;//加载文件格式版本
        unsigned short Part_Flag;//Part标志
        unsigned int Ptr_PN_Len;//加载PN长度的指针
        unsigned int Ptr_TargetID_Num;//目标HW ID数量指针
        unsigned short Ptr_DataFile_Num;//数据文件数量指针
        unsigned int Ptr_SupFile_Num;//支持文件数量指针
        unsigned int Ptr_UserDef_Data;//用户自定义数据指针
        unsigned int Ptr_LoadTypeDes_Len;//加载类型描述长度指针
        unsigned int Ptr_TargetIDPosition_Num;//目标HW ID的位置数量指针
        unsigned int Ptr_LoadCheckVal_Len;//加载校验值长度指针
        unsigned short DataFile_Num;//数据文件数量
        //扩展点NO.5
        file_luh_content *datafile;
        unsigned short LoadCheckVal_Type;//加载校验值类型
        char *LoadCheckVal;//加载校验值(1)
        unsigned short HFile_CRC;//头文件CRC
        unsigned int Load_CRC;//加载CRC
    }file_luh;
};

#endif // PROTOCALFILE_H
