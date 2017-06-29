#include "stdafx.h"
#include "mydevice.h"
#include "PrintMsg.h"
SysVar_Struct gSys;


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
			gDBG.Trace("%s %d:entry normal com mode %d %d!\r\n", __FUNCTION__, __LINE__, gSys.ComCtrl.ComNo, gSys.ComCtrl.CommBR);
			gSys.ComCtrl.hCom = OpenComPort(gSys.ComCtrl.ComNo, gSys.ComCtrl.CommBR);
			if (gSys.ComCtrl.hCom)
			{
				gSys.WorkMode = WORK_NORMAL_COM;
				PostMessage(gSys.mMainWnd, WM_COM_STATE, WPARAM(0), LPARAM(0));
			}
		}
		else
		{
			gDBG.Trace("%s %d:entry USP mode!\r\n", __FUNCTION__, __LINE__);
			gSys.WorkMode = WORK_USP_AUTO;
			PostMessage(gSys.mMainWnd, WM_COM_STATE, WPARAM(0), LPARAM(0));
		}
	}
}

static s32 WaitForEvent(u32 To)
{
	DWORD Result;
	u32 EventID = 0;
	Result = WaitForMultipleObjects(EVENT_WAIT_NUM, gSys.Event, FALSE, To);
	switch (Result)
	{
	case WAIT_FAILED:
	case WAIT_TIMEOUT:
		return -1;
	default:
		EventID = Result - WAIT_OBJECT_0;
		gDBG.Trace("%s %d:Event ID %d\r\n", __FUNCTION__, __LINE__, EventID);
		ResetEvent(gSys.Event[EventID]);
		if (EventID == EVENT_THREAD_STOP)
		{
			gSys.ThreadRun = false;
		}
		break;
	}
	return EventID;
}

static void MyDeviceAutoMode(void)
{
	
}

static DWORD WINAPI MyDeviceThread(LPVOID pData)
{
	s32 iRet;
	COMSTAT  Stat;
	DWORD Error, ReadLen, DummyLen;
	OVERLAPPED Rol;
	u8 Buf[DBG_BUF_MAX];
	u32 RxLen;
	u32 TxLen;
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
					gDBG.Trace("%s %d:%d\r\n", __FUNCTION__, __LINE__, Error);
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
	u8 i;
	memset(&gSys, 0, sizeof(gSys));
	InitRBuffer(&gSys.OperationList, (u8 *)gSys.OperationData, 128, sizeof(OperationReq_Struct));
	InitRBuffer(&gSys.UartTxBuf, (u8 *)gSys.UartTxData, DBG_BUF_MAX / 10, 1);
	InitRBuffer(&gSys.UartRxBuf, (u8 *)gSys.UartRxData, DBG_BUF_MAX, 1);
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
void MyDeviceStartUSPMode(u32 SearchBR, u32 CommBR)
{
	gSys.ComCtrl.IsWork = 1;
	gSys.ComCtrl.ComNo = 0;
	gSys.ComCtrl.Mode = COM_MODE_USP;
	gSys.ComCtrl.SearchBR = SearchBR;
	gSys.ComCtrl.CommBR = CommBR;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceOperationReq(u32 ID, u32 Param1, u32 Param2, void *pData)
{
	OperationReq_Struct Req;
	Req.ID = ID;
	Req.Param1 = Param1;
	Req.Param2 = Param2;
	Req.pData = pData;
	WriteRBufferForce(&gSys.OperationList, (u8 *)&Req, 1);
	SetEvent(gSys.Event[EVENT_OPERATION_REQ]);
}
void MyDeviceStopUartMode(void)
{
	gSys.ComCtrl.IsWork = 0;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceStartUartMode(u32 ComNo, u32 BR)
{
	gSys.ComCtrl.IsWork = 1;
	gSys.ComCtrl.Mode = COM_MODE_NORMAL;
	gSys.ComCtrl.ComNo = ComNo;
	gSys.ComCtrl.CommBR = BR;
	SetEvent(gSys.Event[EVENT_MODE_SW]);
}
void MyDeviceUartSend(u8 *Data, u32 Len)
{
	if (gSys.WorkMode == WORK_NORMAL_COM)
	{
		WriteRBufferForce(&gSys.UartTxBuf, Data, Len);
		SetEvent(gSys.Event[EVENT_COM_TX]);
	}
}