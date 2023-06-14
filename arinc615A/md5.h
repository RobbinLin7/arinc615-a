/**
 * @author Horst Xu
 * @date 2015-07-10
 * @contact 271021733@qq.com
 */
#ifndef __MD5_H__
#define __MD5_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @input: result -- store the calculation result
 *         size   -- size of result. Make sure it's at least 33
 *                   since the result is a 32-byte hexdecimal string.
 *         message-- string to be encrypted
 *         flag   -- 0 means upper case output, 1 means lower case output
 * @return: 0 -- success
 *          1 -- result size less than 33
 *          2 -- calloc failed
 */
int32_t cal_md5(char *result, size_t size, const char *message, uint32_t flag);

#ifdef __cplusplus
}
#endif

#endif //__MD5_H__