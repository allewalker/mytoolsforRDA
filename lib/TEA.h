#ifndef __TEA_H__
#define __TEA_H__

#include <string.h>
#include <stdint.h>


typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int               u32;
typedef char                        s8;
typedef short                       s16;
typedef int                       s32;
typedef long long					u64;

enum 
{
	TEA_TYPE_ORG,
	TEA_TYPE_X,
	TEA_TYPE_XX,
};
void TEA_Encrypt(u32 *Data, u32 *Key, u32 Rounds, u8 Type);
void TEA_Decrypt(u32 *Data, u32 *Key, u32 Rounds, u8 Type);
#endif