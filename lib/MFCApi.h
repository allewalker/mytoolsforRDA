#ifndef __MFCAPI_H__
#define __MFCAPI_H__
#include <afxwin.h>
#include <afxdlgs.h>
#include <afxsock.h> 
#include "CApi.h"
#define WM_MY_MESSAGE (WM_USER + 100)

void OutputLog(char *logName, char *File, int Line, char *String);
bool OpenFilePath(CString DefaultPath, CString &mFilePathStr);
void ProfileWSInt(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, uint32_t Data);
void ProfileWSString(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, LPWSTR Data);
uint32_t ProfileRSInt(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, uint32_t Default);
void ProfileRSString(LPCWSTR FileName, LPCWSTR APPName, LPCWSTR ItemName, LPCWSTR Default, LPWSTR Data, DWORD Size);
uint8_t ShowReceiveData(uint8_t* pBuf, uint32_t ReceiveLen, uint8_t* pShowBuf, uint16_t *pwShowBuf, CEdit *ShowEdit, uint8_t LastByte);
uint32_t GetIntFromEdit(CEdit *Edit, uint32_t *Int);
uint32_t GetStrFromEdit(CEdit *Edit, void *Str, uint32_t Len);
uint32_t GetFloatFromEdit(CEdit *Edit, double *F);
uint32_t GetHexStrFromEdit(CEdit *Edit, uint8_t *HexStr, uint32_t Len);
uint32_t GetHexFromEdit(CEdit *Edit, uint32_t *Hex);
#endif