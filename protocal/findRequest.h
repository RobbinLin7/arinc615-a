#ifndef FINDREQUEST_H
#define FINDREQUEST_H
typedef struct FIND_REQ{
    unsigned short opcode;  /* 操作码 */
    unsigned char ASCII_String; /* 数据内容 */
    unsigned char Packet_Terminator;    /* 包终止符 */
    int tftpRetryNum;
    int tftpTimeout;
    int lusPeriod;
}find_req, *pfind_req;


#endif // FINDREQUEST_H
