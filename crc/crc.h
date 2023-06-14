#ifndef CRC_H
#define CRC_H
#include<string.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define INIT_VALUE_8 0x0//按照具体需求更改
#define FINAL_VALUE_8 0x0

#define CRC16_INIT_VAL		0x0000

#define INIT_VALUE_16 0xFFFF//按照具体需求更改
#define FINAL_VALUE_16 0x0

#define INIT_VALUE_32 0xFFFFFFFF//按照具体需求更改
#define FINAL_VALUE_32 0xFFFFFFFF

uint8_t crc8_naive(uint8_t *p_buf, uint8_t *p_end);
uint16_t crc16_naive(uint8_t *p_buf, uint8_t *p_end);
uint32_t crc32_naive(uint8_t *p_buf, uint8_t *p_end);

uint16_t crc16_table(char* buf, size_t len, unsigned short initVal);
unsigned int CRC16_Table(char *pcrc, size_t count);

#ifdef  __cplusplus
}
#endif

#endif // CRC_H
