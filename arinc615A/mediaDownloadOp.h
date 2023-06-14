#ifndef MEDIA_DOWNLOAD_H
#define MEDIA_DOWNLOAD_H
#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#include "arinc615_global_var_define.h"

bool_t mediaDownloadInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

bool_t mediaDownloadList(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

void mediaDownloadFile(void *a);
#ifdef  __cplusplus
}
#endif

#endif //MEDIA_DOWNLOAD_H


