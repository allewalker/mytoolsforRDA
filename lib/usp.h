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
	uint16_t MagicNum;
	uint16_t DataSize;
	uint16_t Cmd;
	uint16_t CRC16;
	uint8_t Qos;
	uint8_t Xor;

}USP_HeadStruct;

typedef struct
{
	uint8_t *InBuf;
	uint8_t *OutBuf;
	uint32_t InLen;
	uint32_t OutLen;
	uint8_t Qos;
}USP_AnalyzeStruct;

void USP_SetHead(USP_AnalyzeStruct *USP, uint16_t Cmd, uint8_t Qos);
int32_t USP_ResultTx(USP_AnalyzeStruct *USP, uint16_t Cmd, uint16_t Result);
int32_t USP_RWUIDTx(USP_AnalyzeStruct *USP, uint32_t *UID);
int32_t USP_RWParamTx(USP_AnalyzeStruct *USP, uint8_t Sn, uint8_t *Data, uint32_t Len);
int32_t USP_DownloadFileTx(USP_AnalyzeStruct *USP, uint8_t *Data, uint32_t Size);
int32_t USP_SetBRTx(USP_AnalyzeStruct *USP, uint32_t NewBR);
#endif
