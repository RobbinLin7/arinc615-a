#ifndef UPLOAD_OP_H
#define UPLOAD_OP_H

#include <process.h>

#include "arinc615_global_var_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

typedef void* HANDLE;

//extern int backflag;

extern HANDLE mutex;

bool_t uploadInit(char *filename, char * dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

bool_t uploadList(char *filename, char * dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

bool_t uploadFileHeader(int16_t* err_code, uint8_t* err_msg);

void uploadFileData(void* a);

#ifdef  __cplusplus
}
#endif

#endif

