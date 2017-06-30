#ifndef __CUST_USP_H__
#define __CUST_USP_H__
#include "CApi.h"

enum USP_ENUM
{
	USP_CMD_RES = 0,	//Í¨ÓÃÓ¦´ð
	USP_CMD_RW_UID,
	USP_CMD_UPLOAD_UID,
	USP_CMD_READ_VAR,
	USP_CMD_UPLOAD_VAR,
	USP_CMD_RW_PARAM,
	USP_CMD_UPLOAD_PARAM,
	USP_CMD_DL_FILE,
	USP_CMD_SET_BR,
	USP_CMD_READ_VERSION,
	USP_CMD_UPLOAD_VERSION,
	USP_CMD_REBOOT,
	USP_CMD_READ_TRACE,
	USP_CMD_UPLOAD_TRACE,
	USP_CMD_MAX,
	USP_MAGIC_NUM = 0xabcd,
	USP_LEN_MAX = 2048,

};

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

void USP_SetHead(USP_AnalyzeStruct *USP, u16 Cmd, u8 Qos);
s32 USP_ResultTx(USP_AnalyzeStruct *USP, u16 Cmd, u16 Result);
s32 USP_RWUIDTx(USP_AnalyzeStruct *USP, u32 *UID);
s32 USP_RWParamTx(USP_AnalyzeStruct *USP, u8 Sn, u8 *Data, u32 Len);
s32 USP_DownloadFileTx(USP_AnalyzeStruct *USP, u8 *Data, u32 Size);
s32 USP_SetBRTx(USP_AnalyzeStruct *USP, u32 NewBR);
#endif
