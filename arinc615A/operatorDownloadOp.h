#ifndef OPERATOR_DOWNLOAD_H
#define OPERATOR_DOWNLOAD_H
#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "arinc615_global_var_define.h"

bool_t oDownloadInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

bool_t oDownloadList(char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

bool_t transLNAFile(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

void oDownloadFile(void *a);



#ifdef  __cplusplus
}
#endif
#endif //OPERATOR_DOWNLOAD_H


