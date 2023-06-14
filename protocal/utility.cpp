#include "utility.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <io.h>
#include <malloc.h>
//#include <process.h>


void s_to_i(char *f, int n)
{
    if(n==0){
        f[0] = '0', f[1] = '0', f[2] = '\0';
    } else if(n%10 > 0 && n/10 == 0){
        char c = n+'0';
        f[0] = '0', f[1] = c, f[2] = '\0';
    } else if(n%100 > 0 && n/100 == 0){
        char c2 = (n%10)+'0';
        char c1 = (n/10)+'0';
        f[0] = c1, f[1] = c2, f[2] = '\0';
    } else {
        f[0] = '9', f[1] = '9', f[2] = '\0';
    }
}
// makes RRQ packet
/**
char* make_rrq(char *filename){
    char *packet;
    packet = (char *)malloc(2+strlen(filename));
    memset(packet, 0, 2 + strlen(filename));
    strcat(packet, "01");//opcode
    strcat(packet, filename);
    return packet;
}
*/

/*
 * func:setWRQ_RRQ
 * dp:  组WRQ和RRQ包
 * arg1:    请求类型，1--读请求，2--写请求
 * arg2:    文件名
 * arg3:    文件名长度
 * arg4:    out --组的包
 *
 * return: 返回组的包的长度
 */
int setWRQ_RRQ(char rqType, char* filename, int filenameLen,  char* requestPackage)
{
    int i = 0;
    int length = 0;

    const char fileModel[] = TRANSMIT_MODE;       //发送模式为octet

    //帧头
    requestPackage[0] = 0;
    requestPackage[1] = rqType;

    //文件名
    memcpy(requestPackage + 2, filename, filenameLen);

    requestPackage[filenameLen + 2] = 0;

    memcpy(requestPackage + 3 + filenameLen, fileModel, strlen(fileModel));

    requestPackage[filenameLen + 3 + filenameLen + strlen(fileModel)] = 0;

    //requestPackage[length++] = 0;

    return (3 + filenameLen + strlen(fileModel) + 1);
}

// makes WRQ packet
/*
char* make_wrq(char *filename){
    char *packet;
    packet = (char *)malloc(2+strlen(filename));
    memset(packet, 0, 2 + strlen(filename));
    strcat(packet, "02");//opcode
    strcat(packet, filename);
    return packet;
}

*/

// makes data packet
char* make_data_pack(short block, char *data, int dataLen){
    char *packet;

    packet = (char *)malloc(4 + dataLen);
    memset(packet, 0, 4 + dataLen);
    //strcat(packet, "03");//opcode
    packet[1] = 3;
    //qDebug() << "make_data_pack: block = " << block;

    memcpy(packet + 2, (char *)&block, 2);
    packet[2] = block >> 8;

    packet[3] = block & 0xFF;

    //qDebug() << "make_data_pack: pack : " << *(packet + 2) << *(packet + 3);

    memcpy(packet + 4, data, dataLen);

    //strcat(packet, temp);
    //strcat(packet, data);

    //*packLen = 4 + dataLen;

    return packet;
}

// makes ACK packet
int make_ack(short block, char *packet){
    //char *packet;
    //packet = (char *)malloc(4);
    if(packet == NULL)
    {
        return 0;
    }

    memset(packet, 0, 4);
    packet[1] = 4;

    memcpy(packet + 2, (char*)&block, 2);

    return (4);
}

// makes ERR packet
char* make_err(short errcode, const char* errmsg, int *errLen){
    char *packet;
    packet = (char *)malloc(4+strlen(errmsg) + 1);
    memset(packet, 0, 4 + strlen(errmsg) + 1);
    //strcat(packet, "05");//opcode
    packet[1] = 5;

    memcpy(packet + 2, (char *)&errcode, 2);

    memcpy(packet + 4, errmsg, strlen(errmsg));


    *errLen = 4 + strlen(errmsg) + 1;

    return packet;
}

#if 0
int ip_uint2string(unsigned int ip_value, char *ip_str)
{
    unsigned int tmp = 0;

    int ip_len = 0;

    char tmp_str[8] = { 0 };

    //第一段
    tmp = ip_value >> 24;

    itoa(tmp, tmp_str, 10);

    tmp_str[strlen(tmp_str)] = '.';

    strcpy(ip_str, tmp_str);

    ip_len += strlen(tmp_str);

    //第二段

    memset(tmp_str, 0, 8);

    tmp = (ip_value >> 16) & 0xff;

    itoa(tmp, tmp_str, 10);

    tmp_str[strlen(tmp_str)] = '.';

    strcat(ip_str, tmp_str);

    ip_len += strlen(tmp_str);

    //第三段
    memset(tmp_str, 0, 8);

    tmp = (ip_value >> 8) & 0xff;

    itoa(tmp, tmp_str, 10);

    tmp_str[strlen(tmp_str)] = '.';

    strcat(ip_str, tmp_str);

    ip_len += strlen(tmp_str);

    //第四段
    memset(tmp_str, 0, 8);

    tmp = ip_value & 0xff;

    itoa(tmp, tmp_str, 10);

    strcat(ip_str, tmp_str);

    ip_len += strlen(tmp_str);

    return ip_len;
}

#endif
