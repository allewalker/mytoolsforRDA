#include "mydevice.h"
#include "stdafx.h"
SysVar_Struct gSys;

static DWORD WINAPI MyDeviceThread(LPVOID pM)
{

}

void MyDeviceInit(void)
{
	u8 i;
	memset(&gSys, 0, sizeof(gSys));
	InitRBuffer(&gSys.OperationList, (u8 *)gSys.OperationData, 128, sizeof(OperationReq_Struct));
	for (i = 0; i < EVENT_NUM_MAX;i++)
	{
		gSys.Event[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	gSys.hThread = CreateThread(NULL, 0, MyDeviceThread, NULL, 0, NULL);
}

void MyDeviceSetWnd(HWND hWnd)
{
	gSys.mMainWnd = hWnd;
}

void MyDeviceStop(void)
{
	SetEvent(gSys.Event[EVENT_THREAD_STOP]);
}