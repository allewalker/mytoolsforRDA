#ifndef __DES_H__
#define __DES_H__

#include <string.h>
#include <stdint.h>


typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int               u32;
typedef char                        s8;
typedef short                       s16;
typedef int                       s32;
typedef long long					u64;

typedef struct
{
	u8 KeyBit[48];
}DES_SubKeyBitStruct;

void DES_MakeSubKey(u8 *Key, DES_SubKeyBitStruct *SubKey);
int DES_Cal(u8 *BlockIn, DES_SubKeyBitStruct *SubKey, u8 *BlockOut, u8 IsDecrypt);

#endif