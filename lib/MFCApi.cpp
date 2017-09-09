#include "stdafx.h"
#include "MFCApi.h"

void OutputLog(char *logName, char *File, int Line, char *String)
{
	char buf1[40960];
	wchar_t wbuf[40960];
	wchar_t logNamewbuf[128];
	CString Msg, logNameStr;
	sprintf_s(buf1, 40960,  "%s %d:%s", File, Line, String);
	MultiByteToWideChar(CP_ACP, 0, (char *)buf1, strlen(buf1) + 1, wbuf, strlen(buf1) + 1);
	Msg.Format(_T("%s"), wbuf);

	MultiByteToWideChar(CP_ACP, 0, (char *)logName, strlen(logName) + 1, logNamewbuf, strlen(logName) + 1);
	logNameStr.Format(_T("%s"), logNamewbuf);
	try
	{
		//设置文件的打开参数
		CStdioFile outFile(logNameStr, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		CString msLine;
		CTime tt = CTime::GetCurrentTime();

		//作为Log文件，经常要给每条Log打时间戳，时间格式可自由定义， 
		msLine = tt.Format("[%Y-%m-%d %H:%M:%S] ") + Msg;
		msLine += L"\r\n";

		//在文件末尾插入新纪录
		outFile.SeekToEnd();

		outFile.WriteString(msLine);
		outFile.Close();
	}
	catch (CFileException *fx)
	{
		fx->Delete();
	}

}

void ProfileRSString(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, LPCWSTR Default, LPWSTR Data, DWORD Size)
{
	WCHAR CurrentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CurrentPath);
	CString strFileName(CurrentPath);
	strFileName.Append(L"\\");
	strFileName.Append(FileName);
	::GetPrivateProfileString(APPName, ItemName, Default, Data, Size, strFileName);
	::WritePrivateProfileString(APPName, ItemName, Data, strFileName);
}

uint32_t ProfileRSInt(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, uint32_t Default)
{
	uint32_t Res;
	CString Data;
	WCHAR CurrentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CurrentPath);
	CString strFileName(CurrentPath);
	strFileName.Append(L"\\");
	strFileName.Append(FileName);
	Res = ::GetPrivateProfileInt(APPName, ItemName, Default, strFileName);
	Data.Format(_T("%d"), Res);
	::WritePrivateProfileString(APPName, ItemName, Data, strFileName);
	return Res;
}

void ProfileWSString(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, LPWSTR Data)
{
	WCHAR CurrentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CurrentPath);
	CString strFileName(CurrentPath);
	strFileName.Append(L"\\");
	strFileName.Append(FileName);
	::WritePrivateProfileString(APPName, ItemName, Data, strFileName);
}
void ProfileWSInt(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, uint32_t Data)
{
	CString Str;
	WCHAR CurrentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CurrentPath);
	CString strFileName(CurrentPath);
	strFileName.Append(L"\\");
	strFileName.Append(FileName);
	Str.Format(_T("%d"), Data);
	::WritePrivateProfileString(APPName, ItemName, Str, strFileName);
}

bool OpenFilePath(CString DefaultPath, CString &mFilePathStr)
{
	CFileDialog dlgFile(TRUE, NULL, NULL, 6, L"Lod Files(*.lod)|*.lod|BIN Files(*.bin)|*.bin|All Files(*.*)|*.*|");
	CString fileName;
	TCHAR CurrentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CurrentPath);
	const int c_cMaxFiles = 100;
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;

	if (DefaultPath.GetLength())
	{
		dlgFile.GetOFN().lpstrInitialDir = DefaultPath;
	}
	else
	{

		dlgFile.GetOFN().lpstrInitialDir = CurrentPath;
	}
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(c_cbBuffSize);
	dlgFile.GetOFN().nMaxFile = c_cbBuffSize;
	fileName.ReleaseBuffer(c_cbBuffSize);
	if (dlgFile.DoModal() == IDOK)//是否打开成功
	{
		if (fileName.GetAllocLength() != 0)//不能用GetLength
		{
			HANDLE hFile = CreateFile((LPCTSTR)fileName,
				GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				int Error = GetLastError();
				AfxMessageBox(L"所选文件打开失败");
				return false;
			}
			else
			{
				CloseHandle(hFile);
				mFilePathStr = fileName;
				return true;
			}
		}

	}
	else
	{

	}
	return false;
}

/********************************************************************************/
/*从EDIT中获取数据的常用函数													*/
/********************************************************************************/

uint32_t GetIntFromEdit(CEdit *Edit, uint32_t *Int)
{
	CString strTemp;
	Edit->GetWindowTextW(strTemp);
	if (strTemp.GetLength() == 0)
	{
		*Int = 0;
		return 0;
	}
	else
	{
		*Int = _ttoi(strTemp);

	}
	return 1;
}
uint32_t GetStrFromEdit(CEdit *Edit, void *Str, uint32_t Len)
{
	CString strTemp;
	Edit->GetWindowTextW(strTemp);
	uint32_t Size = strTemp.GetLength();
	Len = (Len > Size) ? Size : Len;
	unsigned char *cmd = new unsigned char[Len + 2];
	memset(cmd, 0, Len + 2);
	WideCharToMultiByte(CP_ACP, 0, strTemp, Len + 1, (char *)cmd, Len + 1, NULL, false);
	//TRACE("%d %s\r\n", Len, cmd);
	memcpy(Str, cmd, Len);
	delete[] cmd;
	return Len;
}

uint32_t GetFloatFromEdit(CEdit *Edit, double *F)
{
	CString strTemp;
	Edit->GetWindowTextW(strTemp);
	if (strTemp.GetLength() == 0)
	{
		return 0;
	}
	else
	{
		*F = _ttof(strTemp);
	}
	return 1;
}

uint32_t GetHexStrFromEdit(CEdit *Edit, uint8_t *HexStr, uint32_t Len)
{
	CString Str;
	uint32_t Result;
	Len = Len * 2;
	Edit->GetWindowText(Str);
	uint8_t *Temp = new uint8_t[0, Str.GetLength() * 2 + 2];
	memset(Temp, 0, Str.GetLength() * 2 + 2);
	UnicodeToAsciiN((uint16_t *)Str.GetBuffer(), Temp, Str.GetLength() * 2);
	Len = (Len > Str.GetLength()) ? Str.GetLength() : Len;

	Str.ReleaseBuffer();

	Result = AsciiToHex(Temp, Len, HexStr);
	delete[] Temp;
	return Result;
}
uint32_t GetHexFromEdit(CEdit *Edit, uint32_t *Hex)
{

	CString Str;

	Edit->GetWindowText(Str);

	if (Str.GetLength() == 0)
	{
		return 0;
	}
	uint8_t *Temp = new uint8_t[0, Str.GetLength() * 2 + 2];
	memset(Temp, 0, Str.GetLength() * 2 + 2);
	UnicodeToAsciiN((uint16_t *)Str.GetBuffer(), Temp, Str.GetLength() * 2);
	Str.ReleaseBuffer();
	*Hex = StrToUint(Temp);
	delete[] Temp;
	return 1;
}

uint8_t ShowReceiveData(uint8_t* pBuf, uint32_t ReceiveLen, uint8_t* pShowBuf, uint16_t *pwShowBuf, CEdit *ShowEdit, uint8_t LastByte)
{

	int ReadLen = 0, DisplayLen = 0;
	int i;
	unsigned char DetectState = 0;//0 char 1 half word 2 one word
	if (LastByte)
	{
		pShowBuf[0] = LastByte;
		LastByte = 0;
		memcpy(pShowBuf + 1, pBuf, ReceiveLen);
		ReadLen = ReceiveLen + 1;
	}
	else
	{
		memcpy(pShowBuf, pBuf, ReceiveLen);
		ReadLen = ReceiveLen;
	}
	pShowBuf[ReadLen] = 0;
	//检索末尾是否有半个汉字
	for (i = 0; i < ReadLen; i++)
	{
		if ((pShowBuf[i] & 0x80) != 0x80)
		{
			DetectState = 0;
		}
		else
		{
			if (DetectState == 2)
			{
				DetectState = 1;
			}
			else
			{
				DetectState++;
			}
		}
	}

	DisplayLen = ReadLen;
	LastByte = 0;

	if (DetectState == 1)
	{
		LastByte = pShowBuf[ReadLen - 1];
		DisplayLen = ReadLen - 1;
	}

	pShowBuf[DisplayLen] = 0;

	//计算多字节字符的大小，按字符计算。
	int len = MultiByteToWideChar(CP_ACP, 0, (char *)pShowBuf, DisplayLen, NULL, 0);

	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, (char *)pShowBuf, DisplayLen, (LPWSTR)pwShowBuf, len);

	pwShowBuf[len] = 0;  //添加字符串结尾，注意不是len+1
	int   nLen = ShowEdit->GetWindowTextLength();//获取控件中内容的大小
	ShowEdit->SetSel(nLen, nLen);//将光标位置跳刀最后一个字符
	ShowEdit->ReplaceSel((LPWSTR)pwShowBuf); //继续写入内容
	return LastByte;
}