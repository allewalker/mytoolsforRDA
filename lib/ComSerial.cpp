#include "stdafx.h"
#include "ComSerial.h"

uint32_t ComPort;//当前打开的串口序号
uint32_t ComBaudRate;



/////用读注册表的方式查看系统的串口设备
void GetAvailableCom(uint32_t *ComNo, uint32_t MaxNum)
{
	HKEY  hKey;
	LONG  ret;
	OSVERSIONINFO    osvi;
	BOOL  bOsVersionInfoEx;
	CString  keyinfo;
	unsigned char ComNameW[40];
	unsigned char ComNameA[40];
	char ValueName[256];
	char szNO[3];
	uint32_t  i = 0, j = 0, k = 0;
	DWORD  sType, Reserved, cbData, cbValueName;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	ZeroMemory(ComNo, sizeof(ComNo));
	keyinfo.Format(_T("HARDWARE\\DEVICEMAP\\SERIALCOMM"));
	//CString  keyinfo=_T("HARDWARE\\DEVICEMAP\\SERIALCOMM");
	i = 0;
	sType = REG_SZ;
	Reserved = 0;
	bOsVersionInfoEx = GetVersionEx(&osvi);
	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyinfo, 0, KEY_ALL_ACCESS, &hKey);
	if (ret == ERROR_SUCCESS)
	{
		if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) //WINNT XP 2000
		{
			do
			{
				cbData = 40; cbValueName = 40;
				memset(ComNameW, 0, 40);
				memset(ValueName, 0, 256);
				memset(szNO, 0, 3);
				ret = RegEnumValue(hKey, i, (LPWSTR)ValueName, &cbValueName, NULL, &sType, ComNameW, &cbData);
				if (ret == ERROR_SUCCESS)
				{
					k = 0;
					WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ComNameW, 40, (LPSTR)ComNameA, 40, NULL, false);
					ComNo[i] = atoi((const char *)&ComNameA[3]);
					i++;
					if (i >= MaxNum)
					{
						break;
					}
				}
			} while (ret == ERROR_SUCCESS);
		}
	}
	//串口排序
	for (i = 0; i < MaxNum - 1; i++)
	{
		if (ComNo[i] == 0)
		{
			break;
		}
		for (j = i + 1; j < MaxNum; j++)
		{
			if (ComNo[j] == 0)
			{
				break;
			}
			else if (ComNo[j] <= ComNo[i])
			{
				k = ComNo[j];
				ComNo[j] = ComNo[i];
				ComNo[i] = k;
			}
		}
	}
	RegCloseKey(hKey);
}

bool SetupCom(volatile HANDLE hCom, uint32_t InBuf, uint32_t OutBuf)
{
	COMMTIMEOUTS ComTimeOut;

	if (!SetupComm(hCom, InBuf, OutBuf))
		return false; //设置推荐缓冲区

	if (!GetCommTimeouts(hCom, &ComTimeOut))
		return false;
	memset(&ComTimeOut, 0, sizeof(ComTimeOut));
	ComTimeOut.ReadIntervalTimeout = -1;
	ComTimeOut.ReadTotalTimeoutMultiplier = 0;
	ComTimeOut.ReadTotalTimeoutConstant = 0;
	ComTimeOut.WriteTotalTimeoutMultiplier = 0;
	ComTimeOut.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(hCom, &ComTimeOut))
		return false; //设置超时时间

	if (!PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
		return false; //清空串口缓冲区
	return true;
}

//设置内置结构串口参数：波特率，停止位
bool SetComDCB(volatile HANDLE hCom, uint32_t BaudRate, uint32_t ByteSize, uint32_t Parity, uint32_t StopBits)
{
	DCB ComDcb;
	memset(&ComDcb, 0, sizeof(ComDcb));
	ComDcb.DCBlength = sizeof(ComDcb);

	if (!GetCommState(hCom, &ComDcb))
		return false;
	ComDcb.BaudRate = BaudRate;
	ComDcb.ByteSize = ByteSize;
	ComDcb.Parity = Parity;
	ComDcb.StopBits = StopBits;

	ComDcb.wReserved = 0;                       //没有使用，必须为0  
	ComDcb.XonLim = 40960;                          //指定在XOFF字符发送之前接收到缓冲区中可允许的最小字节数
	ComDcb.XoffLim = 10240;                         //指定在XOFF字符发送之前缓冲区中可允许的最小可用字节数
	ComDcb.XonChar = 0;                         //发送和接收的XON字符 
	ComDcb.XoffChar = 0;                        //发送和接收的XOFF字符
	ComDcb.ErrorChar = 0;                       //代替接收到奇偶校验错误的字符 
	ComDcb.EofChar = 0;                         //用来表示数据的结束  
	ComDcb.EvtChar = 0;                         //事件字符，接收到此字符时，会产生一个事件  
	ComDcb.wReserved1 = 0;                      //没有使用

	ComDcb.fBinary = 1;
	ComDcb.fOutxCtsFlow = 0;
	ComDcb.fOutxDsrFlow = 0;
	ComDcb.fDtrControl = 0;
	ComDcb.fDsrSensitivity = 0;
	ComDcb.fInX = 0;
	ComDcb.fNull = 0;
	ComDcb.fDsrSensitivity = 0;
	ComDcb.fTXContinueOnXoff = 0;
	ComDcb.fOutX = 0;
	ComDcb.fErrorChar = 0;
	ComDcb.fRtsControl = 0;
	ComDcb.fAbortOnError = 0;

	return SetCommState(hCom, &ComDcb) == TRUE;

}

bool TestComPort(uint32_t Port)
{
	CString ComName;
	char ComTestStr[20];
	sprintf_s(ComTestStr, "\\\\.\\COM%d", Port);
	ComName = ComTestStr;
	DWORD Error;
	HANDLE hComTest = CreateFile(
		ComName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	Error = GetLastError();
	if (hComTest != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hComTest);
		return true;
	}
	else
	{
		return false;
	}
}

HANDLE OpenComPort(uint32_t Port, uint32_t BaudRate)
{
	volatile HANDLE hCom;
	CString ComName;

	char ComTestStr[20];
	sprintf_s(ComTestStr, "\\\\.\\COM%d", Port);
	ComName = ComTestStr;
	hCom = CreateFile(
		ComName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //重叠I/O
		//		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	if (SetupCom(hCom, 81920, 81920))
	{
		if (SetComDCB(hCom, BaudRate, 8, NOPARITY, ONESTOPBIT))
		{
		} 
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
	return hCom;
}

bool CloseComPort(volatile HANDLE hCom)
{
	DWORD Error, Mask, Length;
	if (hCom == NULL)
	{
		return true;
	}
	SetCommMask(hCom, 0);
	EscapeCommFunction(hCom, CLRDTR);
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	CloseHandle(hCom);
	return true;
}


uint32_t WriteCom(volatile HANDLE hCom, unsigned char *pBuf, DWORD BufLen)
{
	
	OVERLAPPED Wol;          //写操作OVERLAPPED结构变量
	memset(&Wol, 0, sizeof(Wol));
	Wol.hEvent = CreateEvent(NULL, true, false, NULL);
	DWORD    Error;
	if (ClearCommError(hCom, &Error, NULL) && Error > 0) //清除错误
		PurgeComm(hCom, PURGE_TXABORT | PURGE_TXCLEAR);

	unsigned long WriteLen = 0;
	if (!WriteFile(hCom, pBuf, BufLen, &WriteLen, &Wol))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			TRACE("!\r\n");
			WriteLen = 0;
		}
	}
	return WriteLen;
}

// uint32_t ReadCom(volatile HANDLE hCom, uint8_t *pBuf, uint32_t BufLen, DWORD *ErrorCode, u32 To)
// {
// 	uint32_t Cnt = 0;
// 	COMSTAT  Stat;
// 	DWORD Error, ReadLen, DummyLen, RolLen = 0;
// 	DWORD dwMsecond = 0;
// 	LARGE_INTEGER cpuHz, startCnt, targetCnt;
// 	cpuHz.QuadPart = 0;
// 	startCnt.QuadPart = 0;
// 	targetCnt.QuadPart = 0;
// 	QueryPerformanceFrequency(&cpuHz);
// 	QueryPerformanceCounter(&startCnt);
// 	*ErrorCode = 0;
// 	do
// 	{
// 		ClearCommError(hCom, &Error, &Stat);
// 		if (Error)
// 		{
// 			*ErrorCode = GetLastError();
// 			return 0;
// 		}
// 		if (Stat.cbInQue)
// 		{
// 			break;
// 		}
// 
// 		QueryPerformanceCounter(&targetCnt);
// 		dwMsecond = (DWORD)((targetCnt.QuadPart - startCnt.QuadPart) / cpuHz.QuadPart * 1000);
// 	} while (dwMsecond < To);
// 
// 	if (dwMsecond >= To)
// 	{
// 		return 0;
// 	}
// 	ReadLen = (BufLen <= Stat.cbInQue) ? BufLen : Stat.cbInQue;
// 	OVERLAPPED Rol;
// 	memset(&Rol, 0, sizeof(Rol));
// 	ReadFile(hCom, pBuf, ReadLen, &DummyLen, &Rol);
// 
// 	Error = GetLastError();
// 	if (Error == ERROR_IO_PENDING)
// 	{
// 		GetOverlappedResult(hCom, &Rol, &RolLen, TRUE);
// 		// GetOverlappedResult函数的最后一个参数设为TRUE，
// 		//函数会一直等待，直到读操作完成或由于错误而返回。
// 		Error = GetLastError();
// 		if (Error)
// 		{
// 			TRACE("Error = %d\r\n", Error);
// 			*ErrorCode = Error;
// 			return 0;
// 		}
// 		return RolLen;
// 	}
// 	else if (Error)
// 	{
// 		TRACE("Error = %d\r\n", Error);
// 		*ErrorCode = Error;
// 		return 0;
// 	}
// 	else
// 	{
// 		return DummyLen;
// 	}
// }

