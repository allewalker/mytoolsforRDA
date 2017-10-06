#ifndef __CAPI_H__
#define __CAPI_H__
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#define IsAlpha(c)      (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
#define IsHex(c)      (((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
#define IsDigit(c)        ((c >= '0') && (c <= '9'))
#define IsAlphaDigit(c)    (IsAlpha(c) || IsDigit(c))
#define IsHexDigit(c)    (IsHex(c) || IsDigit(c))
#define CRC32_GEN		(0x04C11DB7)
#define CRC16_CCITT_GEN		(0x1021)
#define CRC16_MODBUS_GEN		(0x8005)
#define CRC32_START		(0xffffffff)
#define CRC16_START		(0xffff)
typedef uint64_t		LongInt;
//typedef uint32_t		LongInt;
typedef  struct
{
	uint32_t param_max_num;
	uint32_t param_max_len;
	uint32_t param_num;
	char *param_str;
}CmdParam;


typedef struct {
	uint8_t Sec;
	uint8_t Min;
	uint8_t Hour;
	uint8_t Week;//表示日期0~6,sun~sat，表示预约时，bit0~bit6,sun~sat
}Time_UserDataStruct;

typedef struct {
	uint16_t Year;
	uint8_t Mon;
	uint8_t Day;
}Date_UserDataStruct;

typedef union
{
	uint32_t dwTime;
	Time_UserDataStruct Time;
}Time_Union;

typedef union
{
	uint32_t dwDate;
	Date_UserDataStruct Date;
}Date_Union;

typedef struct
{
	uint8_t VCC;
	uint8_t ACC;
	uint8_t VACC;
	uint8_t unuse;
}IO_ValueStruct;

typedef union
{
	uint32_t Val;
	IO_ValueStruct IOVal;
}IO_ValueUnion;

typedef union
{
	uint32_t u32_addr;
	uint8_t u8_addr[4];
}IP_AddrUnion;

typedef int32_t (*MyAPIFunc)(void *p);

typedef struct {
	uint32_t Cmd;
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
uint32_t IntPow(uint32_t x, uint8_t y);
uint32_t HexToAscii(uint8_t *Src, uint32_t Len, uint8_t *Dst);
uint32_t StrToUint(const uint8_t *Src);
uint8_t XorCheck(uint8_t *Data, uint32_t Len, uint8_t CheckStart);
uint16_t CRC16Cal(uint8_t *Src, uint16_t Len, uint16_t CRC16Last, uint16_t CRCRoot, uint8_t IsReverse);
void CRC32_CreateTable(uint32_t *Tab, uint32_t Gen);
uint32_t CRC32_Cal(uint32_t * CRC32_Table, uint8_t *Buf, uint32_t Size, uint32_t CRC32Last);
uint32_t ReadRBuffer(RBuffer *Buf, uint8_t *Data, uint32_t Len);
uint32_t QueryRBuffer(RBuffer *Buf, uint8_t *Data, uint32_t Len);
void InitRBuffer(RBuffer *Buf, uint8_t *Data, uint32_t MaxLen, uint32_t DataSize);
void DelRBuffer(RBuffer *Buf, uint32_t Len);
uint32_t WriteRBufferForce(RBuffer *Buf, uint8_t *Data, uint32_t Len);
uint32_t TransferUnpack(uint8_t Flag, uint8_t Code, uint8_t F1, uint8_t F2, uint8_t *InBuf, uint32_t Len, uint8_t *OutBuf);
uint32_t TransferPack(uint8_t Flag, uint8_t Code, uint8_t F1, uint8_t F2, uint8_t *InBuf, uint32_t Len, uint8_t *OutBuf);
unsigned int CmdParseParam(char* pStr, CmdParam *CmdParam);
double GPS_Distance(double lat1, double lat2, double lgt1, double lgt2);
LongInt UTC2Tamp(Date_UserDataStruct *Date, Time_UserDataStruct *Time);
uint32_t Tamp2UTC(LongInt Sec, Date_UserDataStruct *Date, Time_UserDataStruct *Time, uint32_t LastDDay);
#endif 

