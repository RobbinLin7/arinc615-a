#ifndef INFORMATION_OP_H
#define INFORMATION_OP_H
#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "arinc615_global_var_define.h"

bool_t informationInit(char *filename, char *dest_ip_addr, int16_t* err_code, uint8_t* err_msg);

bool_t informationFile(int16_t* err_code, uint8_t* err_msg);

#ifdef  __cplusplus
}
#endif
#endif //INFORMATION_OP_H


