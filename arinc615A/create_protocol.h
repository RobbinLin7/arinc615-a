#ifndef CREATE_PROTOCOL_H
#define CREATE_PROTOCOL_H

#include"file_define.h"
#include "arinc615_global_var_define.h"
#include "arinc615_struct_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

extern int BigOrLittel;
#define Protocol_ver (0x4134U)
#define LOADFILE_VER (0x8004)

#define TEST_2021_8_12 0
typedef struct{
    char path[255];
    char name[255];
    unsigned long file_len;
}OneFileOld;

typedef struct{
    int lines;
    OneFile oneFile[50];
}FileListOld;


File_LCI parsing_LCI(char filename[255]);
File_LUI parsing_LUI(char filename[255]);
File_LND parsing_LND(char filename[255]);
File_LNO parsing_LNO(char filename[255]);

FILE_LCL parsing_LCL(char *filename);
struct File_LUR parsing_LUR(char *filename);
struct File_LUS parsing_LUS(char *filename);
int parsing_LNS(char *filename, File_LNS *LNS);
struct File_LUR create_LUR(char filename[255],int header_num,FileList filelist);
int create_LUR_new(char filename[255],int header_num,FileList filelist);
int create_LNR(char filename[255],int header_num,FileList filelist);
struct File_LNL parsing_LNL(char filename[255]);
struct File_LNA create_LNA(char filename[255],int header_num,FileList filelist);

struct File_LUH create_LUH(char filename[],
                           uint16 Load_PN_LEN,
                           uint16 *Load_PN,
                           uint16 LoadTypeDes_Len,
                           uint16 *LoadTypeDes,
                           uint16 LoadTypeID,
                           FileList filelist);
struct File_LUH parsingLuh(char filename[]);
File_LUH parsing_LUH(char filename[]);
char *File_to_string(char *filename[]);

#ifdef  __cplusplus
}
#endif

#endif // CREATE_PROTOCOL_H
