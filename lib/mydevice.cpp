#include "stdafx.h"
#include "mydevice.h"
#include "PrintMsg.h"
#include "usp.h"
SysVar_Struct gSys;
#define USP_COM_TO (500)

static void MyDeviceChangeWorkMode(void)
{
	if (gSys.ComCtrl.hCom)
	{
		CloseHandle(gSys.ComCtrl.hCom);
		gSys.ComCtrl.hCom = NULL;
	}

	if (!gSys.ComCtrl.IsWork)
	{
		gSys.WorkMode = WORK_IDLE;
		PostMessage(gSys.mMainWnd, WM_COM_STATE, WPARAM(0), LPARAM(0));
	}
	else
	{
		if (gSys.ComCtrl.ComNo)
		{
			gDBG.Trace("%s %u:entry normal com mode %u %u!\r\n", __FUNCTION__, __LINE__, gSys.ComCtrl.ComNo, gSys.ComCtrl.CommBR);
			gSys.ComCtrl.hCom = OpenComPort(gSys.ComCtrl.ComNo, gSys.ComCtrl.CommBR);
			if (gSys.ComCtrl.hCom)
			{
				gSys.WorkMode = WORK_NORMAL_COM;
				PostMessage(gSys.mMainWnd, WM_COM_STATE, WPARAM(0), LPARAM(0));
			}
		}
		else
		{
			gDBG.Trace("%s %u:entry USP mode!\r\n", __FUNCTION__, __LINE__);
			gSys.WorkMode = WORK_USP_AUTO;
			PostMessage(gSys.mMainWnd, WM_COM_STATE, WPARAM(0), LPARAM(0));
		}
	}
}

static int32_t WaitForEvent(uint32_t To)
{
	DWORD Result;
	uint32_t EventID = 0;
	Result = WaitForMultipleObjects(EVENT_WAIT_NUM, gSys.Event, FALSE, To);
	switch (Result)
	{
	case WAIT_FAILED:
	case WAIT_TIMEOUT:
		return -1;
	default:
		EventID = Result - WAIT_OBJECT_0;
		gDBG.Trace("%s %u:Event ID %u\r\n", __FUNCTION__, __LINE__, EventID);
		ResetEvent(gSys.Event[EventID]);
		if (EventID == EVENT_THREAD_STOP)
		{
			gSys.ThreadRun = false;
		}
		break;
	}
	return EventID;
}

static int32_t WaitForSleep(uint32_t To)
{
	ResetEvent(gSys.Event[EVENT_COM_RX]);
	WaitForSingleObject(gSys.Event[EVENT_COM_RX], To);
	return 0;
}

static int32_t MyDeviceUSPRx(uint8_t *Buf, uint32_t BufLen, uint32_t To)
{
	uint32_t PassTime = 0;
	uint32_t RxLen = 0;//总共获取到的字节数
	uint32_t Result;
	uint32_t PackLen = 0;
	DWORD Error = 0;
	USP_HeadStruct Head;
	gSys.ComCtrl.ErrorFlag = 0;
	while (PassTime < To)
	{
		Result = ReadCom(gSys.ComCtrl.hCom, &Buf[RxLen], sizeof(Head), &Error);
		if (Result)
		{
			PassTime = 0;
			RxLen += Result;
		}
		else
		{
			if (Error)
			{
				gDBG.Trace("%s %u:%u\r\n", __FUNCTION__, __LINE__, Error);
				gSys.ComCtrl.ErrorFlag = 1;
				return -Error;
			}
		}

		if (PackLen)
		{
			if (RxLen >= PackLen)
			{
				return RxLen;
			}
		}
		else
		{
			if (RxLen >= sizeof(Head))
			{
				memcpy(&Head, Buf, sizeof(Head));
				if (Head.Xor == XorCheck(Buf, sizeof(Head) - 1, 0))
				{
					if (Head.MagicNum == USP_MAGIC_NUM)
					{
						if (Head.DataSize)
						{
							if (Head.DataSize < (USP_LEN_MAX - sizeof(Head)))
							{
								PackLen = Head.DataSize + sizeof(Head);
								PassTime = 0;
							}
							else
							{
								gDBG.Trace("%s %u:%u\r\n", __FUNCTION__, __LINE__, Head.DataSize);
								return -1;
							}

						}
						else
						{
							return sizeof(Head);
						}

					}
				}
			}
		}
		WaitForSleep(1);
		PassTime++;
	}
	return -1;
}

static int32_t MyDeviceUSPProc(uint16_t Cmd, uint8_t *pData, uint32_t Len, uint32_t Param, uint32_t To)
{
	uint8_t RxBuf[USP_LEN_MAX];
	uint8_t TxBuf[USP_LEN_MAX];
	int32_t Result;
	uint32_t Pos;
	uint32_t UID[3];
	USP_HeadStruct Head;
	USP_AnalyzeStruct USP;
	uint16_t LastCmd;
	uint16_t LastResult;
	uint8_t *DataStart;
	uint8_t ParamSn;
	USP.OutBuf = TxBuf;
	USP.OutLen = 0;
	switch (Cmd)
	{
	case USP_CMD_RW_UID:
		if (pData)
		{
			memcpy(UID, pData, 12);
		}
		USP_RWUIDTx(&USP, UID);
		break;
	case USP_CMD_RW_PARAM:
		USP_RWParamTx(&USP, Param, pData, Len);
		break;
	case USP_CMD_DL_FILE:
		USP_DownloadFileTx(&USP, pData, Len);
		break;
	case USP_CMD_SET_BR:
		USP_SetBRTx(&USP, Param);
		break;
	case USP_CMD_READ_VAR:
	case USP_CMD_READ_VERSION:
	
	case USP_CMD_READ_TRACE:
		USP_SetHead(&USP, Cmd, 1);
		break;
	case USP_CMD_REBOOT:
		USP_SetHead(&USP, Cmd, 0);
		break;
	default:
		return -1;

	}
USP_RX:
	WriteCom(gSys.ComCtrl.hCom, TxBuf, USP.OutLen);
	if ( (Cmd == USP_CMD_SET_BR) || (Cmd == USP_CMD_REBOOT) )
	{
		WaitForSleep(100);
		return 0;
	}
	Result = MyDeviceUSPRx(RxBuf, USP_LEN_MAX, To);

	if (Result > 0)
	{
		memcpy(&Head, RxBuf, sizeof(Head));
		if (Head.DataSize)
		{
			if (Head.CRC16 != (uint16_t)~CRC16Cal(RxBuf + sizeof(Head), Head.DataSize, CRC16_START, CRC16_CCITT_GEN, 0))
			{
				gDBG.Trace("%s %u:%u crc %04x %04x\r\n", __FUNCTION__, __LINE__, Head.Cmd, Head.CRC16, (uint16_t)~CRC16Cal(RxBuf + sizeof(Head), Head.DataSize, CRC16_START, CRC16_CCITT_GEN, 0));
				return -1;
			}
			DataStart = &RxBuf[sizeof(Head)];

			switch (Head.Cmd)
			{
			case USP_CMD_RES:
				memcpy(&LastCmd, DataStart, 2);
				memcpy(&LastResult, DataStart + 2, 2);
				if (LastCmd != Cmd)
				{
					gDBG.Trace("%s %u:Tx %u Rx %u\r\n", __FUNCTION__, __LINE__, Cmd, LastCmd);
					return -1;
				}
				else if (LastResult)
				{
					gDBG.Trace("%s %u:Rx %u %u \r\n", __FUNCTION__, __LINE__, Cmd, LastResult);
					return -1;
				}
				else
				{
					return USP_CMD_RES;
				}
				break;
			case USP_CMD_UPLOAD_UID:
				if (Head.DataSize != 12)
				{
					gDBG.Trace("%s %u:UID Len %u \r\n", __FUNCTION__, __LINE__, Head.DataSize);
					return -1;
				}
				memcpy(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID, DataStart, Head.DataSize);
				break;
			case USP_CMD_UPLOAD_VAR:
				if (Head.DataSize != sizeof(gSys.DevData.Var) + sizeof(gSys.DevData.State) + sizeof(gSys.DevData.Error) + sizeof(gSys.DevData.IMEI) + sizeof(gSys.DevData.IMSI) + sizeof(gSys.DevData.ICCID) + sizeof(RMC_InfoStruct)+sizeof(GSV_InfoStruct))
				{
					gDBG.Trace("%s %u:Var Len %u %u\r\n", __FUNCTION__, __LINE__, Head.DataSize, sizeof(gSys.DevData.Var) + sizeof(gSys.DevData.State) + sizeof(gSys.DevData.Error) + sizeof(gSys.DevData.IMEI) + sizeof(gSys.DevData.IMSI) + sizeof(gSys.DevData.ICCID) + sizeof(RMC_InfoStruct)+sizeof(GSV_InfoStruct));
					return -1;
				}
				Pos = 0;
				memcpy(gSys.DevData.Var, DataStart + Pos, sizeof(gSys.DevData.Var));
				Pos += sizeof(gSys.DevData.Var);

				memcpy(gSys.DevData.State, DataStart + Pos, sizeof(gSys.DevData.State));
				Pos += sizeof(gSys.DevData.State);

				memcpy(gSys.DevData.Error, DataStart + Pos, sizeof(gSys.DevData.Error));
				Pos += sizeof(gSys.DevData.Error);

				memcpy(gSys.DevData.IMEI, DataStart + Pos, sizeof(gSys.DevData.IMEI));
				Pos += sizeof(gSys.DevData.IMEI);

				memcpy(gSys.DevData.IMSI, DataStart + Pos, sizeof(gSys.DevData.IMSI));
				Pos += sizeof(gSys.DevData.IMSI);

				memcpy(gSys.DevData.ICCID, DataStart + Pos, sizeof(gSys.DevData.ICCID));
				Pos += sizeof(gSys.DevData.ICCID);

				memcpy(&gSys.DevData.RMCInfo, DataStart + Pos, sizeof(RMC_InfoStruct));
				Pos += sizeof(RMC_InfoStruct);

				memcpy(&gSys.DevData.GSVInfoSave, DataStart + Pos, sizeof(GSV_InfoStruct));
				Pos += sizeof(GSV_InfoStruct);
				break;
			case USP_CMD_UPLOAD_PARAM:
				ParamSn = DataStart[0];
				DataStart++;
				if (ParamSn == 0xff)
				{
					if (Head.DataSize != (PARAM_TYPE_MAX * sizeof(Param_Byte60Union) + 1))
					{
						gDBG.Trace("%s %u:param len %u %u\r\n", __FUNCTION__, __LINE__, PARAM_TYPE_MAX * sizeof(Param_Byte60Union), Head.DataSize);
						return -1;
					}

					Pos = 0;
					for (int i = 0; i < PARAM_TYPE_MAX; i++)
					{
						memcpy(gSys.DevData.nParam[i].Data.pad, DataStart + Pos, sizeof(Param_Byte60Union));
						Pos += sizeof(Param_Byte60Union);
					}
				}
				else if (ParamSn < PARAM_TYPE_MAX)
				{
					if (Head.DataSize != (sizeof(Param_Byte60Union) + 1))
					{
						gDBG.Trace("%s %u:param len %u %u\r\n", __FUNCTION__, __LINE__, sizeof(Param_Byte60Union), Head.DataSize);
						return -1;
					}

					memcpy(gSys.DevData.nParam[ParamSn].Data.pad, DataStart + Pos, sizeof(Param_Byte60Union));
				}
				else
				{
					gDBG.Trace("%s %u:param sn %u\r\n", __FUNCTION__, __LINE__, ParamSn);
					return -1;
				}
				break;
			case USP_CMD_UPLOAD_VERSION:
				memcpy(&gSys.DevData.MainVersion, DataStart, 4);
				break;
			case USP_CMD_UPLOAD_TRACE:
				WriteRBufferForce(&gSys.UartRxBuf, DataStart, Head.DataSize);
				PostMessage(gSys.mMainWnd, WM_COM_RX, WPARAM(0), LPARAM(0));
				break;
			default:
				break;
			}
			return Head.Cmd;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

static void MyDeviceAutoMode(void)
{
	uint32_t dwLen;
	uint32_t TxLen;
	uint32_t FinishLen;
	int32_t Result;
	uint8_t *DataStart;
	uint8_t Retry;
	uint32_t i;
	OperationReq_Struct Req;
	//首先检查是否已经扫描到设备
	if (gSys.ComCtrl.hCom)
	{
		if (gSys.OperationList.Len)
		{
			while (gSys.OperationList.Len)
			{
				gSys.ComCtrl.ErrorFlag = 0;
				ReadRBuffer(&gSys.OperationList, (uint8_t *)&Req, 1);
				switch (Req.ID)
				{
				case REQ_RESET:
					Result = MyDeviceUSPProc(USP_CMD_REBOOT, NULL, 0, 0, USP_COM_TO);
					PostMessage(gSys.mMainWnd, WM_CORE_RESET, WPARAM(0), LPARAM(0));
					WaitForSleep(10);
					goto ERROR_OUT;
				case REQ_SET_PARAM:
					Result = MyDeviceUSPProc(USP_CMD_RW_PARAM, (uint8_t *)Req.pData, Req.Param1, Req.Param2, USP_COM_TO);
					if (Result != USP_CMD_UPLOAD_PARAM)
					{
						gDBG.Trace("%s %u:set param fail!\r\n", __FUNCTION__, __LINE__);
					}
					if (Req.pData)
					{
						delete[] Req.pData;
					}
				case REQ_SET_UID:
					Result = MyDeviceUSPProc(USP_CMD_RW_UID, (uint8_t *)Req.pData, Req.Param1, Req.Param2, USP_COM_TO);
					if (Result != USP_CMD_UPLOAD_UID)
					{
						gDBG.Trace("%s %u:set uid fail!\r\n", __FUNCTION__, __LINE__);
					}
					if (Req.pData)
					{
						delete[] Req.pData;
					}
					break;
				case REQ_UPGRADE:
					dwLen = (sizeof(File_HeadStruct)+(gSys.UpgradeFileBuf.Head.BinFileLen * 4096));
					Result = MyDeviceUSPProc(USP_CMD_DL_FILE, NULL, dwLen, 0, USP_COM_TO);
					if (Result >= 0)
					{
						FinishLen = 0;
						DataStart = (uint8_t *)&gSys.UpgradeFileBuf;
						while (FinishLen < dwLen)
						{
							if ((dwLen - FinishLen) > 1024)
							{
								TxLen = 1024;
							}
							else
							{
								TxLen = dwLen - FinishLen;
							}
							for (Retry = 0; Retry < 5; Retry++)
							{
								Result = MyDeviceUSPProc(USP_CMD_DL_FILE, DataStart + FinishLen, TxLen, 0, USP_COM_TO);
								if (Result >= 0)
								{
									FinishLen += TxLen;
									gDBG.Trace("%s %u:%d %d!\r\n", __FUNCTION__, __LINE__, FinishLen, dwLen);
									break;
								}
								WaitForSleep(50);
							}

							if (Result < 0)
							{
								gDBG.Trace("%s %u:upgrade fail!\r\n", __FUNCTION__, __LINE__);
								break;
							}
							WaitForSleep(10);
						}

						if (FinishLen >= dwLen)
						{
							gDBG.Trace("%s %u:upgrade ok!\r\n", __FUNCTION__, __LINE__);
						}
						else
						{
							gDBG.Trace("%s %u:upgrade fail!\r\n", __FUNCTION__, __LINE__);
						}
					}
					break;
				default:
					Result = 0;
					break;
				}
				if ((Result < 0) || gSys.ComCtrl.ErrorFlag)
				{
					goto ERROR_OUT;
				}
			}
		}
		else
		{
			Result = MyDeviceUSPProc(USP_CMD_READ_VAR, 0, 0, 0, USP_COM_TO);
			if (Result != USP_CMD_UPLOAD_VAR)
			{
				if (!gSys.ComCtrl.ErrorFlag)
				{
					WaitForSleep(100);
					Result = MyDeviceUSPProc(USP_CMD_READ_VAR, 0, 0, 0, USP_COM_TO);
					if (Result != USP_CMD_UPLOAD_VAR)
					{
						gDBG.Trace("%s %u:get var error!\r\n", __FUNCTION__, __LINE__);
						goto ERROR_OUT;
					}
					else
					{
						PostMessage(gSys.mMainWnd, WM_CORE_UPDATE_VAR, WPARAM(0), LPARAM(0));
					}
				}
			}
			else
			{
				PostMessage(gSys.mMainWnd, WM_CORE_UPDATE_VAR, WPARAM(0), LPARAM(0));
			}
		}
	}
	else
	{
		GetAvailableCom(gSys.ComNoList, MAXCOMNO);
		for (i = 0; i < MAXCOMNO; i++)
		{
			if (!gSys.ComNoList[i])
			{
				break;
			}
			gSys.ComCtrl.hCom = OpenComPort(gSys.ComNoList[i], gSys.ComCtrl.SearchBR);
			if (!gSys.ComCtrl.hCom)
			{
				continue;
			}
			if (gSys.ComCtrl.SearchBR != gSys.ComCtrl.CommBR)
			{
				MyDeviceUSPProc(USP_CMD_SET_BR, NULL, 0, gSys.ComCtrl.CommBR, 200);
				CloseHandle(gSys.ComCtrl.hCom);
				gSys.ComCtrl.hCom = NULL;
				gDBG.Trace("%s %u:%u\r\n", __FUNCTION__, __LINE__, gSys.ComCtrl.CommBR);
				gSys.ComCtrl.hCom = OpenComPort(gSys.ComNoList[i], gSys.ComCtrl.CommBR);
				if (!gSys.ComCtrl.hCom)
				{
					continue;
				}
				WaitForSleep(100);
			}
			
			gSys.ComCtrl.ErrorFlag = 0;
			Result = MyDeviceUSPProc(USP_CMD_READ_VERSION, NULL, 0, 0, USP_COM_TO);
			if (Result != USP_CMD_UPLOAD_VERSION)
			{
				CloseHandle(gSys.ComCtrl.hCom);
				gSys.ComCtrl.hCom = NULL;
				continue;
			}

			Result = MyDeviceUSPProc(USP_CMD_RW_PARAM, NULL, 0, 0xff, USP_COM_TO);
			if (Result != USP_CMD_UPLOAD_PARAM)
			{
				CloseHandle(gSys.ComCtrl.hCom);
				gSys.ComCtrl.hCom = NULL;
				continue;
			}

			Result = MyDeviceUSPProc(USP_CMD_READ_VAR, NULL, 0, 0, USP_COM_TO);
			if (Result != USP_CMD_UPLOAD_VAR)
			{
				CloseHandle(gSys.ComCtrl.hCom);
				gSys.ComCtrl.hCom = NULL;
				continue;
			}
			PostMessage(gSys.mMainWnd, WM_CORE_UPDATE_VAR, WPARAM(0), LPARAM(0));
			PostMessage(gSys.mMainWnd, WM_CORE_UPDATE_VERSION, WPARAM(0), LPARAM(0));
			PostMessage(gSys.mMainWnd, WM_CORE_UPDATE_PARAM, WPARAM(0), LPARAM(0));
			gDBG.Trace("%s %u:COM%u connect\r\n", __FUNCTION__, __LINE__, gSys.ComNoList[i]);
		}
		return;
	}
	return;
ERROR_OUT:
	if (gSys.ComCtrl.hCom)
	{
		CloseHandle(gSys.ComCtrl.hCom);
		gSys.ComCtrl.hCom = NULL;
		InitRBuffer(&gSys.OperationList, (uint8_t *)gSys.OperationData, 8, sizeof(OperationReq_Struct));
	}
}

static DWORD WINAPI MyDeviceThread(LPVOID pData)
{
	int32_t iRet;
	COMSTAT  Stat;
	DWORD Error, ReadLen, DummyLen;
	OVERLAPPED Rol;
	uint8_t Buf[DBG_BUF_MAX];
	uint32_t RxLen;
	uint32_t TxLen;
	//底层数据交换线程启动
	gSys.ThreadRun = true;
	gSys.WorkMode = WORK_IDLE;
	while (gSys.ThreadRun)
	{
		switch (gSys.WorkMode)
		{
		case WORK_IDLE:		
			iRet = WaitForEvent(INFINITE);
			if (iRet == EVENT_MODE_SW)
			{
				MyDeviceChangeWorkMode();
			}
			break;
		case WORK_USP_AUTO:
			MyDeviceAutoMode();
			iRet = WaitForEvent(50);
			switch (iRet)
			{
			case EVENT_THREAD_STOP:
				continue;
			case EVENT_MODE_SW:
				MyDeviceChangeWorkMode();
				continue;
			default:
				break;
			}
			break;
		case WORK_NORMAL_COM:
			iRet = WaitForEvent(5);
			switch (iRet)
			{
			case EVENT_THREAD_STOP:
				continue;
			case EVENT_COM_TX:
				TxLen = ReadRBuffer(&gSys.UartTxBuf, Buf, gSys.UartTxBuf.Len);
				WriteCom(gSys.ComCtrl.hCom, Buf, TxLen);
				break;
			case EVENT_MODE_SW:
				MyDeviceChangeWorkMode();
				continue;
			default:
				break;
			}
			RxLen = ReadCom(gSys.ComCtrl.hCom, Buf, DBG_BUF_MAX, &Error);
			if (RxLen)
			{
				WriteRBufferForce(&gSys.UartRxBuf, Buf, RxLen);
				PostMessage(gSys.mMainWnd, WM_COM_RX, WPARAM(0), LPARAM(0));
			}
			else
			{
				if (Error)
				{
					gDBG.Trace("%s %u:%u\r\n", __FUNCTION__, __LINE__, Error);
					gSys.ComCtrl.IsWork = 0;
					MyDeviceChangeWorkMode();
				}
			}
			break;
		default:
			gSys.WorkMode = WORK_IDLE;
			break;
		}


	}
	return 0;
}

void MyDeviceInit(void)
{
	uint8_t i;
	memset(&gSys, 0, sizeof(gSys));
	InitRBuffer(&gSys.OperationList, (uint8_t *)gSys.OperationData, 8, sizeof(OperationReq_Struct));
	InitRBuffer(&gSys.UartTxBuf, (uint8_t *)gSys.UartTxData, DBG_BUF_MAX / 10, 1);
	InitRBuffer(&gSys.UartRxBuf, (uint8_t *)gSys.UartRxData, DBG_BUF_MAX, 1);
	for (i = 0; i < EVENT_NUM_MAX;i++)
	{
		gSys.Event[i] = CreateEvent(NULL , TRUE, FALSE, NULL);
		ResetEvent(gSys.Event[i]);
	}
	gSys.hThread = CreateThread(NULL, 0, MyDeviceThread, NULL, 0, NULL);
	CRC32_CreateTable(gSys.CRC32Table, CRC32_GEN);
}

void MyDeviceSetWnd(HWND hWnd)
{
	gSys.mMainWnd = hWnd;
}

void MyDeviceQuit(void)
{
	SetEvent(gSys.Event[EVENT_THREAD_STOP]);
}

void MyDeviceStopUSPMode(void)
{
	gSys.ComCtrl.IsWork = 0;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceStartUSPMode(uint32_t SearchBR, uint32_t CommBR)
{
	gSys.ComCtrl.IsWork = 1;
	gSys.ComCtrl.ComNo = 0;
	gSys.ComCtrl.Mode = COM_MODE_USP;
	gSys.ComCtrl.SearchBR = SearchBR;
	gSys.ComCtrl.CommBR = CommBR;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceOperationReq(uint32_t ID, uint32_t Param1, uint32_t Param2, uint8_t *pData)
{
	OperationReq_Struct Req;
	Req.ID = ID;
	Req.Param1 = Param1;
	Req.Param2 = Param2;
	Req.pData = pData;
	WriteRBufferForce(&gSys.OperationList, (uint8_t *)&Req, 1);
	SetEvent(gSys.Event[EVENT_OPERATION_REQ]);
}
void MyDeviceStopUartMode(void)
{
	gSys.ComCtrl.IsWork = 0;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceStartUartMode(uint32_t ComNo, uint32_t BR)
{
	gSys.ComCtrl.IsWork = 1;
	gSys.ComCtrl.Mode = COM_MODE_NORMAL;
	gSys.ComCtrl.ComNo = ComNo;
	gSys.ComCtrl.CommBR = BR;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceUartSend(uint8_t *Data, uint32_t Len)
{
	if (gSys.WorkMode == WORK_NORMAL_COM)
	{
		WriteRBufferForce(&gSys.UartTxBuf, Data, Len);
		SetEvent(gSys.Event[EVENT_COM_TX]);
	}
}