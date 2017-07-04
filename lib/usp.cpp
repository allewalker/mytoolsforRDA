
#include "stdafx.h"
#include "usp.h"
#include "mydevice.h"

void USP_SetHead(USP_AnalyzeStruct *USP, u16 Cmd, u8 Qos)
{
	USP_HeadStruct Head;
	Head.MagicNum = USP_MAGIC_NUM;
	Head.Cmd = Cmd;
	Head.DataSize = USP->OutLen;
	if (Head.DataSize)
	{
		Head.CRC16 = ~CRC16Cal(USP->OutBuf + sizeof(USP_HeadStruct), Head.DataSize, CRC16_START, CRC16_CCITT_GEN, 0);
	}
	else
	{
		Head.CRC16 = 0;
	}
	Head.Qos = Qos;
	Head.Xor = XorCheck((u8 *)&Head, sizeof(USP_HeadStruct)-1, 0);
	memcpy(USP->OutBuf, &Head, sizeof(USP_HeadStruct));
	USP->OutLen += sizeof(USP_HeadStruct);
}


/*-----------------------------------------------------*/
s32 USP_ResultTx(USP_AnalyzeStruct *USP, u16 Cmd, u16 Result)
{
	memcpy(USP->OutBuf + sizeof(USP_HeadStruct), &Cmd, 2);
	memcpy(USP->OutBuf + sizeof(USP_HeadStruct)+2, &Result, 2);
	USP->OutLen = 4;
	USP_SetHead(USP, USP_CMD_RES, 0);
	return 0;
}

s32 USP_RWUIDTx(USP_AnalyzeStruct *USP, u32 *UID)
{
	if (UID)
	{
		memcpy(USP->OutBuf + sizeof(USP_HeadStruct), UID, sizeof(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID));
		USP->OutLen = sizeof(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID);
	}
	else
	{
		USP->OutLen = 0;
	}
	USP_SetHead(USP, USP_CMD_RW_UID, 1);
	return 0;
}

// s32 USP_UploadUIDTx(USP_AnalyzeStruct *USP)
// {
// 	memcpy(USP->OutBuf + sizeof(USP_HeadStruct), gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID, sizeof(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID));
// 	USP->OutLen = sizeof(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID);
// 	USP_SetHead(USP, USP_CMD_UPLOAD_UID, 0);
// 	return 0;
// }

// s32 USP_ReadVarTx(USP_AnalyzeStruct *USP)
// {
// 	USP->OutLen = 0;
// 	USP_SetHead(USP, USP_CMD_READ_VAR, 1);
// 	return 0;
// }
// 
// s32 USP_UploadVarTx(USP_AnalyzeStruct *USP)
// {
// 	u32 Pos = sizeof(USP_HeadStruct);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.Var[0], sizeof(gSys.DevData.Var));
// 	Pos += sizeof(gSys.DevData.Var);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.State[0], sizeof(gSys.DevData.State));
// 	Pos += sizeof(gSys.DevData.State);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.Error[0], sizeof(gSys.DevData.Error));
// 	Pos += sizeof(gSys.DevData.Error);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.IMEI[0], sizeof(gSys.DevData.IMEI));
// 	Pos += sizeof(gSys.DevData.IMEI);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.IMSI[0], sizeof(gSys.DevData.IMSI));
// 	Pos += sizeof(gSys.DevData.IMSI);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.ICCID[0], sizeof(gSys.DevData.ICCID));
// 	Pos += sizeof(gSys.DevData.ICCID);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.RMCInfo, sizeof(RMC_InfoStruct));
// 	Pos += sizeof(RMC_InfoStruct);
// 
// 	memcpy(USP->OutBuf + Pos, &gSys.DevData.GSVInfoSave, sizeof(GSV_InfoStruct));
// 	Pos += sizeof(GSV_InfoStruct);
// 	USP->OutLen = Pos - sizeof(USP_HeadStruct);
// 	USP_SetHead(USP, USP_CMD_UPLOAD_VAR, 0);
// 	return 0;
// }

s32 USP_RWParamTx(USP_AnalyzeStruct *USP, u8 Sn, u8 *Data, u32 Len)
{
	USP->OutBuf[sizeof(USP_HeadStruct)] = Sn;
	if (Data)
	{
		USP->OutBuf[sizeof(USP_HeadStruct)+1] = 0;
		memcpy(USP->OutBuf + sizeof(USP_HeadStruct)+2, Data, Len);
		USP->OutLen = Len + 2;
	}
	else if (Len)
	{
		USP->OutBuf[sizeof(USP_HeadStruct)+1] = 1;
		USP->OutLen = Len + 2;
	}
	else
	{
		USP->OutLen = 1;
	}
	USP_SetHead(USP, USP_CMD_RW_PARAM, 1);
	return 0;
}

s32 USP_DownloadFileTx(USP_AnalyzeStruct *USP, u8 *Data, u32 Size)
{
	memcpy(USP->OutBuf + sizeof(USP_HeadStruct), &Size, 4);
	if (Data)
	{
		memcpy(USP->OutBuf + sizeof(USP_HeadStruct)+4, Data, Size);
		USP->OutLen = Size + 4;
	}
	else
	{
		USP->OutLen = 4;
	}
	USP_SetHead(USP, USP_CMD_DL_FILE, 1);
	return 0;
}

s32 USP_SetBRTx(USP_AnalyzeStruct *USP, u32 NewBR)
{
	memcpy(USP->OutBuf + sizeof(USP_HeadStruct), &NewBR, 4);
	USP->OutLen = 4;
	USP_SetHead(USP, USP_CMD_SET_BR, 0);
	return 0;
}

// s32 USP_ReadVersionTx(USP_AnalyzeStruct *USP)
// {
// 	USP->OutLen = 0;
// 	USP_SetHead(USP, USP_CMD_READ_VERSION, 1);
// 	return 0;
// }
// 
// s32 USP_UploadVersionTx(USP_AnalyzeStruct *USP)
// {
// 	USP->OutLen = 0;
// 	return 0;
// }
// 
// s32 USP_ReadTraceTx(USP_AnalyzeStruct *USP)
// {
// 	USP->OutLen = 0;
// 	USP_SetHead(USP, USP_CMD_READ_TRACE, 1);
// 	return 0;
// }
// 
// s32 USP_UploadTraceTx(USP_AnalyzeStruct *USP)
// {
// 	USP->OutLen = 4;
// 	memset(USP->OutBuf + sizeof(USP_HeadStruct), 0, 4);
// 	USP_SetHead(USP, USP_CMD_UPLOAD_TRACE, 0);
// 	return 0;
// }
