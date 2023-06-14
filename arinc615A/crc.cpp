#include "crc.h"

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

