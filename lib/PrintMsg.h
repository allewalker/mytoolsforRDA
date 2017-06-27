#pragma once
#include "MFCApi.h"
#include "config.h"
#define WM_TRACE (WM_MY_MESSAGE + 0)
//#define DBG(x,y) do {char temp[256];int ret;ret = sprintf(temp, "%s %d:"x,__FUNCTION__, __LINE__, ##y); gPrintMsg.SetMsg((uint8_t *)temp, ret);  } while (0)
class CPrintMsg
{
private:
	UINT8 m_DataBuf[DBG_BUF_MAX];
	HWND mMainWnd = NULL;
	
public:
	CPrintMsg();
	~CPrintMsg();	
	RBuffer m_MsgBuffer;
	
	HANDLE hInSem = NULL;
	void Trace(const char* format, ...);
	void HexTrace(u8 *Data, u32 Len);
	void DecTrace(u8 *Data, u32 Len);
	void SetWnd(HWND);
};

extern CPrintMsg gDBG;
