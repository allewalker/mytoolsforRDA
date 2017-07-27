#ifndef __MD5_H__
#define __MD5_H__
#include <string.h>
/************************************************************************/
/*MD5ฯเนุ                                                                      */
/************************************************************************/
typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int               u32;
typedef char                        s8;
typedef short                       s16;
typedef int                       s32;
typedef long long					u64;
typedef struct
{
	u32 count[2];
	u32 state[4];
	u8 buffer[64];
}MD5_CTX;

void MD5_Cal(void *Text, u32 TextLen, u8 *Code);
#endif