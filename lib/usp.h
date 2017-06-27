#ifndef __CUST_USP_H__
#define __CUST_USP_H__
#include "CApi.h"
typedef struct
{
	u16 MagicNum;
	u16 DataSize;
	u16 Cmd;
	u16 CRC16;
	u8 Qos;
	u8 Xor;

}USP_HeadStruct;

typedef struct
{
	u8 *InBuf;
	u8 *OutBuf;
	u32 InLen;
	u32 OutLen;
	u8 Qos;
}USP_AnalyzeStruct;

u32 USP_CheckHead(u8 *Data);
u32 USP_Analyze(u8 *InBuf, u32 Len, u8 *OutBuf);
void USP_SetHead(USP_AnalyzeStruct *USP, u16 Cmd, u8 Qos);
#endif
