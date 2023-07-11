#include "crc.h"
#include <QDebug>
//生成多项式最高位一定不是0，则就是1，就像十进制数最高位一定不是0一样，所以除数实际上是33位、17位、9位
static uint32_t POLYNOMIAL32 = 0x04C11DB7;//32位crc多项式
static uint16_t POLYNOMIAL16 = 0x1021;//16位crc多项式
static uint8_t POLYNOMIAL8 = 0x01;//8位crc多项式
static uint16_t strtest[20] = {'a','b','c','d'};//测试数据

/*
*验证网址：http://www.ghsi.de/pages/subpages/Online%20CRC%20Calculation/
* 个人认为crc校验的原理：将原数据加上一个crc校验码（FCS），使得原数据能被生成多项式整除，整除规则为模2除法
*/
/* 计算8位数据的crc的值 */
uint8_t crc8_byte(uint8_t data){
    uint8_t crc = data;
    int i = 0;
    for(i=0; i < 8; i++)
    { /* 每次左移相当于末尾补0，一共补了8个0 ,for循环里是模2除法的过程*/
        if((crc & 0x80) != 0)
        {
            crc <<= 1; //因为生成多项式省略了最高位1，所以需要左移移位，相当于减去了最高位
            crc ^= POLYNOMIAL8;
        }
        else
        {
            crc <<= 1;
        }
    }
    return crc;
}
//将字节串传入crc8_byte得一个字节的crc校验码，结果异或得串的crc校验码
uint8_t crc8_naive(uint8_t *p_buf, uint8_t *p_end)
{
    uint8_t crc = INIT_VALUE_8;
    uint8_t data;
    int i = 0;
    for (;p_buf < p_end;p_buf++){
        i++;
        //printf("%d\n",i);
        data = ((uint8_t)(*p_buf));
        /* 原来的CRC和现在的CRC之间的关系是 原来CRC左移0位之后的余项 + 新的数据的CRC */
        crc = crc8_byte(crc) ^ crc8_byte(data);
    }
    return crc ^ FINAL_VALUE_8;
}

/* 计算数据左移8位之后的crc的值 */
//uint32_t crc32_byte(uint32_t data){
//    uint32_t crc = data << 24; //先左移r-8位
//    for(int i=0;i<8;i++){ /* 每次左移相当于末尾补0，一共补了8个0 */
//        if((crc & 0x80000000) != 0){ /* 商x的n次方，需要做一次减法 */
//            crc <<= 1; //因为生成多项式省略了最高位1，所以需要左移移位，相当于减去了最高位
//            crc ^= POLYNOMIAL32;
//        }else{ /* 商0 */
//            crc <<= 1;
//        }
//    }
//    return crc;
//}

/* 计算32位数据的crc的值 */
uint32_t crc32_byte2(uint32_t data){
    uint32_t crc = data;
    for(int i=0;i<8;i++){ /* 每次左移相当于末尾补0，一共补了8个0 */
        if((crc & 0x80000000) != 0){ /* 商x的n次方，需要做一次减法 */
            crc <<= 1; //因为生成多项式省略了最高位1，所以需要左移移位，相当于减去了最高位
            crc ^= POLYNOMIAL32;
        }else{ /* 商0 */
            crc <<= 1;
        }
    }
    return crc;
}
//将一个字节传入crc32_byte2()得crc校验码，结果异或得串的crc校验码
uint32_t crc32_naive(uint8_t *p_buf, uint8_t *p_end){
    uint32_t crc = INIT_VALUE_32;
    uint32_t data;
    for (;p_buf < p_end;p_buf++){
        data = ((uint32_t)(*p_buf)) << 24;
        /* 原来的CRC和现在的CRC之间的关系是 原来CRC左移24位之后的余项 + 新的数据的CRC */
        crc = crc32_byte2(crc) ^ crc32_byte2(data);
    }
    return crc ^ FINAL_VALUE_32;
}

uint16_t crc16_byte2(uint16_t data){
    uint16_t crc = data;
    for(int i=0;i<8;i++){ /* 每次左移相当于末尾补0，一共补了8个0 */
        if((crc & 0x8000) != 0){ /* 商x的n次方，需要做一次减法 */
            crc <<= 1; //因为生成多项式省略了最高位1，所以需要左移移位，相当于减去了最高位
            crc ^= POLYNOMIAL16;
        }else{ /* 商0 */
            crc <<= 1;
        }
    }
    return crc;
}
uint16_t crc16_naive(uint8_t *p_buf, uint8_t *p_end){
    uint16_t crc = INIT_VALUE_16;
    uint16_t data;
    for (;p_buf < p_end;p_buf++){
        data = ((uint16_t)(*p_buf)) << 8;
        /* 原来的CRC和现在的CRC之间的关系是 原来CRC左移8位之后的余项 + 新的数据的CRC */
        crc = crc16_byte2(crc) ^ crc16_byte2(data);
    }
    return crc ^ FINAL_VALUE_16;
}

uint16_t crc16_table(char* buf, size_t len, unsigned short initVal)
{
    static int crc_table[] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };
    uint16_t crc = initVal;
    for (size_t i = 0; i < len; i++)
    {
        crc = ((crc << 8) ^ crc_table[((crc >> 8) ^ buf[i]) & 0xff]);
    }
    return crc;

}
