#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once


#define TRANSMIT_MODE		"octet"				//传输模式为octet

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
// converts block number to length-2 string
void s_to_i(char *f, int n);
//char* make_rrq(char *filename);
//char* make_wrq(char *filename);
char* make_data_pack(short block, char *data, int dataLen);
int make_ack(short block, char *packet);
char* make_err(short errcode, const char* errmsg, int *errLen);

int setWRQ_RRQ(char rqType, char* filename, int filenameLen,  char* requestPackage);


//int ip_uint2string(unsigned int ip_value, char *ip_str);

#define RE_TRANS_TIMES 3

#ifdef  __cplusplus
}
#endif
#endif // UTILITY_H
