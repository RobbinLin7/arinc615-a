#include "create_protocol.h"
#include <stdio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//
#include "file_define.h"
//#include <windows.h>
#include <direct.h>
#include <time.h>
#include "crc.h"
//#include "debug_.h"
//using namespace std;
char ret[10240] = {0};
int BigOrLittel = 0;

File_LCL parsing_LCL(char *filename){
    FILE *fp1 = NULL;
    //errno_t err;
    File_LCL lcl;
    char file_LCL[FILE_PATH_BUFFER_SIZE] = {0};
    int flen;
    sprintf(file_LCL, "%s\\%s%s", getFolderPath, filename, ".LCL");

    memset(&lcl, 0, sizeof(File_LCL));

    //printf("file.LCL %s\n",file_LCL);
    if((fp1 = fopen(file_LCL,"rb"))==NULL)
    {
        printf("cannot open file.LCL %s\n", file_LCL);
        //fclose(fp1);
        /*
        WaitForSingleObject(p1->hmutex, 100);
        p1->flag = LCL_Open_Failed;
        ReleaseMutex(p1->hmutex);
        Sleep(200);
        WaitForSingleObject(p1->hmutex, 100);
        p1->flag = Information_Send_Fail;
        ReleaseMutex(p1->hmutex);*/
        return lcl;
    }

    fseek(fp1, 0L, SEEK_END); /* 定位到文件末尾 */
    flen=ftell(fp1); /* 得到文件大小 */
    fseek(fp1,0L,SEEK_SET);
    //printf("read file len:%d\n",flen);

    fread(&lcl.file_len, 4, 1, fp1);
    //printf("\n\nBigOrLittel:%d\n\n",BigOrLittel);
    BigOrLittel ? lcl.file_len = BigLittleSwap32(lcl.file_len) : 1;

    fread(&lcl.Pro_ver,2,1,fp1);
    BigOrLittel ? lcl.Pro_ver = BigLittleSwap16(lcl.Pro_ver) : 1;
    fread(&lcl.Hw_num,2,1,fp1);
    BigOrLittel ? lcl.Hw_num = BigLittleSwap16(lcl.Hw_num) : 1;

    //printf("file_len:%d\nHw_num:%d\n",lcl.file_len,lcl.Hw_num);
    lcl.Hws = (HARDWARE*) malloc(lcl.Hw_num*sizeof (HARDWARE));//准备获取硬件信息

    for(int i = 0;i < lcl.Hw_num;i++){
        int temp;
        memset(&lcl.Hws[i].name_len,0,sizeof (HARDWARE));
        fread(&lcl.Hws[i].name_len,1,1,fp1);//往第i段硬件中读取信息
        temp = lcl.Hws[i].name_len;
        fread(lcl.Hws[i].file_name,temp,1,fp1);

        fread(&lcl.Hws[i].se_num_len,1,1,fp1);
        temp = lcl.Hws[i].se_num_len;
        fread(lcl.Hws[i].serial_num,temp,1,fp1);

        fread(&lcl.Hws[i].parts_num,2,1,fp1);
        BigOrLittel ? lcl.Hws[i].parts_num = BigLittleSwap16(lcl.Hws[i].parts_num) : 1;

        lcl.Hws[i].parts = (PARTS_INFOR*) malloc(lcl.Hws[i].parts_num*sizeof (PARTS_INFOR));
        for(int j = 0;j < lcl.Hws[i].parts_num; j++){
            memset(&lcl.Hws[i].parts[j],0,sizeof (PARTS_INFOR));

            fread(&lcl.Hws[i].parts[j].parts_num_len,1,1,fp1);
            temp = lcl.Hws[i].parts[j].parts_num_len;
            fread(lcl.Hws[i].parts[j].partsNo,temp,1,fp1);

            fread(&lcl.Hws[i].parts[j].Amendment_len,1,1,fp1);
            temp = lcl.Hws[i].parts[j].Amendment_len;
            fread(lcl.Hws[i].parts[j].Amendment,temp,1,fp1);

            fread(&lcl.Hws[i].parts[j].Part_Designation_Text_Length,1,1,fp1);
            temp = lcl.Hws[i].parts[j].Part_Designation_Text_Length;
            fread(lcl.Hws[i].parts[j].Part_Designation_Text,temp,1,fp1);
        }
    }
    fclose (fp1);
    return lcl;
}

struct File_LUR parsing_LUR(char *filename){
    FILE *fp3;
    char file_LUR[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUR,"%s\\%s%s", sendFolderPath, filename, ".LUR");
    if((fp3=fopen(file_LUR,"rb"))==NULL)
    {
        printf("cannot open file\n");
    }
    int flen;
    fseek(fp3,0L,SEEK_END); /* 定位到文件末尾 */
    flen=ftell(fp3); /* 得到文件大小 */
    fseek(fp3,0L,SEEK_SET);
    struct File_LUR LUR;
    memset(&LUR,0,sizeof (File_LUR));
    int charswicth = 0;
    fread(&LUR.file_len,4,1,fp3);
    //printf("read LUR，length：%ld\n",LUR.file_len);
    BigOrLittel ? LUR.file_len = BigLittleSwap32(LUR.file_len) : 1;
    fread(&LUR.Pro_ver,2,1,fp3);
    fread(&LUR.Header_num,2,1,fp3);

    BigOrLittel ? LUR.Pro_ver = BigLittleSwap16(LUR.Pro_ver) : 1;
    BigOrLittel ? LUR.Header_num = BigLittleSwap16(LUR.Header_num) : 1;

    //cout<<"LUR.file_len : "<<LUR.file_len<<endl;
    //cout<<"LUR.Pro_ver : "<<LUR.Pro_ver<<endl;
    //cout<<"LUR.Header_num : "<<LUR.Header_num<<endl;

    LUR.Hfile = (HFILE_INFO*) malloc(LUR.Header_num*sizeof (HFILE_INFO));
    for(int i = 0;i < LUR.Header_num;i++){
        memset(&LUR.Hfile[i],0,sizeof (HFILE_INFO));

        fread(&LUR.Hfile[i].len_name,1,1,fp3);
        charswicth = LUR.Hfile[i].len_name;
        fread(LUR.Hfile[i].name,charswicth,1,fp3);

        fread(&LUR.Hfile[i].load_part_len_name,1,1,fp3);
        charswicth = LUR.Hfile[i].load_part_len_name;
        fread(LUR.Hfile[i].load_part_name,charswicth,1,fp3);
    }
    fclose (fp3);
    return LUR;
}

struct File_LUS parsing_LUS(char *filename){
    FILE *fp3;
    char file_LUS[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUS, "%s\\%s%s", getFolderPath, filename,".LUS");

    struct File_LUS LUS;
    memset(&LUS,0,sizeof (struct File_LUS));
    if((fp3=fopen(file_LUS,"rb"))==NULL)
    {
        printf("Failed to read LUS: %s\n", file_LUS);
        LUS.file_len = PROTOCOL_FILE_ERROR;
        return LUS;
    }
    int flen;
    fseek(fp3,0L,SEEK_END); /* 定位到文件末尾 */
    flen=ftell(fp3); /* 得到文件大小 */
    fseek(fp3,0L,SEEK_SET);

    //    uint32 file_len;//文件长度
    //    uint16 Pro_ver;//协议版本号
    //    uint16 op_stat_code;//操作状态码
    //    unsigned char stat_des_len;//状态描述长度
    //    char stat_des[255];//状态描述
    //    uint16 counter;//计数器
    //    uint16 excep_timer;//
    //    uint16 estim_timer;//估计时长
    //    int load_list_ratio;//列表加载比率
    //    uint16 hfile_num;//头文件数量
    //    struct Hfile_info_LUS *LUS_files;//头文件列表

    fread(&LUS,8,1,fp3);
    BigOrLittel ? LUS.file_len = BigLittleSwap32(LUS.file_len) : 1;
    BigOrLittel ? LUS.Pro_ver = BigLittleSwap16(LUS.Pro_ver) : 1;
    BigOrLittel ? LUS.op_stat_code = BigLittleSwap16(LUS.op_stat_code) : 1;
    fread(&LUS.stat_des_len,1,1,fp3);
    memset(LUS.stat_des,0,255);
    fread(LUS.stat_des,LUS.stat_des_len,1,fp3);

    fread(&LUS.counter,sizeof (LUS.counter),1,fp3);
    BigOrLittel ? LUS.counter = BigLittleSwap16(LUS.counter) : 1;
    fread(&LUS.excep_timer,sizeof (LUS.excep_timer),1,fp3);
    BigOrLittel ? LUS.excep_timer = BigLittleSwap16(LUS.excep_timer) : 1;
    fread(&LUS.estim_timer,sizeof (LUS.estim_timer),1,fp3);
    BigOrLittel ? LUS.estim_timer = BigLittleSwap16(LUS.estim_timer) : 1;
    fread(LUS.load_list_ratio, 3,1,fp3);

    fread(&LUS.hfile_num,sizeof (LUS.hfile_num),1,fp3);
    BigOrLittel ? LUS.hfile_num = BigLittleSwap16(LUS.hfile_num) : 1;
    LUS.LUS_files = (HINFO_LUS*)malloc(sizeof (HINFO_LUS)*LUS.hfile_num);

    for (int i = 0;i<LUS.hfile_num;i++) {
        //        unsigned char Hfile_name_len;//头文件名长度
        //        char Hfile_name[255];//头文件名
        //        unsigned char load_part_num_name_len;//加载部分名字长度
        //        char load_part_num_name[255];//加载部分名字
        //        char load_ritio[3];//加载率
        //        uint16 load_stat;//加载状态
        //        unsigned char load_stat_des_len;//加载状态描述长度
        //        char load_stat_des[255];//加载状态描述
        int temp = 0;
        fread(&LUS.LUS_files[i].Hfile_name_len,1,1,fp3);
        temp = LUS.LUS_files[i].Hfile_name_len;
        memset(LUS.LUS_files[i].Hfile_name,0,255);
        fread(LUS.LUS_files[i].Hfile_name,temp,1,fp3);

        fread(&LUS.LUS_files[i].load_part_num_name_len,1,1,fp3);
        temp = LUS.LUS_files[i].load_part_num_name_len;
        memset(LUS.LUS_files[i].load_part_num_name,0,255);
        fread(LUS.LUS_files[i].load_part_num_name,temp,1,fp3);

        fread(LUS.LUS_files[i].load_ritio,3,1,fp3);
        fread(&LUS.LUS_files[i].load_stat,2,1,fp3);
        BigOrLittel ? LUS.LUS_files[i].load_stat = BigLittleSwap16(LUS.LUS_files[i].load_stat) : 1;

        fread(&LUS.LUS_files[i].load_stat_des_len,1,1,fp3);
        temp = LUS.LUS_files[i].load_stat_des_len;
        memset(LUS.LUS_files[i].load_stat_des,0,255);
        fread(LUS.LUS_files[i].load_stat_des,temp,1,fp3);
    }

    fclose(fp3);
    return LUS;
}

int parsing_LNS(char *filename, struct File_LNS *LNS){
    FILE *fp3;
    char file_LNS[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LNS, "%s\\%s%s", getFolderPath, filename, ".LNS");

    int i = 0;

    //struct File_LNS LNS;
    //memset(LNS, 0, sizeof (struct File_LNS));

    if((fp3=fopen(file_LNS,"rb"))==NULL)
    {
        //qDebug() << "parsing_LNS: Failed to read LNS\n";
        //LNS.file_len = -1;
        return -1;
    }

    //    uint32 file_len;//文件长度
    //    uint16 Pro_ver;//协议版本号
    //    uint16 op_stat_code;//操作状态吗
    //    unsigned char stat_des_len;//状态描述长度
    //    char stat_des[255];//状态描述
    //    uint16 counter;//计数器
    //    uint16 excep_timer;//
    //    uint16 estim_timer;//估计时长
    //    int load_list_ratio;//列表加载比率
    //    uint16 hfile_num;//头文件数量
    //    struct file_info_LNS *LNS_files;//头文件列表

    fread(LNS, 8, 1, fp3);
    BigOrLittel ? LNS->file_len = BigLittleSwap32(LNS->file_len) : 1;
    BigOrLittel ? LNS->Pro_ver = BigLittleSwap16(LNS->Pro_ver) : 1;
    BigOrLittel ? LNS->op_stat_code = BigLittleSwap16(LNS->op_stat_code) : 1;
    fread(&LNS->stat_des_len,1,1,fp3);
    memset(LNS->stat_des,0,255);
    int temp0 = LNS->stat_des_len;
    fread(LNS->stat_des,temp0,1,fp3);
    //printf("HHHHHHstat_des_len:%d\n",LNS.stat_des_len);
    fread(&LNS->counter,sizeof (LNS->counter),1,fp3);
    BigOrLittel ? LNS->counter = BigLittleSwap16(LNS->counter) : 1;
    //("sizeof(LNS.counter):%lld\n",sizeof(LNS.counter));
    fread(&LNS->excep_timer,sizeof (LNS->excep_timer),1,fp3);
    BigOrLittel ? LNS->excep_timer = BigLittleSwap16(LNS->excep_timer) : 1;
    //printf("sizeof(LNS.excep_timer):%lld\n",sizeof(LNS.excep_timer));
    fread(&LNS->estim_timer,sizeof (LNS->estim_timer),1,fp3);
    BigOrLittel ? LNS->estim_timer = BigLittleSwap16(LNS->estim_timer) : 1;
    //printf("sizeof(LNS.estim_timer):%lld\n",sizeof(LNS.estim_timer));
    fread(LNS->down_list_ratio, 1, 3, fp3);
    //printf("HHHHHHdown_list_ratio:%d\n", (LNS->down_list_ratio[0] - '0') * 100 + (LNS->down_list_ratio[1] - '0') * 10 + (LNS->down_list_ratio[2] - '0'));

    //printf("sizeof(LNS.down_list_ratio):%lld\n",sizeof(LNS.down_list_ratio));
    fread(&LNS->file_num, sizeof (LNS->file_num), 1, fp3);
    BigOrLittel ? LNS->file_num = BigLittleSwap16(LNS->file_num) : 1;
    //printf("HHHHHHfile_num:%d\n",LNS.file_num);
    //printf("sizeof(LNS.file_num):%lld\n",sizeof(LNS.file_num));
#if 1
    LNS->LNS_files = (HINFO_LNS* ) malloc (sizeof (HINFO_LNS) * LNS->file_num);

    if(LNS->LNS_files == NULL)
    {
        printf("malloc ERROR!\n");
        return -1;
    }
    memset(LNS->LNS_files, 0, sizeof(HINFO_LNS) * LNS->file_num);
    for (i = 0; i<LNS->file_num; i++) {
        //        unsigned char file_name_len;//文件名长度
        //        char file_name[255];//文件名
        //        uint16 file_stat;//文件状态
        //        unsigned char file_stat_des_len;//文件状态描述长度
        //        char file_stat_des[255];//文件状态描述
        int temp = 0;
        fread(&LNS->LNS_files[i].file_name_len, 1, 1, fp3);
        temp = LNS->LNS_files[i].file_name_len;
        memset(LNS->LNS_files[i].file_name, 0, 255);
        fread(LNS->LNS_files[i].file_name, 1, temp, fp3);

        fread(&LNS->LNS_files[i].file_stat, 2, 1, fp3);
        BigOrLittel ? LNS->LNS_files[i].file_stat = BigLittleSwap16(LNS->LNS_files[i].file_stat) : 1;

        fread(&LNS->LNS_files[i].file_stat_des_len, 1, 1, fp3);
        temp = LNS->LNS_files[i].file_stat_des_len;
        memset(LNS->LNS_files[i].file_stat_des, 0, 255);
        fread(LNS->LNS_files[i].file_stat_des, 1, temp, fp3);
    }
#endif
    fclose(fp3);

    fp3 = NULL;

    return 0;
}

struct File_LUR create_LUR(char filename[255],int header_num,FileList filelist){
    struct File_LUR LUR_demo;
    //            struct File_LUR{
    //                int file_len;//文件长度
    //                short Pro_ver;//协议版本号
    //                short Header_num;//头文件个数
    //                struct Hfile_info Hfile[];//头文件信息
    //            }__attribute__((packed));
    //            1)结构体变量的首地址能够被其最宽基本类型成员的大小所整除；
    //            2)结构体每个成员相对于结构体首地址的偏移量（offset）都是成员大小的整数倍，如有需要编译器会在成员之间加上填充字节（internal padding）；
    //            3)结构体的总大小为结构体最宽基本类型成员大小的整数倍，如有需要编译器会在最末一个成员之后加上填充字节（trailing padding）。
    //**********LUR构造**********
    //格式↓
    //    struct File_LUR{
    //        int file_len;//文件长度
    //        short Pro_ver;//协议版本号
    //        short Header_num;//头文件个数
    //        struct Hfile_info *Hfile;//头文件信息
    //    }__attribute__((packed));

    LUR_demo.file_len = 8;
    LUR_demo.Pro_ver = Protocol_ver;
    LUR_demo.Header_num = header_num;

    //头文件信息格式↓
    //    struct Hfile_info{
    //    unsigned char len_name;//头文件名长度
    //    char name[255];//头文件名
    //    unsigned char load_part_len_name;//头文件名长度
    //    char load_part_name[255];//头文件名
    //    }__attribute__((packed));
    LUR_demo.Hfile = (HFILE_INFO*) malloc(LUR_demo.Header_num*sizeof (HFILE_INFO));

    for(int i = 0;i<LUR_demo.Header_num;i++){
        memset(LUR_demo.Hfile[i].name,0,sizeof (LUR_demo.Hfile[i].name));
        strcpy(LUR_demo.Hfile[i].name,filelist.oneFile[i].name);
        LUR_demo.Hfile[i].len_name = strlen(LUR_demo.Hfile[i].name) + 1;
        LUR_demo.file_len += 1 + LUR_demo.Hfile[i].len_name;

        char name[255] = {0};
        if(strlen(filelist.oneFile[i].path)==0){
            strcpy(name,"../Upload/");
        }else {
            sprintf(name,"%s%s/","../Upload/",filelist.oneFile[i].path);
        }
        memset(LUR_demo.Hfile[i].load_part_name,0,sizeof (LUR_demo.Hfile[i].load_part_name));
        strcpy(LUR_demo.Hfile[i].load_part_name,name);//给目标端创建的路径
        LUR_demo.Hfile[i].load_part_len_name = strlen(LUR_demo.Hfile[i].load_part_name) + 1;
        LUR_demo.file_len += 1 + LUR_demo.Hfile[i].load_part_len_name;
    }

    uint32 file_len;
    uint16 Pro_ver;
    uint16 Header_num;
    BigOrLittel ? file_len = BigLittleSwap32(LUR_demo.file_len) : file_len = LUR_demo.file_len;
    BigOrLittel ? Pro_ver = BigLittleSwap16(LUR_demo.Pro_ver) : Pro_ver = LUR_demo.Pro_ver;
    BigOrLittel ? Header_num = BigLittleSwap16(LUR_demo.Header_num) : Header_num = LUR_demo.Header_num;


    //**********LUR构造完毕**********
    char file_LUR[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUR,"%s\\%s%s", sendFolderPath, filename, ".LUR");
    FILE *fp2;
    if((fp2=fopen(file_LUR,"wb"))==NULL)
    {
        printf("cannot open file_LUR!!!!!!!!!!!\n");
        return LUR_demo;
    }
    fwrite (&file_len ,4, 1, fp2);
    fwrite (&Pro_ver ,2, 1, fp2);
    fwrite (&Header_num ,2, 1, fp2);
    for (int i = 0;i<LUR_demo.Header_num;i++) {
        fwrite (&LUR_demo.Hfile[i].len_name, 1, 1, fp2);
        fwrite (LUR_demo.Hfile[i].name,LUR_demo.Hfile[i].len_name, 1, fp2);

        fwrite (&LUR_demo.Hfile[i].load_part_len_name, 1, 1, fp2);
        fwrite (LUR_demo.Hfile[i].load_part_name,LUR_demo.Hfile[i].load_part_len_name, 1, fp2);
    }
    fclose(fp2);
    fp2 = NULL;
    return LUR_demo;
}

int create_LUR_new(char *filename,int header_num, FileList filelist){
    struct File_LUR LUR_demo;
    //            struct File_LUR{
    //                int file_len;//文件长度
    //                short Pro_ver;//协议版本号
    //                short Header_num;//头文件个数
    //                struct Hfile_info Hfile[];//头文件信息
    //            }__attribute__((packed));
    //            1)结构体变量的首地址能够被其最宽基本类型成员的大小所整除；
    //            2)结构体每个成员相对于结构体首地址的偏移量（offset）都是成员大小的整数倍，如有需要编译器会在成员之间加上填充字节（internal padding）；
    //            3)结构体的总大小为结构体最宽基本类型成员大小的整数倍，如有需要编译器会在最末一个成员之后加上填充字节（trailing padding）。
    //**********LUR构造**********
    //格式↓
    //    struct File_LUR{
    //        int file_len;//文件长度
    //        short Pro_ver;//协议版本号
    //        short Header_num;//头文件个数
    //        struct Hfile_info *Hfile;//头文件信息
    //    }__attribute__((packed));

    memset(&LUR_demo, 0, sizeof(File_LUR));

    LUR_demo.file_len = 8;
    LUR_demo.Pro_ver = Protocol_ver;
    LUR_demo.Header_num = 1;

    //头文件信息格式↓
    //    struct Hfile_info{
    //    unsigned char len_name;//头文件名长度
    //    char name[255];//头文件名
    //    unsigned char load_part_len_name;//头文件名长度
    //    char load_part_name[255];//头文件名
    //    }__attribute__((packed));
    LUR_demo.Hfile = (HFILE_INFO*) malloc(LUR_demo.Header_num*sizeof (HFILE_INFO));

    char name[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(name, "%s.LUH", filename);
    for(int i = 0;i<LUR_demo.Header_num;i++){
        memset(LUR_demo.Hfile[i].name, 0 ,sizeof (LUR_demo.Hfile[i].name));
        strcpy(LUR_demo.Hfile[i].name, name);
        LUR_demo.Hfile[i].len_name = strlen(LUR_demo.Hfile[i].name) + 1;
        LUR_demo.file_len += 1 + LUR_demo.Hfile[i].len_name;

        //char name[255] = {0};
        if(strlen(filelist.oneFile[i].path)==0){
            strcpy(name, sendFolderPath);
        }else {
            //sprintf(name, "%s%s/", sendFolderPath, filelist.oneFile[i].path);
            strcpy(name, filelist.oneFile[i].path);
        }
        //memset(LUR_demo.Hfile[i].load_part_name, 0, 255);
        strcpy(LUR_demo.Hfile[i].load_part_name, name);//LUH文件存储path
        LUR_demo.Hfile[i].load_part_len_name = strlen(LUR_demo.Hfile[i].load_part_name) + 1;
        LUR_demo.file_len += 1 + LUR_demo.Hfile[i].load_part_len_name;
    }

    uint32 file_len;
    uint16 Pro_ver;
    uint16 Header_num;
    BigOrLittel ? file_len = BigLittleSwap32(LUR_demo.file_len) : file_len = LUR_demo.file_len;
    BigOrLittel ? Pro_ver = BigLittleSwap16(LUR_demo.Pro_ver) : Pro_ver = LUR_demo.Pro_ver;
    BigOrLittel ? Header_num = BigLittleSwap16(LUR_demo.Header_num) : Header_num = LUR_demo.Header_num;


    //**********LUR构造完毕**********
    char file_LUR[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUR,"%s\\%s%s", sendFolderPath, filename, ".LUR");
    FILE *fp2;
    if((fp2=fopen(file_LUR,"wb"))==NULL)
    {
        printf("cannot open file_LUR!!!!!!!!!!!\n");
        return (-1);
    }
    fwrite (&file_len, 4, 1, fp2);
    fwrite (&Pro_ver, 2, 1, fp2);
    fwrite (&Header_num ,2, 1, fp2);
    for (int i = 0;i<LUR_demo.Header_num;i++) {
        fwrite (&LUR_demo.Hfile[i].len_name, 1, 1, fp2);
        fwrite (LUR_demo.Hfile[i].name,LUR_demo.Hfile[i].len_name, 1, fp2);

        fwrite (&LUR_demo.Hfile[i].load_part_len_name, 1, 1, fp2);
        fwrite (LUR_demo.Hfile[i].load_part_name,LUR_demo.Hfile[i].load_part_len_name, 1, fp2);
    }
    fclose(fp2);
    fp2 = NULL;

    //释放内存
    free(LUR_demo.Hfile);
    LUR_demo.Hfile = NULL;
    return 0;
}


int create_LNR(char filename[255],int header_num,FileList filelist){
    struct File_LNR LNR_demo;
    //    struct File_LNR{
    //        uint32 file_len;//文件长度
    //        uint16 Pro_ver;//协议版本号
    //        uint16 file_num;//文件个数
    //        unsigned char user_def_len;//用户定义的数据长度
    //        char user_def[255];//用户定义的数据
    //        struct Hfile_info *hfile;
    //    }__attribute__((packed));
    //            1)结构体变量的首地址能够被其最宽基本类型成员的大小所整除；
    //            2)结构体每个成员相对于结构体首地址的偏移量（offset）都是成员大小的整数倍，如有需要编译器会在成员之间加上填充字节（internal padding）；
    //            3)结构体的总大小为结构体最宽基本类型成员大小的整数倍，如有需要编译器会在最末一个成员之后加上填充字节（trailing padding）。

    LNR_demo.file_len = 8;
    LNR_demo.Pro_ver = 2;
    LNR_demo.file_num = header_num;

    //头文件信息格式↓
    //    struct Hfile_info{
    //    unsigned char len_name;//头文件名长度
    //    char name[255];//头文件名
    //    }__attribute__((packed));
    LNR_demo.hfile = (HFILE_INFO*) malloc(LNR_demo.file_num*sizeof (HFILE_INFO));

    for(int i = 0;i<LNR_demo.file_num;i++){
        memset(LNR_demo.hfile[i].name,0,sizeof (LNR_demo.hfile[i].name));
        strcpy(LNR_demo.hfile[i].name,filelist.oneFile[i].name);

        LNR_demo.hfile[i].len_name = strlen(LNR_demo.hfile[i].name) + 1;
        LNR_demo.file_len+=1+LNR_demo.hfile[i].len_name;
        //        char name[255] = {0};
        //        if(strlen(filelist.oneFile[i].path)==0){
        //            strcpy(name,"../Upload/");
        //        }else {
        //            sprintf(name,"%s%s/","../Upload/",filelist.oneFile[i].path);
        //        }
        //        memset(LNR_demo.Hfile[i].load_part_name,0,sizeof (LNR_demo.Hfile[i].load_part_name));
        //        strcpy(LNR_demo.Hfile[i].load_part_name,name);//给目标端创建的路径
        //        LNR_demo.hfile[i].load_part_len_name = 0;
        //        LNR_demo.file_len += 1 + LNR_demo.Hfile[i].load_part_len_name;
    }
    memset(LNR_demo.user_def,0,255);
    memcpy(LNR_demo.user_def,"sefsawd",strlen("sefsawd"));
    LNR_demo.user_def_len = 1+strlen(LNR_demo.user_def);
    LNR_demo.file_len += 1+LNR_demo.user_def_len;
    //**********LNR构造完毕**********
    char file_LNR[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LNR,"%s\\%s%s", sendFolderPath, filename, ".LNR");
    FILE *fp2;
    if((fp2=fopen(file_LNR,"wb"))==NULL)
    {
        printf("cannot open file_LNR!!!!!!!!!!!\n");
        return -1;
    }

    BigOrLittel ? LNR_demo.file_len = BigLittleSwap32(LNR_demo.file_len) : 1;
    BigOrLittel ? LNR_demo.Pro_ver = BigLittleSwap16(LNR_demo.Pro_ver) : 1;
    BigOrLittel ? LNR_demo.file_num = BigLittleSwap16(LNR_demo.file_num) : 1;

    fwrite (&LNR_demo ,8, 1, fp2);
    BigOrLittel ? LNR_demo.file_len = BigLittleSwap32(LNR_demo.file_len) : 1;
    BigOrLittel ? LNR_demo.Pro_ver = BigLittleSwap16(LNR_demo.Pro_ver) : 1;
    BigOrLittel ? LNR_demo.file_num = BigLittleSwap16(LNR_demo.file_num) : 1;

    //cout<<"LNR_demo.file_num"<<LNR_demo.file_num<<endl;
    for (int i = 0;i<LNR_demo.file_num;i++) {
        fwrite (&LNR_demo.hfile[i].len_name, 1, 1, fp2);
        fwrite (LNR_demo.hfile[i].name,LNR_demo.hfile[i].len_name, 1, fp2);
        //cout<<"LNR_demo.name"<<LNR_demo.hfile[i].name<<endl;

        //        fwrite (&LNR_demo.hfile[i].load_part_len_name, 1, 1, fp2);
        //        fwrite (LNR_demo.Hfile[i].load_part_name,LNR_demo.Hfile[i].load_part_len_name, 1, fp2);
    }
    fwrite (&LNR_demo.user_def_len, 1, 1, fp2);
    fwrite (LNR_demo.user_def,LNR_demo.user_def_len, 1, fp2);
    fclose(fp2);
    fp2 = NULL;
    return 0;
}

struct File_LNA create_LNA(char filename[255],int header_num, FileList filelist){
    struct File_LNA LNA_demo;
    //    struct File_LNL{
    //        uint32 file_len;//文件长度
    //        uint16 Pro_ver;//协议版本号
    //        uint16 file_num;//文件个数
    //        struct file_info_LNL *file;//文件信息
    //    }__attribute__((packed));
    //            1)结构体变量的首地址能够被其最宽基本类型成员的大小所整除；
    //            2)结构体每个成员相对于结构体首地址的偏移量（offset）都是成员大小的整数倍，如有需要编译器会在成员之间加上填充字节（internal padding）；
    //            3)结构体的总大小为结构体最宽基本类型成员大小的整数倍，如有需要编译器会在最末一个成员之后加上填充字节（trailing padding）。

    LNA_demo.file_len = 8;
    LNA_demo.Pro_ver = Protocol_ver;
    LNA_demo.Header_num = header_num;

    //    struct file_info_LNL{
    //    unsigned char len_name;//头文件名长度
    //    char name[255];//头文件名
    //    }__attribute__((packed));
    LNA_demo.file = (HFILE_INFO*) malloc(LNA_demo.Header_num*sizeof (HFILE_INFO));
    memset(LNA_demo.file, 0, LNA_demo.Header_num*sizeof (HFILE_INFO));
    for(int i = 0;i<LNA_demo.Header_num;i++){
        memset(LNA_demo.file[i].name,0,sizeof (LNA_demo.file[i].name));
        strcpy(LNA_demo.file[i].name,filelist.oneFile[i].name);

        LNA_demo.file[i].len_name = strlen(LNA_demo.file[i].name) + 1;
        LNA_demo.file_len += 1 + LNA_demo.file[i].len_name;
    }
    //**********LNA构造完毕**********
    char file_LNA[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LNA,"%s\\%s%s", sendFolderPath, filename,".LNA");
    FILE *fp2;
    if((fp2=fopen(file_LNA,"wb"))==NULL)
    {
        //printf("cannot open file_LNA!!!!!!!!!!!\n");
        return LNA_demo;
    }

    BigOrLittel ? LNA_demo.file_len = BigLittleSwap32(LNA_demo.file_len) : 1;
    BigOrLittel ? LNA_demo.Pro_ver = BigLittleSwap16(LNA_demo.Pro_ver) : 1;
    BigOrLittel ? LNA_demo.Header_num = BigLittleSwap16(LNA_demo.Header_num) : 1;

    fwrite (&LNA_demo ,8, 1, fp2);
    BigOrLittel ? LNA_demo.file_len = BigLittleSwap32(LNA_demo.file_len) : 1;
    BigOrLittel ? LNA_demo.Pro_ver = BigLittleSwap16(LNA_demo.Pro_ver) : 1;
    BigOrLittel ? LNA_demo.Header_num = BigLittleSwap16(LNA_demo.Header_num) : 1;
    for (int i = 0;i<LNA_demo.Header_num;i++) {
        fwrite (&LNA_demo.file[i].len_name, 1, 1, fp2);
        fwrite (LNA_demo.file[i].name,LNA_demo.file[i].len_name, 1, fp2);
    }
    fclose(fp2);
    fp2 = NULL;
    return LNA_demo;
}

struct File_LNL parsing_LNL(char filename[255]){
    FILE *fp3;
    char file_LNL[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LNL,"%s\\%s%s", getFolderPath, filename, ".LNL");

    struct File_LNL LNL;
    memset(&LNL,0,sizeof (struct File_LNL));

    LNL.file_len = 0;
    if((fp3=fopen(file_LNL,"rb"))==NULL)
    {
        printf("Failed to read LNS\n");
        LNL.file_len = -1;
        return LNL;
    }
    //    uint32 file_len;//文件长度
    //    uint16 Pro_ver;//协议版本号
    //    uint16 file_num;//文件个数
    //    struct file_info_LNL *file;//文件信息
    fread(&LNL,8,1,fp3);
    BigOrLittel ? LNL.file_len = BigLittleSwap32(LNL.file_len) : 1;
    BigOrLittel ? LNL.Pro_ver = BigLittleSwap16(LNL.Pro_ver) : 1;
    BigOrLittel ? LNL.file_num = BigLittleSwap16(LNL.file_num) : 1;

    LNL.file = (HINFO_LNL*)malloc(sizeof (HINFO_LNL) * LNL.file_num);
    memset(LNL.file, 0, sizeof (HINFO_LNL) * LNL.file_num);
    for (int i = 0;i<LNL.file_num;i++) {
        //        unsigned char file_name_len;//文件名长度
        //        char file_name[255];//文件名
        //        unsigned char file_stat_des_len;//文件状态描述长度
        //        char file_stat_des[255];//文件状态描述
        //printf("HHHHHHHHHHHHH:%ud\n",LNL.file_num);
        int temp = 0;
        fread(&LNL.file[i].file_name_len,1,1,fp3);
        temp = LNL.file[i].file_name_len;
        memset(LNL.file[i].file_name,0,255);
        fread(LNL.file[i].file_name,temp,1,fp3);

        fread(&LNL.file[i].file_stat_des_len,1,1,fp3);
        temp = LNL.file[i].file_stat_des_len;
        memset(LNL.file[i].file_stat_des,0,255);
        fread(LNL.file[i].file_stat_des,temp,1,fp3);
    }
    fclose(fp3);
    return LNL;
}

File_LUH create_LUH(char filename[],
                    uint16 Load_PN_LEN,
                    uint16 *Load_PN,
                    uint16 LoadTypeDes_Len,
                    uint16 *LoadTypeDes,
                    uint16 LoadTypeID,
                    FileList filelist)
{//先构造LUH结构体 ，再写LUH文件
    FILE *fp;
    File_LUH LUH;
    memset(&LUH, 0, sizeof (LUH));
    char file_LUH[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUH, "%s\\%s%s", sendFolderPath, filename, ".LUH");
    if((fp = fopen(file_LUH,"wb+"))==NULL)
    {
        printf("Failed to read LUH: %s\n", file_LUH);
        LUH.file_len = -1;
        //return LUH;
    }
    else
    {
        LUH.file_len = 20;
    }

    
    LUH.LoadFile_Ver = LOADFILE_VER;
    LUH.Part_Flag = 0x00;

    //初始化绝对指针值
    LUH.Ptr_PN_Len = 20;//1111111111
    LUH.Ptr_TargetID_Num = 0;
    LUH.Ptr_DataFile_Num = 0;
    LUH.Ptr_SupFile_Num = 0;
    LUH.Ptr_UserDef_Data = 0;
    LUH.Ptr_LoadTypeDes_Len = 0;
    LUH.Ptr_TargetIDPosition_Num = 0;
    LUH.Ptr_LoadCheckVal_Len = 0;
    uint32 offset = 20;
    //加载PN长度
    //加载PN
    LUH.Load_PN_LEN = Load_PN_LEN;
    if(LUH.Load_PN_LEN == 0)LUH.Load_PN_LEN = 2;
    LUH.Load_PN = (char *) malloc (LUH.Load_PN_LEN);
    memset(LUH.Load_PN, 0, Load_PN_LEN);
    memcpy(LUH.Load_PN, Load_PN, Load_PN_LEN);

    //    加载类型描述长度
    //    加载类型描述
    //    加载类型ID
    offset += 1 + LUH.Load_PN_LEN / 2;
    LUH.Ptr_LoadTypeDes_Len = offset;//222222222
    LUH.LoadTypeDes_Len = LoadTypeDes_Len;
    if(LUH.LoadTypeDes_Len == 0)LUH.LoadTypeDes_Len = 2;
    LUH.LoadTypeDes = (char *) malloc (LUH.LoadTypeDes_Len);
    memset(LUH.LoadTypeDes, 0, LoadTypeDes_Len);
    memcpy(LUH.LoadTypeDes, LoadTypeDes, LoadTypeDes_Len);
    LUH.LoadTypeID = LoadTypeID;

    offset += 1 + LUH.LoadTypeDes_Len / 2 + 1;
    //    目标HW ID数量
    //      *目标HW ID长度
    //      *目标HW ID
    LUH.Ptr_TargetID_Num = offset;//333333333
    LUH.TargetIDs_Num = 0;
    LUH.TargetID = (TargetIDs *) malloc (sizeof (TargetIDs) *  LUH.TargetIDs_Num);
    memset(&LUH.TargetID, 0, sizeof (LUH.TargetID));
    offset += 1;
    for(int i = 0;i < LUH.TargetIDs_Num;++i){
        LUH.TargetID[i].TargetID_Num = 4;
        LUH.TargetID[i].TargetID = (char *) malloc (LUH.TargetID[i].TargetID_Num);
        memset(LUH.TargetID[i].TargetID, 0, LUH.TargetID[i].TargetID_Num);
        memset(LUH.TargetID[i].TargetID, 2, LUH.TargetID[i].TargetID_Num);

        offset += 2 + LUH.TargetID[i].TargetID_Num;//记录该部分长度作为Ptr_TargetIDPosition_Num偏移
    }

    //    目标HW ID的位置数量
    //      %目标HW ID的位置长度
    //       %目标HW ID的位置
    //       %目标HW ID位置数量
    //          %&位置长度
    //          %&位置
    LUH.Ptr_TargetIDPosition_Num = 0;//44444444
    LUH.TargetIDPosition_Num = 0;
    LUH.TargetIDs_Pos = (TargetIDs_Pos *) malloc (sizeof (TargetIDs_Pos) *  LUH.TargetIDPosition_Num);
    for(int i = 0;i < LUH.TargetIDPosition_Num;++i){
        LUH.TargetIDs_Pos[i].TargetID_Pos_Len = 2;
        LUH.TargetIDs_Pos[i].TargetID_Pos = (char *)
                malloc (LUH.TargetIDs_Pos[i].TargetID_Pos_Len);
        memset(LUH.TargetIDs_Pos[i].TargetID_Pos, 0, 2);

        offset += 2 + LUH.TargetIDs_Pos[i].TargetID_Pos_Len + 2;

        LUH.TargetIDs_Pos[i].TargetID_Pos_Num = 2;
        LUH.TargetIDs_Pos[i].Tid_Pos = (TIDs_Pos *) malloc (sizeof (TIDs_Pos) *  LUH.TargetIDs_Pos[i].TargetID_Pos_Num);
        memset(LUH.TargetIDs_Pos[i].Tid_Pos, 0, sizeof (TIDs_Pos) *  LUH.TargetIDs_Pos[i].TargetID_Pos_Num);

        for(int j = 0;j < LUH.TargetIDs_Pos[i].TargetID_Pos_Num;++j){
            LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len = 2;
            LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos = (char *) malloc (LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len);
            memset(LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos, 0, 2);

            offset += 2 + LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len;
        }
    }

    //    数据文件数量
    //       +数据文件指针
    //       +数据文件名称长度
    //       +数据文件名称
    //       +数据文件PN长度
    //       +数据文件PN
    //       +数据文件长度
    //       +数据文件CRC
    //       +数据文件长度（以字节为单位）
    //       +数据文件校验值长度
    //       +数据文件校验值类型
    //       +数据文件校验值
    //       +扩展点NO.6

    LUH.Ptr_DataFile_Num = offset;//5555555
	LUH.DataFile_Num = filelist.file_number;
    LUH.datafile = (DataFile* ) malloc(sizeof (DataFile) * LUH.DataFile_Num);
    memset(LUH.datafile,0, LUH.DataFile_Num);
    offset += 1;
    for(int i = 0;i < LUH.DataFile_Num;++i){
        //LUH.datafile[i].Ptr_DataFile = 0x0000;

        LUH.datafile[i].DataFile_Name_Len = ((strlen(filelist.oneFile[i].name) + 1 + 1) / 2) * 2;
        if(LUH.datafile[i].DataFile_Name_Len == 0)LUH.datafile[i].DataFile_Name_Len = 2;
        LUH.datafile[i].DataFile_Name = (char *)malloc (LUH.datafile[i].DataFile_Name_Len);
        memset(LUH.datafile[i].DataFile_Name,0, LUH.datafile[i].DataFile_Name_Len);
        memcpy(LUH.datafile[i].DataFile_Name,filelist.oneFile[i].name, LUH.datafile[i].DataFile_Name_Len);
        offset += 1 + LUH.datafile[i].DataFile_Name_Len / 2 + 1;

        LUH.datafile[i].DataFile_PN_Len = ((strlen(filelist.oneFile[i].path) + 1 + 1)/ 2) * 2;
        if(LUH.datafile[i].DataFile_PN_Len == 0)LUH.datafile[i].DataFile_PN_Len = 2;
        LUH.datafile[i].DataFile_PN = (char *) malloc (LUH.datafile[i].DataFile_PN_Len);
        memset(LUH.datafile[i].DataFile_PN, 0, LUH.datafile[i].DataFile_PN_Len);
       // memcpy(LUH.datafile[i].DataFile_PN,filelist.oneFile[i].path, LUH.datafile[i].DataFile_PN_Len);
        offset += 1 + LUH.datafile[i].DataFile_PN_Len / 2;

        LUH.datafile[i].DataFile_Len = filelist.oneFile[i].file_len / 2;
        offset += 2;

        //循环读取文件并求出crc16
        FILE *fp;
        char file_data[FILE_PATH_BUFFER_SIZE] = {0};
        sprintf(file_data,"%s\\%s", fileFolderPath, filelist.oneFile[i].name);
        /* 20211202：修改数据文件存储路径为用户指定路径，by liuli  */
       // sprintf(file_data,"%s\\%s", filelist.oneFile[i].path, filelist.oneFile[i].name);
        printf("flie data path:%s\n\n", file_data);
        if((fp = fopen(file_data, "rb"))==NULL)
        {
            printf("Failed to read datafile: %s\n", filelist.oneFile[i].name);
            return LUH;
        }

        uint32 flen;
        fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
        flen = ftell(fp); /* 得到文件大小 */
        fseek(fp,0L,SEEK_SET);

        unsigned char *to_crc = (unsigned char * ) malloc(flen);
        fread(to_crc, 1, flen, fp);
        //cout<<"DataFile_CRC  crc16_naive flen : " << flen <<endl;
        LUH.datafile[i].DataFile_CRC = crc16_naive(to_crc, to_crc + flen);

        printf("*******data file flen = %d, crc = %d.\n", flen, LUH.datafile[i].DataFile_CRC);

        offset += 1;
        fclose(fp);
        //crc校验结束

        LUH.datafile[i].DataFile_LenInBytes = flen;
        offset += 4;

        LUH.datafile[i].DataFile_CheckVal_Len = 0;
        //LUH.datafile[i].DataFile_CheckVal_Type = 0;
        offset += 1;

        LUH.datafile[i].DataFile_CheckVal = (char *) malloc(2);
        memset(LUH.datafile[i].DataFile_CheckVal, 0, 2);

        //offset += 2;

        if(i + 1 == LUH.DataFile_Num)LUH.datafile[i].Ptr_DataFile = 0;
        else LUH.datafile[i].Ptr_DataFile = offset;
    }

    LUH.SupFile_Num = 0;//支持文件不考虑
    LUH.Ptr_SupFile_Num = 0;//6666666666
    //忽略用户定义数据
    LUH.Ptr_UserDef_Data = 0;//7777777777


    LUH.Ptr_LoadCheckVal_Len = 0;
    LUH.LoadCheckVal_Len = 0;
    LUH.LoadCheckVal_Type = 0;

    //LUH.Ptr_LoadCheckVal_Len = offset;//888888
    LUH.LoadCheckVal = (char *) malloc (2);
    memset(LUH.LoadCheckVal, 0, 2);

    //big little swap
    if(BigOrLittel){
        LUH.LoadFile_Ver = BigLittleSwap16(LUH.LoadFile_Ver);
        LUH.Part_Flag = BigLittleSwap16(LUH.Part_Flag);
        LUH.Ptr_PN_Len = BigLittleSwap32(LUH.Ptr_PN_Len);
        LUH.Ptr_TargetID_Num = BigLittleSwap32(LUH.Ptr_TargetID_Num);
        LUH.Ptr_DataFile_Num = BigLittleSwap32(LUH.Ptr_DataFile_Num);
        LUH.Ptr_SupFile_Num = BigLittleSwap32(LUH.Ptr_SupFile_Num);
        LUH.Ptr_UserDef_Data = BigLittleSwap32(LUH.Ptr_UserDef_Data);
        LUH.Ptr_LoadTypeDes_Len = BigLittleSwap32(LUH.Ptr_LoadTypeDes_Len);
        LUH.Ptr_TargetIDPosition_Num = BigLittleSwap32(LUH.Ptr_TargetIDPosition_Num);
        LUH.Ptr_LoadCheckVal_Len = BigLittleSwap32(LUH.Ptr_LoadCheckVal_Len);
    }
    //头文件CRC写文件后再写入
    //写文件-----------------
    fwrite(&LUH, 40, 1, fp);

    if(LUH.Ptr_PN_Len){
        BigOrLittel ? LUH.Load_PN_LEN = BigLittleSwap16(LUH.Load_PN_LEN) : 1;
        fwrite(&LUH.Load_PN_LEN, 2, 1, fp);
        BigOrLittel ? LUH.Load_PN_LEN = BigLittleSwap16(LUH.Load_PN_LEN) : 1;
        fwrite(LUH.Load_PN, LUH.Load_PN_LEN, 1, fp);
    }

    if(LUH.Ptr_LoadTypeDes_Len){
        BigOrLittel ? LUH.LoadTypeDes_Len = BigLittleSwap16(LUH.LoadTypeDes_Len) : 1;
        fwrite(&LUH.LoadTypeDes_Len, 2, 1, fp);
        BigOrLittel ? LUH.LoadTypeDes_Len = BigLittleSwap16(LUH.LoadTypeDes_Len) : 1;
        fwrite(LUH.LoadTypeDes, LUH.LoadTypeDes_Len, 1, fp);

        BigOrLittel ? LUH.LoadTypeID = BigLittleSwap16(LUH.LoadTypeID) : 1;
        fwrite(&LUH.LoadTypeID, 2, 1, fp);
        BigOrLittel ? LUH.LoadTypeID = BigLittleSwap16(LUH.LoadTypeID) : 1;
    }

    if(LUH.Ptr_TargetID_Num){
        BigOrLittel ? LUH.TargetIDs_Num = BigLittleSwap16(LUH.TargetIDs_Num) : 1;
        fwrite(&LUH.TargetIDs_Num, 2, 1, fp);
        BigOrLittel ? LUH.TargetIDs_Num = BigLittleSwap16(LUH.TargetIDs_Num) : 1;
        for(int i = 0;i < LUH.TargetIDs_Num;++i){
            fwrite(&LUH.TargetID[i].TargetID_Num, 2, 1, fp);
            fwrite(LUH.TargetID[i].TargetID, LUH.TargetID[i].TargetID_Num, 1, fp);
            fwrite(LUH.TargetID[i].TargetID, LUH.TargetID[i].TargetID_Num, 1, fp);
        }
    }

    if(LUH.Ptr_TargetIDPosition_Num){
        BigOrLittel ? LUH.TargetIDPosition_Num = BigLittleSwap16(LUH.TargetIDPosition_Num) : 1;
        fwrite(&LUH.TargetIDPosition_Num, 2, 1, fp);
        BigOrLittel ? LUH.TargetIDPosition_Num = BigLittleSwap16(LUH.TargetIDPosition_Num) : 1;
        for(int i = 0;i < LUH.TargetIDPosition_Num;++i){
            fwrite(&LUH.TargetIDs_Pos[i].TargetID_Pos_Len, 2, 1, fp);
            fwrite(LUH.TargetIDs_Pos[i].TargetID_Pos, LUH.TargetIDs_Pos[i].TargetID_Pos_Len, 1, fp);
            fwrite(&LUH.TargetIDs_Pos[i].TargetID_Pos_Num, 2, 1, fp);
            for(int j = 0;j < LUH.TargetIDs_Pos[i].TargetID_Pos_Num;++j){
                fwrite(&LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len, 2, 1, fp);
                fwrite(LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos, LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len, 1, fp);
            }
        }
    }

    if(LUH.Ptr_DataFile_Num){
        BigOrLittel ? LUH.DataFile_Num = BigLittleSwap16(LUH.DataFile_Num) : 1;
        fwrite(&LUH.DataFile_Num, 2, 1, fp);
        BigOrLittel ? LUH.DataFile_Num = BigLittleSwap16(LUH.DataFile_Num) : 1;
        for(int i = 0;i < LUH.DataFile_Num;++i){
            BigOrLittel ? LUH.datafile[i].Ptr_DataFile = BigLittleSwap16(LUH.datafile[i].Ptr_DataFile) : 1;
            fwrite(&LUH.datafile[i].Ptr_DataFile, 2, 1, fp);
            BigOrLittel ? LUH.datafile[i].Ptr_DataFile = BigLittleSwap16(LUH.datafile[i].Ptr_DataFile) : 1;

            BigOrLittel ? LUH.datafile[i].DataFile_Name_Len = BigLittleSwap16(LUH.datafile[i].DataFile_Name_Len) : 1;
            fwrite(&LUH.datafile[i].DataFile_Name_Len, 2, 1, fp);
            BigOrLittel ? LUH.datafile[i].DataFile_Name_Len = BigLittleSwap16(LUH.datafile[i].DataFile_Name_Len) : 1;
            fwrite(LUH.datafile[i].DataFile_Name, LUH.datafile[i].DataFile_Name_Len, 1, fp);

            BigOrLittel ? LUH.datafile[i].DataFile_PN_Len = BigLittleSwap16(LUH.datafile[i].DataFile_PN_Len) : 1;
            fwrite(&LUH.datafile[i].DataFile_PN_Len, 2, 1, fp);
            BigOrLittel ? LUH.datafile[i].DataFile_PN_Len = BigLittleSwap16(LUH.datafile[i].DataFile_PN_Len) : 1;
            fwrite(LUH.datafile[i].DataFile_PN, LUH.datafile[i].DataFile_PN_Len, 1, fp);

            BigOrLittel ? LUH.datafile[i].DataFile_Len = BigLittleSwap32(LUH.datafile[i].DataFile_Len) : 1;
            fwrite(&LUH.datafile[i].DataFile_Len, 4, 1, fp);
            BigOrLittel ? LUH.datafile[i].DataFile_Len = BigLittleSwap32(LUH.datafile[i].DataFile_Len) : 1;

            BigOrLittel ? LUH.datafile[i].DataFile_CRC = BigLittleSwap16(LUH.datafile[i].DataFile_CRC) : 1;
            fwrite(&LUH.datafile[i].DataFile_CRC, 2, 1, fp);
            BigOrLittel ? LUH.datafile[i].DataFile_CRC = BigLittleSwap16(LUH.datafile[i].DataFile_CRC) : 1;

            BigOrLittel ? LUH.datafile[i].DataFile_LenInBytes = BigLittleSwap64(LUH.datafile[i].DataFile_LenInBytes) : 1;
            fwrite(&LUH.datafile[i].DataFile_LenInBytes, 8, 1, fp);
            BigOrLittel ? LUH.datafile[i].DataFile_LenInBytes = BigLittleSwap64(LUH.datafile[i].DataFile_LenInBytes) : 1;

            BigOrLittel ? LUH.datafile[i].DataFile_CheckVal_Len = BigLittleSwap16(LUH.datafile[i].DataFile_CheckVal_Len) : 1;
            fwrite(&LUH.datafile[i].DataFile_CheckVal_Len, 2, 1, fp);
            BigOrLittel ? LUH.datafile[i].DataFile_CheckVal_Len = BigLittleSwap16(LUH.datafile[i].DataFile_CheckVal_Len) : 1;

            if(LUH.datafile[i].DataFile_CheckVal_Len){
                BigOrLittel ? LUH.datafile[i].DataFile_CheckVal_Type = BigLittleSwap16(LUH.datafile[i].DataFile_CheckVal_Type) : 1;
                fwrite(&LUH.datafile[i].DataFile_CheckVal_Type, 2, 1, fp);
                BigOrLittel ? LUH.datafile[i].DataFile_CheckVal_Type = BigLittleSwap16(LUH.datafile[i].DataFile_CheckVal_Type) : 1;

                fwrite(LUH.datafile[i].DataFile_CheckVal, 2, 1, fp);
            }
        }
    }

    if(LUH.Ptr_SupFile_Num){
        fwrite(&LUH.SupFile_Num, 2, 1, fp);
    }

    if(LUH.Ptr_UserDef_Data){

    }

    //if(LUH.Ptr_LoadCheckVal_Len){
        BigOrLittel ? LUH.LoadCheckVal_Len = BigLittleSwap16(LUH.LoadCheckVal_Len) : 1;
        fwrite(&LUH.LoadCheckVal_Len, 2, 1, fp);
        BigOrLittel ? LUH.LoadCheckVal_Len = BigLittleSwap16(LUH.LoadCheckVal_Len) : 1;

        if(LUH.LoadCheckVal_Len){
            fwrite(&LUH.LoadCheckVal_Type, 2, 1, fp);
            fwrite(LUH.LoadCheckVal, 2, 1, fp);
        }
    //}

    // 读 已写文件部分进行CRC

    uint32 flen;
    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
    flen=ftell(fp); /* 得到文件大小 */

    fseek(fp,0L,SEEK_SET); /* 定位到文件开头 */
    LUH.file_len = (flen + 6) / 2;
    BigOrLittel ? LUH.file_len = BigLittleSwap32(LUH.file_len) : 1;
    fwrite(&LUH, 4, 1, fp);
    BigOrLittel ? LUH.file_len = BigLittleSwap32(LUH.file_len) : 1;
    fseek(fp,0L,SEEK_SET);
    unsigned char * to_crc = (unsigned char * ) malloc(flen);
    fread(to_crc, flen, 1, fp);

    LUH.HFile_CRC = crc16_naive(to_crc, to_crc + flen);
    LUH.Load_CRC = LUH.HFile_CRC;
    //cout<<"HFile_CRC  crc16_naive flen : " << flen <<endl;

    BigOrLittel ? LUH.HFile_CRC = BigLittleSwap16(LUH.HFile_CRC) : 1;
    BigOrLittel ? LUH.Load_CRC = BigLittleSwap32(LUH.Load_CRC) : 1;

    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
    fwrite(&LUH.HFile_CRC, 2, 1, fp);
    fwrite(&LUH.Load_CRC, 4, 1, fp);

    BigOrLittel ? LUH.HFile_CRC = BigLittleSwap16(LUH.HFile_CRC) : 1;
    BigOrLittel ? LUH.Load_CRC = BigLittleSwap32(LUH.Load_CRC) : 1;
    
    fclose(fp);

    //debug out
    //big little swap
    if(BigOrLittel){
        LUH.LoadFile_Ver = BigLittleSwap16(LUH.LoadFile_Ver);
        LUH.Part_Flag = BigLittleSwap16(LUH.Part_Flag);
        LUH.Ptr_PN_Len = BigLittleSwap32(LUH.Ptr_PN_Len);
        LUH.Ptr_TargetID_Num = BigLittleSwap32(LUH.Ptr_TargetID_Num);
        LUH.Ptr_DataFile_Num = BigLittleSwap32(LUH.Ptr_DataFile_Num);
        LUH.Ptr_SupFile_Num = BigLittleSwap32(LUH.Ptr_SupFile_Num);
        LUH.Ptr_UserDef_Data = BigLittleSwap16(LUH.Ptr_UserDef_Data);
        LUH.Ptr_LoadTypeDes_Len = BigLittleSwap32(LUH.Ptr_LoadTypeDes_Len);
        LUH.Ptr_TargetIDPosition_Num = BigLittleSwap32(LUH.Ptr_TargetIDPosition_Num);
        LUH.Ptr_LoadCheckVal_Len = BigLittleSwap32(LUH.Ptr_LoadCheckVal_Len);
        LUH.LoadCheckVal_Type = BigLittleSwap16(LUH.LoadCheckVal_Type);
    }
    if(1){
        //cout<<"LUH.file_len : " << LUH.file_len <<endl;
        //cout<<"LUH.LoadFile_Ver : " << LUH.LoadFile_Ver <<endl;
        //cout<<"LUH.Part_Flag : " << LUH.Part_Flag <<endl;

        //cout<<"LUH.Ptr_PN_Len : " << LUH.Ptr_PN_Len <<endl;
        //cout<<"LUH.Ptr_TargetID_Num : " << LUH.Ptr_TargetID_Num <<endl;
        //cout<<"LUH.Ptr_DataFile_Num : " << LUH.Ptr_DataFile_Num <<endl;
        //cout<<"LUH.Ptr_SupFile_Num : " << LUH.Ptr_SupFile_Num <<endl;
        //cout<<"LUH.Ptr_UserDef_Data : " << LUH.Ptr_UserDef_Data <<endl;
        //cout<<"LUH.Ptr_LoadTypeDes_Len : " << LUH.Ptr_LoadTypeDes_Len <<endl;
        //cout<<"LUH.Ptr_TargetIDPosition_Num : " << LUH.Ptr_TargetIDPosition_Num <<endl;
        //cout<<"LUH.Ptr_LoadCheckVal_Len : " << LUH.Ptr_LoadCheckVal_Len <<endl;

        //cout<<"LUH.Load_PN_LEN : " << LUH.Load_PN_LEN <<endl;
        //cout<<"LUH.Load_PN : " << *LUH.Load_PN <<endl;
        //cout<<"LUH.LoadTypeDes_Len : " << LUH.LoadTypeDes_Len <<endl;
        //cout<<"LUH.LoadTypeDes : " << *LUH.LoadTypeDes <<endl;
        //cout<<"LUH.LoadTypeID : " << LUH.LoadTypeID <<endl;

        //cout<<"LUH.TargetIDs_Num : " << LUH.TargetIDs_Num <<endl;
        for(int i = 0;i < LUH.TargetIDs_Num;++i){
            //cout<<"LUH.TargetID[i].TargetID_Num : " << LUH.TargetID[i].TargetID_Num <<endl;
            //cout<<"LUH.TargetID[0].TargetID : " << (char *)LUH.TargetID[i].TargetID <<endl;
            //cout<<"LUH.TargetID[1].TargetID : " << (char *)(LUH.TargetID[i].TargetID + 1) <<endl;
        }

        //cout<<"LUH.DataFile_Num : " << LUH.DataFile_Num <<endl;
        for(int i = 0;i < LUH.DataFile_Num;++i){
            //cout<<"LUH.datafile[i].Ptr_DataFile : " << LUH.datafile[i].Ptr_DataFile <<endl;

            //cout<<"LUH.datafile[i].DataFile_Name_Len : " << LUH.datafile[i].DataFile_Name_Len <<endl;
            //cout<<"LUH.datafile[i].DataFile_Name : " << (char *)LUH.datafile[i].DataFile_Name <<endl;
            //cout<<"LUH.datafile[i].DataFile_PN_Len : " << LUH.datafile[i].DataFile_PN_Len <<endl;
            //cout<<"LUH.datafile[i].DataFile_PN : " << (char *)LUH.datafile[i].DataFile_PN <<endl;
            //cout<<"LUH.datafile[i].DataFile_Len : " << LUH.datafile[i].DataFile_Len <<endl;

            //BigOrLittel ? LUH.datafile[i].DataFile_CRC = BigLittleSwap16(LUH.datafile[i].DataFile_CRC) : 1;
            //cout<<"LUH.datafile[i].DataFile_CRC : " << LUH.datafile[i].DataFile_CRC <<endl;
            //BigOrLittel ? LUH.datafile[i].DataFile_CRC = BigLittleSwap16(LUH.datafile[i].DataFile_CRC) : 1;

            //cout<<"LUH.datafile[i].DataFile_LenInBytes : " << LUH.datafile[i].DataFile_LenInBytes <<endl;
            //cout<<"LUH.datafile[i].DataFile_LenInBytes---len : " << sizeof(LUH.datafile[i].DataFile_LenInBytes) <<endl;

            BigOrLittel ? LUH.datafile[i].DataFile_LenInBytes = BigLittleSwap64(LUH.datafile[i].DataFile_LenInBytes) : 1;
            //cout<<"LUH.datafile[i].DataFile_LenInBytes : " << LUH.datafile[i].DataFile_LenInBytes <<endl;
            BigOrLittel ? LUH.datafile[i].DataFile_LenInBytes = BigLittleSwap64(LUH.datafile[i].DataFile_LenInBytes) : 1;

            //cout<<"LUH.datafile[i].DataFile_CheckVal_Len : " << LUH.datafile[i].DataFile_CheckVal_Len <<endl;
            if(LUH.datafile[i].DataFile_CheckVal_Len){
                //cout<<"LUH.datafile[i].DataFile_CheckVal_Type : " << LUH.datafile[i].DataFile_CheckVal_Type <<endl;
                //cout<<"LUH.datafile[i].DataFile_CheckVal : " << *LUH.datafile[i].DataFile_CheckVal <<endl;
            }
        }
        if(LUH.Ptr_SupFile_Num){
            //cout<<"LUH.SupFile_Num : " << LUH.SupFile_Num <<endl;
        }

        //cout<<"LUH.LoadCheckVal_Len : " << LUH.LoadCheckVal_Len <<endl;
        if(LUH.Ptr_LoadCheckVal_Len){
            //cout<<"LUH.LoadCheckVal_Type : " << LUH.LoadCheckVal_Type <<endl;
        }
        //cout<<"LUH.HFile_CRC : " << LUH.HFile_CRC <<endl;
        //cout<<"LUH.Load_CRC : " << LUH.Load_CRC <<endl;
    }

    return LUH;
}

File_LCI parsing_LCI(char filename[])
{
    FILE *fp;
    char file_LCI[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LCI,"../Get/%s%s",filename,".LCI");

    struct File_LCI LCI;
    memset(&LCI,0,sizeof (struct File_LCI));

    LCI.file_len = 0;
    if((fp = fopen(file_LCI,"rb"))==NULL)
    {
        //printf("Failed to read LCI: %s\n", file_LCI);
        LCI.file_len = -1;
        return LCI;
    }

    //    struct File_LCI{
    //        uint32 file_len;//文件大小
    //        uint16 Pro_ver;//协议版本号
    //        uint16 status_flag;//状态标志码
    //        unsigned char stat_des_len;//状态描述长度
    //        char stat_des[255];//状态描述
    //    };

    fread(&LCI,8,1,fp);

    BigOrLittel ? LCI.file_len = BigLittleSwap32(LCI.file_len) : 1;
    BigOrLittel ? LCI.Pro_ver = BigLittleSwap16(LCI.Pro_ver) : 1;
    BigOrLittel ? LCI.status_flag = BigLittleSwap16(LCI.status_flag) : 1;

    fread(&LCI.stat_des_len, 1, 1, fp);
    int temp = (int)LCI.stat_des_len;
    fread(&LCI.stat_des, temp, 1, fp);
    fclose(fp);
    return LCI;
}

File_LUI parsing_LUI(char filename[])
{
    FILE *fp;
    char file_LUI[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUI,"../Get/%s%s",filename,".LUI");

    struct File_LUI LUI;
    memset(&LUI,0,sizeof (struct File_LUI));

    LUI.file_len = 0;
    if((fp = fopen(file_LUI,"rb"))==NULL)
    {
        printf("Failed to read LNS\n");
        LUI.file_len = -1;
        return LUI;
    }

    //    struct File_LUI{
    //        uint32 file_len;//文件大小
    //        uint16 Pro_ver;//协议版本号
    //        uint16 status_flag;//状态标志码
    //        unsigned char stat_des_len;//状态描述长度
    //        char stat_des[255];//状态描述
    //    };

    fread(&LUI,8,1,fp);
    BigOrLittel ? LUI.file_len = BigLittleSwap32(LUI.file_len) : 1;
    BigOrLittel ? LUI.Pro_ver = BigLittleSwap16(LUI.Pro_ver) : 1;
    BigOrLittel ? LUI.status_flag = BigLittleSwap16(LUI.status_flag) : 1;
    fread(&LUI.stat_des_len, 1, 1, fp);
    int temp = (int)LUI.stat_des_len;

    fread(&LUI.stat_des, temp, 1, fp);

    fclose(fp);
    return LUI;
}

File_LND parsing_LND(char filename[])
{
    FILE *fp;
    char file_LND[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LND,"../Get/%s%s",filename,".LND");

    struct File_LND LND;
    memset(&LND,0,sizeof (struct File_LND));

    LND.file_len = 0;
    if((fp = fopen(file_LND,"rb"))==NULL)
    {
        //printf("Failed to read LNS\n");
        LND.file_len = -1;
        return LND;
    }

    //    struct File_LND{
    //        uint32 file_len;//文件大小
    //        uint16 Pro_ver;//协议版本号
    //        uint16 status_flag;//状态标志码
    //        unsigned char stat_des_len;//状态描述长度
    //        char stat_des[255];//状态描述
    //    };

    fread(&LND,8,1,fp);
    BigOrLittel ? LND.file_len = BigLittleSwap32(LND.file_len) : 1;
    BigOrLittel ? LND.Pro_ver = BigLittleSwap16(LND.Pro_ver) : 1;
    BigOrLittel ? LND.status_flag = BigLittleSwap16(LND.status_flag) : 1;

    fread(&LND.stat_des_len, 1, 1, fp);
    int temp = (int)LND.stat_des_len;

    fread(&LND.stat_des, temp, 1, fp);

    fclose(fp);
    return LND;
}

File_LNO parsing_LNO(char filename[])
{
    FILE *fp;
    char file_LNO[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LNO,"../Get/%s%s",filename,".LNO");

    struct File_LNO LNO;
    memset(&LNO,0,sizeof (struct File_LNO));

    LNO.file_len = 0;
    if((fp = fopen(file_LNO,"rb"))==NULL)
    {
        //printf("Failed to read LNS\n");
        LNO.file_len = -1;
        return LNO;
    }

    //    struct File_LNO{
    //        uint32 file_len;//文件大小
    //        uint16 Pro_ver;//协议版本号
    //        uint16 status_flag;//状态标志码
    //        unsigned char stat_des_len;//状态描述长度
    //        char stat_des[255];//状态描述
    //    };

    fread(&LNO,8,1,fp);
    BigOrLittel ? LNO.file_len = BigLittleSwap32(LNO.file_len) : 1;
    BigOrLittel ? LNO.Pro_ver = BigLittleSwap16(LNO.Pro_ver) : 1;
    BigOrLittel ? LNO.status_flag = BigLittleSwap16(LNO.status_flag) : 1;

    fread(&LNO.stat_des_len, 1, 1, fp);
    int temp = (int)LNO.stat_des_len;

    fread(&LNO.stat_des, temp, 1, fp);

    fclose(fp);
    return LNO;
}

File_LUH parsing_LUH(char filename[])
{
    File_LUH LUH;
    memset(&LUH, 0, sizeof (LUH));
    FILE *fp;
    char file_data[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_data,"../target-file/Get/%s",filename);
    if((fp = fopen(file_data,"rb"))==NULL)
    {
        //printf("Failed to read datafile: %s\n", filename);
        return LUH;
    }

    fread(&LUH, 40 ,1 ,fp);
    fread(&LUH.Load_PN_LEN, 2 ,1 ,fp);
    LUH.Load_PN = (char *) malloc(LUH.Load_PN_LEN);
    memset(LUH.Load_PN, 0, LUH.Load_PN_LEN);
    fread(LUH.Load_PN, LUH.Load_PN_LEN,1 ,fp);

    if(LUH.Ptr_LoadTypeDes_Len){
        fread(&LUH.LoadTypeDes_Len, 2 ,1 ,fp);
        LUH.LoadTypeDes = (char *) malloc (LUH.LoadTypeDes_Len);
        fread(LUH.LoadTypeDes, LUH.LoadTypeDes_Len,1 ,fp);
        fread(&LUH.LoadTypeID, 2 ,1 ,fp);
    }

    fread(&LUH.TargetIDs_Num, 2 ,1 ,fp);
    LUH.TargetID = (TargetIDs *) malloc(sizeof (TargetIDs) * LUH.TargetIDs_Num);
    for(int i = 0;i < LUH.TargetIDs_Num;++i){
        fread(&LUH.TargetID[i].TargetID_Num, 2, 1, fp);
        LUH.TargetID[i].TargetID = (char *) malloc (LUH.TargetID[i].TargetID_Num);
        memset(LUH.TargetID[i].TargetID, 0, LUH.TargetID[i].TargetID_Num);
        fread(LUH.TargetID[i].TargetID, LUH.TargetID[i].TargetID_Num, 1, fp);
    }

    if(LUH.Ptr_TargetIDPosition_Num){
        fread(&LUH.TargetIDPosition_Num, 2 ,1 ,fp);
        LUH.TargetIDs_Pos = (TargetIDs_Pos *) malloc(sizeof (TargetIDs_Pos) * LUH.TargetIDPosition_Num);
        for(int i = 0;i < LUH.TargetIDPosition_Num;++i){
            fread(&LUH.TargetIDs_Pos[i].TargetID_Pos_Len, 2, 1, fp);
            LUH.TargetIDs_Pos[i].TargetID_Pos = (char *) malloc (LUH.TargetIDs_Pos[i].TargetID_Pos_Len);
            memset(LUH.TargetIDs_Pos[i].TargetID_Pos, 0, LUH.TargetIDs_Pos[i].TargetID_Pos_Len);
            fread(LUH.TargetIDs_Pos[i].TargetID_Pos, LUH.TargetIDs_Pos[i].TargetID_Pos_Len, 1, fp);

            fread(&LUH.TargetIDs_Pos[i].TargetID_Pos_Num, 2, 1, fp);
            LUH.TargetIDs_Pos[i].Tid_Pos = (TIDs_Pos *) malloc(sizeof(TIDs_Pos) * LUH.TargetIDs_Pos[i].TargetID_Pos_Num);
            for(int j = 0;j < LUH.TargetIDs_Pos[i].TargetID_Pos_Num;++j){
                fread(&LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len, 2, 1, fp);
                LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos = (char *)
                        malloc (LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len);
                memset(&LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos,
                       0, LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len);
                fread(&LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos, LUH.TargetIDs_Pos[i].Tid_Pos[j].Pos_Len, 1, fp);
            }
        }
    }

    fread(&LUH.DataFile_Num, 2 ,1 ,fp);
    LUH.datafile = (DataFile* ) malloc(sizeof (DataFile) * LUH.DataFile_Num);
    memset(LUH.datafile,0, LUH.DataFile_Num);
    for(int i = 0;i < LUH.DataFile_Num;++i){
        fread(&LUH.datafile[i].Ptr_DataFile, 2, 1, fp);

        fread(&LUH.datafile[i].DataFile_Name_Len, 2, 1, fp);
        LUH.datafile[i].DataFile_Name = (char *)malloc (LUH.datafile[i].DataFile_Name_Len);
        memset(LUH.datafile[i].DataFile_Name,0, LUH.datafile[i].DataFile_Name_Len);
        fread(LUH.datafile[i].DataFile_Name, LUH.datafile[i].DataFile_Name_Len, 1, fp);

        fread(&LUH.datafile[i].DataFile_PN_Len, 2, 1, fp);
        LUH.datafile[i].DataFile_PN = (char *)malloc (LUH.datafile[i].DataFile_PN_Len);
        memset(LUH.datafile[i].DataFile_PN,0, LUH.datafile[i].DataFile_PN_Len);
        fread(LUH.datafile[i].DataFile_PN, LUH.datafile[i].DataFile_PN_Len, 1, fp);

        fread(&LUH.datafile[i].DataFile_Len, 4, 1, fp);
        fread(&LUH.datafile[i].DataFile_CRC, 2, 1, fp);
        fread(&LUH.datafile[i].DataFile_LenInBytes, 8, 1, fp);
        fread(&LUH.datafile[i].DataFile_CheckVal_Len, 2, 1, fp);
        fread(&LUH.datafile[i].DataFile_CheckVal_Type, 2, 1, fp);
        LUH.datafile[i].DataFile_CheckVal = (char *) malloc(2);
        memset(LUH.datafile[i].DataFile_CheckVal, 0, 2);
        fread(LUH.datafile[i].DataFile_CheckVal, 2, 1, fp);
    }

    fread(&LUH.SupFile_Num, 2, 1, fp);
    fread(&LUH.LoadCheckVal_Len, 2, 1, fp);
    fread(&LUH.LoadCheckVal_Type, 2, 1, fp);
    LUH.LoadCheckVal = (char *) malloc(2);
    memset(LUH.LoadCheckVal, 0, 2);
    fread(LUH.LoadCheckVal, 2, 1, fp);

    fread(&LUH.HFile_CRC, 2, 1, fp);
    fread(&LUH.Load_CRC, 4, 1, fp);

    //debug
    if(0){
        printf("LUH.file_len : %lu\n" , LUH.file_len);
        printf("LUH.LoadFile_Ver : %u\n", LUH.LoadFile_Ver);
        printf("LUH.Part_Flag : %u\n", LUH.Part_Flag);

        printf("LUH.Ptr_PN_Len : %lu\n", LUH.Ptr_PN_Len);
        printf("LUH.Ptr_TargetID_Num : %lu\n", LUH.Ptr_TargetID_Num);
        printf("LUH.Ptr_DataFile_Num : %lu\n", LUH.Ptr_DataFile_Num);
        printf("LUH.Ptr_SupFile_Num : %lu\n", LUH.Ptr_SupFile_Num);
        printf("LUH.Ptr_UserDef_Data : %lu\n", LUH.Ptr_UserDef_Data);
        printf("LUH.Ptr_LoadTypeDes_Len : %lu\n", LUH.Ptr_LoadTypeDes_Len);
        printf("LUH.Ptr_TargetIDPosition_Num : %lu\n", LUH.Ptr_TargetIDPosition_Num);
        printf("LUH.Ptr_LoadCheckVal_Len : %lu\n", LUH.Ptr_LoadCheckVal_Len);

        printf("LUH.Load_PN_LEN : %u\n", LUH.Load_PN_LEN);
        printf("LUH.Load_PN : %u\n", *LUH.Load_PN);
        printf("LUH.LoadTypeDes_Len : %u\n", LUH.LoadTypeDes_Len);
        printf("LUH.LoadTypeDes : %u\n", *LUH.LoadTypeDes);
        printf("LUH.LoadTypeID : %u\n", LUH.LoadTypeID);

        printf("LUH.TargetIDs_Num : %u \n", LUH.TargetIDs_Num);
        for(int i = 0;i < LUH.TargetIDs_Num;++i){
            printf("LUH.TargetID[i].TargetID_Num : %u \n", LUH.TargetID[i].TargetID_Num);
            printf("LUH.TargetID[0].TargetID : %s \n", (char *)LUH.TargetID[i].TargetID);
            printf("LUH.TargetID[1].TargetID : %s \n", (char *)(LUH.TargetID[i].TargetID + 1));
        }

        printf("LUH.DataFile_Num : %u \n", LUH.DataFile_Num);
        for(int i = 0;i < LUH.DataFile_Num;++i){
            printf("LUH.datafile[%d].Ptr_DataFile : %u \n", i, LUH.datafile[i].Ptr_DataFile);

            printf("LUH.datafile[%d].DataFile_Name_Len : %u \n", i, LUH.datafile[i].DataFile_Name_Len);
            printf("LUH.datafile[%d].DataFile_Name : %s \n", i, (char *)LUH.datafile[i].DataFile_Name);
            printf("LUH.datafile[%d].DataFile_PN_Len : %u \n", i, LUH.datafile[i].DataFile_PN_Len);
            printf("LUH.datafile[%d].DataFile_PN : %s \n", i, (char *)LUH.datafile[i].DataFile_PN);
            printf("LUH.datafile[%d].DataFile_Len : %lu \n", i, LUH.datafile[i].DataFile_Len);
            printf("LUH.datafile[%d].DataFile_CRC : %u \n", i, LUH.datafile[i].DataFile_CRC);
            printf("LUH.datafile[%d].DataFile_LenInBytes : %llu \n", i, LUH.datafile[i].DataFile_LenInBytes);
            printf("LUH.datafile[%d].DataFile_CheckVal_Len : %u \n", i, LUH.datafile[i].DataFile_CheckVal_Len);
            printf("LUH.datafile[%d].DataFile_CheckVal_Type : %u \n", i, LUH.datafile[i].DataFile_CheckVal_Type);
            printf("LUH.datafile[%d].DataFile_CheckVal : %u \n", i, *LUH.datafile[i].DataFile_CheckVal);

        }
        printf("LUH.SupFile_Num : %u \n", LUH.SupFile_Num);
        printf("LUH.LoadCheckVal_Len : %u \n", LUH.LoadCheckVal_Len);
        printf("LUH.LoadCheckVal_Type : %u \n", LUH.LoadCheckVal_Type);
        printf("LUH.LoadCheckVal_Type : %u \n", *LUH.LoadCheckVal);
        printf("LUH.HFile_CRC : %u \n", LUH.HFile_CRC);
        printf("LUH.Load_CRC : %lu \n", LUH.Load_CRC);
    }
    return LUH;
}

//参数：(操作类型，操作阶段确定文件名)，Get/Send，信息头，信息内容
/*
void DebugOut(int optype, int opnum, int GetOrSend, char* inforHead, char *infor, bool status, char* source, char* dest)
{
    FILE *fp;//文件指针
    char file_debug[256] = {0};//文件名
    char file_debug_date[256] = {0};//文件夹名
    //std::cout<<"debug out"<<std::endl;
    //日期
    time_t now;
    struct tm *curTime;
    char date[256] = {0};
    now = time(NULL);
    curTime = localtime(&now);
    sprintf(date,"%04d-%02d-%02d %02dH",curTime->tm_year+1900,curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour);

    //按照日期新建文件夹
    sprintf(file_debug_date,"../ClientFile/Debug/%s",date);
    if(_mkdir(file_debug_date) == -1){
        printf("新建debug文件夹：%s\n",file_debug_date);
        //std::cout<<file_debug_date<<std::endl;
    }else{
        printf("已有debug文件夹：%s\n",file_debug_date);
    }

    //getorsend参数处理
    char getsend[8] = {0};
    if(GetOrSend){
        strcpy(getsend, "send");
    }else{
        strcpy(getsend, "get");
    }
    //status参数处理
    char debugstatus[8] = {0};
    if(status){
        strcpy(debugstatus, "Success");
    }else{
        strcpy(debugstatus, "Failure");
    }

    //debug信息处理
    switch(optype){
    case 1://find
        sprintf(file_debug,"%s/%s%s", file_debug_date, "Debug_Find", ".txt");
        std::cout<<file_debug<<std::endl;
        if((fp=fopen(file_debug,"a+")) == NULL){//初始化文件指针
            return ;
        }

        sprintf(date,"%02d:%02d:%02d", curTime->tm_hour, curTime->tm_min, curTime->tm_sec);
        fprintf(fp,"(%s)%s : (%s) %s \n\t(%s -> %s): %s\n\t%s\n\n",
                debugstatus, date, "Find", getsend, source, dest, inforHead, infor);
        break;

    case 2://information,inforHead参数确定协议文件类型,infor为文件名
        sprintf(file_debug,"%s/%s%s", file_debug_date, "Debug_Information", ".txt");
        std::cout<<"file_debug : "<<file_debug<<std::endl;
        if((fp=fopen(file_debug,"a+")) == NULL){//初始化文件指针
            return ;
        }
        sprintf(date,"%02d:%02d:%02d", curTime->tm_hour, curTime->tm_min, curTime->tm_sec);

        if(NULL != strstr(inforHead,"LCI")){//LCI文件
            File_LCI lci = parsing_LCI(infor);
            fprintf(fp,"\n(%s)%s : (%s) %s \n\t(%s -> %s): %s\n\t",
                    debugstatus, date, "Information", getsend, source, dest, inforHead);

            Debug_LCI(fp, lci);

        }
        if(NULL != strstr(inforHead,"LCL")){//LCL文件
            File_LCL lcl = parsing_LCL(infor);
            fprintf(fp,"\n(%s)%s : (%s) %s \n\t(%s -> %s): %s\n\t",
                    debugstatus, date, "Information", getsend, source, dest, inforHead);

            Debug_LCL(fp, lcl);
        }
        break;

    case 3://upload
        sprintf(file_debug,"%s/%s%s", file_debug_date, "Debug_Upload", ".txt");
        std::cout<<"file_debug : "<<file_debug<<std::endl;
        if((fp=fopen(file_debug,"a+")) == NULL){//初始化文件指针
            return ;
        }
        sprintf(date,"%02d:%02d:%02d", curTime->tm_hour, curTime->tm_min, curTime->tm_sec);

        if(NULL != strstr(inforHead,"LUI")){//LUI文件
            File_LUI lui = parsing_LUI(infor);
            fprintf(fp,"\n(%s)%s : (%s) %s \n\t(%s -> %s): %s\n\t",
                    debugstatus, date, "Upload", getsend, source, dest, inforHead);

            Debug_LUI(fp, lui);
        }
        if(NULL != strstr(inforHead,"LUR")){//LUR文件
            File_LUR lur = parsing_LUR(infor);
            fprintf(fp,"\n(%s)%s : (%s) %s \n\t(%s -> %s): %s\n\t",
                    debugstatus, date, "Upload", getsend, source, dest, inforHead);

            Debug_LUR(fp, lur);
        }
        if(NULL != strstr(inforHead,"LUH")){//LUH文件
            File_LUH luh = parsing_LUH(infor);
            fprintf(fp,"\n(%s)%s : (%s) %s \n\t(%s -> %s): %s\n\t",
                    debugstatus, date, "Upload", getsend, source, dest, inforHead);

            Debug_LUH(fp, luh);
        }

        break;
    case 4://mediadownload
        break;
    case 5://operatordownload
        break;

    }


    fclose(fp);
}

*/

char *File_to_string(char *filename)
{
    return ret;
}
