#ifndef __AES_H__
#define __AES_H__

#include <string.h>

typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int               u32;
typedef char                        s8;
typedef short                       s16;
typedef int                       s32;
typedef long long					u64;

#define BPOLY (0x1b) //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define KEY_COUNT 1 
#define BLOCKSIZE (16) //!< Block size in number of bytes.

typedef struct  
{
	u8 AESKey[32];
	u8 Block1[256]; //!< Workspace 1.
	u8 Block2[256]; //!< Worksapce 2.
	u8 Block3[256];
	u8* powTbl; //!< Final location of exponentiation lookup table.
	u8* logTbl; //!< Final location of logarithm lookup table.
	u8* sBox; //!< Final location of s-box.
	u8* sBoxInv; //!< Final location of inverse s-box.
	u8* ExpandedKey; //!< Final location of expanded key. 
}AES_CtrlStruct;
void AES_DecInit(AES_CtrlStruct *AES);
void AES_Decrypt(AES_CtrlStruct *AES, unsigned char * buffer, unsigned char * chainBlock);
void AES_EncInit(AES_CtrlStruct *AES);
void AES_Encrypt(AES_CtrlStruct *AES, unsigned char * buffer, unsigned char * chainBlock);
#endif