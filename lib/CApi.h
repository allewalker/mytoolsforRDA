#ifndef __CAPI_H__
#define __CAPI_H__
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int               u32;
typedef char                        s8;
typedef short                       s16;
typedef int                       s32;
typedef long long					u64;
#define IsAlpha(c)      (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
#define IsHex(c)      (((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
#define IsDigit(c)        ((c >= '0') && (c <= '9'))
#define IsAlphaDigit(c)    (IsAlpha(c) || IsDigit(c))
#define IsHexDigit(c)    (IsHex(c) || IsDigit(c))
#define CRC32_GEN		(0x04C11DB7)
#define CRC16_GEN		(0x1021)
#define CRC32_START		(0xffffffff)
#define CRC16_START		(0xffff)

typedef  struct
{
	uint32_t param_max_num;
	uint32_t param_max_len;
	uint32_t param_num;
	char *param_str;
}CmdParam;


typedef struct {
	u8 Sec;
	u8 Min;
	u8 Hour;
	u8 Week;//表示日期0~6,sun~sat，表示预约时，bit0~bit6,sun~sat
}Time_UserDataStruct;

typedef struct {
	u16 Year;
	u8 Mon;
	u8 Day;
}Date_UserDataStruct;

typedef union
{
	u32 dwTime;
	Time_UserDataStruct Time;
}Time_Union;

typedef union
{
	u32 dwDate;
	Date_UserDataStruct Date;
}Date_Union;

typedef struct
{
	u8 VCC;
	u8 ACC;
	u8 VACC;
	u8 unuse;
}IO_ValueStruct;

typedef union
{
	u32 Val;
	IO_ValueStruct IOVal;
}IO_ValueUnion;

typedef union
{
	u32 u32_addr;
	u8 u8_addr[4];
}IP_AddrUnion;

typedef s32 (*MyAPIFunc)(void *p);

typedef struct {
	u32 Cmd;
	MyAPIFunc Func;
}CmdFunStruct;

typedef struct {
	uint8_t *Data;
	uint32_t Len;
	uint32_t Offset;
	uint32_t MaxLength;
	uint32_t DataSize;
}RBuffer;

void UnicodeToAsciiN(uint16_t *Src, uint8_t *Dst, uint32_t Len);
void AsciiToUnicodeN(uint8_t *Src, uint16_t *Dst, uint32_t Len);
uint32_t AsciiToHex(uint8_t *Src, uint32_t len, uint8_t *Dst);
uint32_t HexToAscii(uint8_t *Src, uint32_t Len, uint8_t *Dst);
uint32_t StrToUint(const uint8_t *Src);
u8 XorCheck(u8 *Data, u32 Len, u8 CheckStart);
uint16_t CRC16Cal(uint8_t *Src, uint16_t Len, uint16_t CRC16Last, uint16_t CRCRoot);
void CRC32_CreateTable(uint32_t *Tab, uint32_t Gen);
uint32_t CRC32_Cal(uint32_t * CRC32_Table, uint8_t *Buf, uint32_t Size, uint32_t CRC32Last);
u32 ReadRBuffer(RBuffer *Buf, u8 *Data, u32 Len);
u32 QueryRBuffer(RBuffer *Buf, u8 *Data, u32 Len);
void InitRBuffer(RBuffer *Buf, u8 *Data, u32 MaxLen, u32 DataSize);
void DelRBuffer(RBuffer *Buf, u32 Len);
u32 WriteRBufferForce(RBuffer *Buf, const char *Data, u32 Len);
u32 TransferUnpack(u8 Flag, u8 Code, u8 F1, u8 F2, u8 *InBuf, u32 Len, u8 *OutBuf);
u32 TransferPack(u8 Flag, u8 Code, u8 F1, u8 F2, u8 *InBuf, u32 Len, u8 *OutBuf);
/************************************************************************/
/*MD5相关                                                                      */
/************************************************************************/
typedef struct
{
	uint32_t count[2];
	uint32_t state[4];
	uint8_t buffer[64];
}MD5_CTX;


void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, uint8_t *input, uint32_t inputlen);
void MD5Final(MD5_CTX *context, uint8_t digest[16]);
void MD5Transform(uint32_t state[4], uint8_t block[64]);
void MD5Encode(uint8_t *output, uint32_t *input, uint32_t len);
void MD5Decode(uint32_t *output, uint8_t *input, uint32_t len);
unsigned int CmdParseParam(char* pStr, CmdParam *CmdParam);
#endif 

