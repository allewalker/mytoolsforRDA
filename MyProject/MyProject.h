
// MyProject.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "PrintMsg.h"
#include "usp.h"
#include "mydevice.h"
#include "ComCtrl.h"
#include "DevUpgrade.h"
// CMyProjectApp: 
// �йش����ʵ�֣������ MyProject.cpp
//

class CMyProjectApp : public CWinApp
{
public:
	CMyProjectApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyProjectApp theApp;