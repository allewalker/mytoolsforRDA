#include "stdafx.h"
#include "PrintMsg.h"
CPrintMsg gDBG;

CPrintMsg::CPrintMsg()
{
	InitRBuffer(&m_MsgBuffer, m_DataBuf, DBG_BUF_MAX, 1);
	hInSem = CreateSemaphore(NULL, 1, 1, NULL);
	mMainWnd = NULL;
}


CPrintMsg::~CPrintMsg()
{
	ReleaseSemaphore(hInSem, 1, NULL);
}

void CPrintMsg::Trace(const char* format, ...)
{
	unsigned int ret;
	char tmp_buffer[DBG_BUF_MAX] = { 0 };
	va_list ap;
	va_start(ap, format);
	ret = vsnprintf_s(tmp_buffer, DBG_BUF_MAX, format, ap);
	va_end(ap);
	
	if (m_MsgBuffer.Data != m_DataBuf)
	{
		InitRBuffer(&m_MsgBuffer, m_DataBuf, DBG_BUF_MAX, 1);
		hInSem = CreateSemaphore(NULL, 1, 1, NULL);
	}
	WaitForSingleObject(hInSem, INFINITE);
	WriteRBufferForce(&m_MsgBuffer, (uint8_t *)tmp_buffer, ret);
	ReleaseSemaphore(hInSem, 1, NULL);
	if (mMainWnd)
	{
		PostMessage(mMainWnd, WM_TRACE, WPARAM(0), LPARAM(0));
	}
}

void CPrintMsg::HexTrace(uint8_t *Data, uint32_t Len)
{
	unsigned int ret, i;
	char tmp[4];
	
	if (m_MsgBuffer.Data != m_DataBuf)
	{
		InitRBuffer(&m_MsgBuffer, m_DataBuf, DBG_BUF_MAX, 1);
		hInSem = CreateSemaphore(NULL, 1, 1, NULL);
	}
	WaitForSingleObject(hInSem, INFINITE);
	for (i = 0; i < Len; i++)
	{
		ret = sprintf(tmp, "%02x ", Data[i]);
		WriteRBufferForce(&m_MsgBuffer, (uint8_t *)tmp, ret);
	}
	WriteRBufferForce(&m_MsgBuffer, (uint8_t *)"\r\n", 2);

	
	
	ReleaseSemaphore(hInSem, 1, NULL);
	if (mMainWnd)
	{
		PostMessage(mMainWnd, WM_TRACE, WPARAM(0), LPARAM(0));
	}
}

void CPrintMsg::DecTrace(uint8_t *Data, uint32_t Len)
{
	unsigned int ret, i;
	char tmp[5];

	if (m_MsgBuffer.Data != m_DataBuf)
	{
		InitRBuffer(&m_MsgBuffer, m_DataBuf, DBG_BUF_MAX, 1);
		hInSem = CreateSemaphore(NULL, 1, 1, NULL);
	}
	WaitForSingleObject(hInSem, INFINITE);
	for (i = 0; i < Len; i++)
	{
		ret = sprintf(tmp, "%03d ", Data[i]);
		WriteRBufferForce(&m_MsgBuffer, (uint8_t *)tmp, ret);
	}
	WriteRBufferForce(&m_MsgBuffer, (uint8_t *)"\r\n", 2);

	ReleaseSemaphore(hInSem, 1, NULL);
	if (mMainWnd)
	{
		PostMessage(mMainWnd, WM_TRACE, WPARAM(0), LPARAM(0));
	}
}

void CPrintMsg::SetWnd(HWND hWnd)
{
	mMainWnd = hWnd;
}