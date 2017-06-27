#pragma once
#include <cassert>
#include <strstream>
#include <algorithm>
#include <exception>
#include <iomanip>
#include<iostream>
using namespace std;
#include <windows.h>
#include "CApi.h"


void GetAvailableCom(uint32_t *ComNo, uint32_t MaxNum);
HANDLE OpenComPort(uint32_t Port, uint32_t BaudRate);
bool CloseComPort(volatile HANDLE hCom);
bool TestComPort(uint32_t Port);
bool SetupCom(volatile HANDLE hCom, uint32_t InBuf, uint32_t OutBuf);
bool SetComDCB(volatile HANDLE hCom, uint32_t BaudRate, uint32_t ByteSize, uint32_t Parity, uint32_t StopBits);

//uint32_t ReadCom(volatile HANDLE hCom, uint8_t *pBuf, uint32_t BufLen, DWORD *ErrorCode, u32 To);
uint32_t WriteCom(volatile HANDLE hCom, unsigned char *pBuf, DWORD BufLen);
