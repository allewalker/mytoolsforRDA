
#include "stdafx.h"
#include "usp.h"
#include "mydevice.h"
enum USP_ENUM
{
	USP_CMD_RES = 0,	//通用应答
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
	USP_MAGIC_NUM = 0xabcd

};

s32 USP_ResultRx(void *pData);
s32 USP_RWUIDRx(void *pData);
s32 USP_UploadUIDRx(void *pData);
s32 USP_ReadVarRx(void *pData);
s32 USP_UploadVarRx(void *pData);
s32 USP_RWParamRx(void *pData);
s32 USP_UploadParamRx(void *pData);
s32 USP_DownloadFileRx(void *pData);
s32 USP_SetBRRx(void *pData);
s32 USP_ReadVersionRx(void *pData);
s32 USP_UploadVersionRx(void *pData);
s32 USP_RebootRx(void *pData);
s32 USP_ReadTraceRx(void *pData);
s32 USP_UploadTraceRx(void *pData);

s32 USP_ResultTx(USP_AnalyzeStruct *USP, u16 Cmd, u16 Result);
s32 USP_RWUIDTx(USP_AnalyzeStruct *USP, u32 *UID);
s32 USP_UploadUIDTx(USP_AnalyzeStruct *USP);
s32 USP_ReadVarTx(USP_AnalyzeStruct *USP);
s32 USP_UploadVarTx(USP_AnalyzeStruct *USP);
s32 USP_RWParamTx(USP_AnalyzeStruct *USP, u8 Sn, u8 *Data, u32 Len);
s32 USP_UploadParamTx(USP_AnalyzeStruct *USP, u8 Sn);
s32 USP_DownloadFileTx(USP_AnalyzeStruct *USP, u8 *Data, u32 Size);
s32 USP_SetBRTx(USP_AnalyzeStruct *USP, u32 NewBR);
s32 USP_ReadVersionTx(USP_AnalyzeStruct *USP);
s32 USP_UploadVersionTx(USP_AnalyzeStruct *USP);
s32 USP_RebootTx(USP_AnalyzeStruct *USP);
s32 USP_ReadTraceTx(USP_AnalyzeStruct *USP);
s32 USP_UploadTraceTx(USP_AnalyzeStruct *USP);

const CmdFunStruct USPFun[] =
{
	{
		USP_CMD_RES,
		USP_ResultRx,
	},
	{
		USP_CMD_RW_UID,
		USP_RWUIDRx,
	},
	{
		USP_CMD_UPLOAD_UID,
		USP_UploadUIDRx,
	},
	{
		USP_CMD_READ_VAR,
		USP_ReadVarRx,
	},
	{
		USP_CMD_UPLOAD_VAR,
		USP_UploadVarRx,
	},
	{
		USP_CMD_RW_PARAM,
		USP_RWParamRx,
	},
	{
		USP_CMD_UPLOAD_PARAM,
		USP_UploadParamRx,
	},
	{
		USP_CMD_DL_FILE,
		USP_DownloadFileRx,
	},

	{
		USP_CMD_SET_BR,
		USP_SetBRRx,
	},
	{
		USP_CMD_READ_VERSION,
		USP_ReadVersionRx,
	},
	{

		USP_CMD_UPLOAD_VERSION,
		USP_UploadVersionRx,
	},
	{
		USP_CMD_REBOOT,
		USP_RebootRx,
	},
	{
		USP_CMD_READ_TRACE,
		USP_ReadTraceRx,
	},
	{
		USP_CMD_UPLOAD_TRACE,
		USP_UploadTraceRx,
	}

};

void USP_SetHead(USP_AnalyzeStruct *USP, u16 Cmd, u8 Qos)
{
	USP_HeadStruct Head;
	Head.MagicNum = USP_MAGIC_NUM;
	Head.Cmd = Cmd;
	Head.DataSize = USP->OutLen;
	if (Head.DataSize)
	{
		Head.CRC16 = ~CRC16Cal(USP->OutBuf + sizeof(USP_HeadStruct), Head.DataSize, CRC16_START, CRC16_GEN);
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

u32 USP_CheckHead(u8 *Data)
{
	USP_HeadStruct Head;
	memcpy(&Head, Data, sizeof(USP_HeadStruct));
	if (Head.MagicNum == USP_MAGIC_NUM)
	{
		return 1;
	}
	return 0;
}

u32 USP_CheckLen(u8 *Data)
{
	USP_HeadStruct Head;
	memcpy(&Head, Data, sizeof(USP_HeadStruct));
	if (Head.Xor != XorCheck(Data, sizeof(Head)-1, 0))
	{

		return 0;
	}
	if (Head.DataSize <= (COM_BUF_LEN - 10))
	{
		return Head.DataSize;
	}
	else
	{
		return 0;
	}

}

u32 USP_Analyze(u8 *InBuf, u32 Len, u8 *OutBuf)
{
	USP_HeadStruct Head;
	USP_AnalyzeStruct USP;
	u32 RxLen = Len;
	u16 CRC16;

	memcpy(&Head, InBuf, sizeof(Head));

	USP.InLen = Head.DataSize;
	USP.InBuf = InBuf + sizeof(Head);
	USP.OutBuf = OutBuf;
	USP.OutLen = 0;
	USP.Qos = Head.Qos;

	if ((u32)Head.DataSize != (RxLen - sizeof(Head)))
	{
		goto USP_ANALYZE_DONE;
	}

	if (Head.DataSize)
	{
		CRC16 = ~CRC16Cal(InBuf + sizeof(Head), Head.DataSize, CRC16_START, CRC16_GEN);
		if (CRC16 != Head.CRC16)
		{
			goto USP_ANALYZE_DONE;
		}
	}

	if (Head.Cmd < (sizeof(USPFun) / sizeof(CmdFunStruct)))
	{
		USPFun[Head.Cmd].Func(&USP);
		if (USP.Qos)
		{
			return USP.OutLen;
		}
		else
		{
			return 0;
		}
	}

USP_ANALYZE_DONE:
	USP_ResultTx(&USP, Head.Cmd, 1);
	return USP.OutLen;

}

s32 USP_ResultRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_RWUIDRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_UploadUIDRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_ReadVarRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_UploadVarRx(void *pData)
{
	//如果是外接MCU，则解析
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_RWParamRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;

}

s32 USP_UploadParamRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_DownloadFileRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;

}

s32 USP_SetBRRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}
s32 USP_ReadVersionRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	return USP_UploadVersionTx(USP);
}
s32 USP_UploadVersionRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_RebootRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
}

s32 USP_ReadTraceRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	return USP_UploadTraceTx(USP);
}

s32 USP_UploadTraceRx(void *pData)
{
	USP_AnalyzeStruct *USP = (USP_AnalyzeStruct *)pData;
	USP->OutLen = 0;
	return 0;
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

s32 USP_UploadUIDTx(USP_AnalyzeStruct *USP)
{
	memcpy(USP->OutBuf + sizeof(USP_HeadStruct), gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID, sizeof(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID));
	USP->OutLen = sizeof(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID);
	USP_SetHead(USP, USP_CMD_UPLOAD_UID, 0);
	return 0;
}

s32 USP_ReadVarTx(USP_AnalyzeStruct *USP)
{
	USP->OutLen = 0;
	USP_SetHead(USP, USP_CMD_READ_VAR, 1);
	return 0;
}

s32 USP_UploadVarTx(USP_AnalyzeStruct *USP)
{
	u32 Pos = sizeof(USP_HeadStruct);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.Var[0], sizeof(gSys.DevData.Var));
	Pos += sizeof(gSys.DevData.Var);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.State[0], sizeof(gSys.DevData.State));
	Pos += sizeof(gSys.DevData.State);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.Error[0], sizeof(gSys.DevData.Error));
	Pos += sizeof(gSys.DevData.Error);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.IMEI[0], sizeof(gSys.DevData.IMEI));
	Pos += sizeof(gSys.DevData.IMEI);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.IMSI[0], sizeof(gSys.DevData.IMSI));
	Pos += sizeof(gSys.DevData.IMSI);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.ICCID[0], sizeof(gSys.DevData.ICCID));
	Pos += sizeof(gSys.DevData.ICCID);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.RMCInfo, sizeof(RMC_InfoStruct));
	Pos += sizeof(RMC_InfoStruct);

	memcpy(USP->OutBuf + Pos, &gSys.DevData.GSVInfoSave, sizeof(GSV_InfoStruct));
	Pos += sizeof(GSV_InfoStruct);
	USP->OutLen = Pos - sizeof(USP_HeadStruct);
	USP_SetHead(USP, USP_CMD_UPLOAD_VAR, 0);
	return 0;
}

s32 USP_RWParamTx(USP_AnalyzeStruct *USP, u8 Sn, u8 *Data, u32 Len)
{
	USP->OutBuf[sizeof(USP_HeadStruct)] = Sn;
	if (Data)
	{
		memcpy(USP->OutBuf + sizeof(USP_HeadStruct)+1, Data, Len);
		USP->OutLen = Len + 1;
	}
	else
	{
		USP->OutLen = 1;
	}
	USP_SetHead(USP, USP_CMD_RW_PARAM, 1);
	return 0;
}

s32 USP_UploadParamTx(USP_AnalyzeStruct *USP, u8 Sn)
{
	u32 Pos, i;
	u8 ParamType = Sn;
	ParamType = USP->InBuf[0];

	USP->OutBuf[sizeof(USP_HeadStruct)] = ParamType;
	Pos = 1;

	if (ParamType == 0xff)
	{
		for (i = 0; i < PARAM_TYPE_MAX; i++)
		{
			memcpy(USP->OutBuf + sizeof(USP_HeadStruct)+Pos, gSys.DevData.nParam[i].Data.pad, sizeof(Param_Byte60Union));
			Pos += sizeof(Param_Byte60Union);
		}
		USP->OutLen = 1 + sizeof(Param_Byte60Union)* PARAM_TYPE_MAX;
	}
	else
	{
		memcpy(USP->OutBuf + sizeof(USP_HeadStruct)+Pos, gSys.DevData.nParam[ParamType].Data.pad, sizeof(Param_Byte60Union));
		USP->OutLen = 1 + sizeof(Param_Byte60Union);
	}

	USP_SetHead(USP, USP_CMD_UPLOAD_PARAM, 0);
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

s32 USP_ReadVersionTx(USP_AnalyzeStruct *USP)
{
	USP->OutLen = 0;
	USP_SetHead(USP, USP_CMD_READ_VERSION, 1);
	return 0;
}

s32 USP_UploadVersionTx(USP_AnalyzeStruct *USP)
{
	USP->OutLen = 0;
	return 0;
}

s32 USP_ReadTraceTx(USP_AnalyzeStruct *USP)
{
	USP->OutLen = 0;
	USP_SetHead(USP, USP_CMD_READ_TRACE, 1);
	return 0;
}

s32 USP_UploadTraceTx(USP_AnalyzeStruct *USP)
{
	USP->OutLen = 4;
	memset(USP->OutBuf + sizeof(USP_HeadStruct), 0, 4);
	USP_SetHead(USP, USP_CMD_UPLOAD_TRACE, 0);
	return 0;
}
