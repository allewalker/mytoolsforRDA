// ComCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "MyProject.h"
#include "ComCtrl.h"
#include "afxdialogex.h"

#define GET_VAR_TIMER 1

#define COM_CTRL_APP L"CommParam"
#define COM_CTRL_SEARCR_BR L"searchbaudrate"
#define COM_CTRL_COMM_BR L"commbaudrate"
#define COM_CTRL_START_COM L"startcom"
#define COM_CTRL_SEARCH_FRQ L"searchfrq"
#define COM_CTRL_VAR_FRQ L"varfrq"
#define COM_CTRL_PARAM_FRQ L"paramfrq"
#define COM_CTRL_TRACE_FRQ L"tracefrq"
#define COM_CTRL_VER_FRQ L"versionfrq"
#define COM_CTRL_DL_PATH L"dlpath"
// CComCtrl 对话框

IMPLEMENT_DYNAMIC(CComCtrl, CDialogEx)

CComCtrl::CComCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CComCtrl::IDD, pParent)
{
	DLData = new uint8_t[1024 * 1024];
}

CComCtrl::~CComCtrl()
{
}

void CComCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i;
	DDX_Control(pDX, IDC_SLIP_COMM_BR_EDIT, mCommBREdit);
	DDX_Control(pDX, IDC_SLIP_SEARCH_BR_EDIT, mSearchBREdit);
	DDX_Control(pDX, IDC_SLIP_STOP_BUTTON, mStopButton);
	DDX_Control(pDX, IDC_SLIP_WORK_BUTTON, mWorkButton);
}


BEGIN_MESSAGE_MAP(CComCtrl, CDialogEx)

	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_SLIP_WORK_BUTTON, &CComCtrl::OnBnClickedSlipWorkButton)
	ON_BN_CLICKED(IDC_SLIP_STOP_BUTTON, &CComCtrl::OnBnClickedSlipStopButton)
END_MESSAGE_MAP()


// CComCtrl 消息处理程序

void CComCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	uint32_t GetVarFrq;
	switch (nIDEvent)
	{

	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CComCtrl::UpdateProfile()
{
	uint32_t Val;
	CString Str;

	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_SEARCR_BR, 9600);
	Str.Format(_T("%d"), Val);
	mSearchBREdit.SetWindowTextW(Str);

// 	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_START_COM, 1);
// 	Str.Format(_T("%d"), Val);


	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_COMM_BR, 115200);
	Str.Format(_T("%d"), Val);
	mCommBREdit.SetWindowTextW(Str);

// 	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_SEARCH_FRQ, 200);
// 	Str.Format(_T("%d"), Val);
// 
// 
// 	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_VAR_FRQ, 500);
// 	Str.Format(_T("%d"), Val);


	ProfileRSString(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_DL_PATH, L"", BinFilePath.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);

}

void CComCtrl::Stop()
{

	SLIP_WorkStop();
	SaveProfile();
	delete[] DLData;
}


void CComCtrl::SaveProfile()
{
	uint32_t Val;
	if (GetIntFromEdit(&mSearchBREdit, &Val) > 0)
	{
		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_SEARCR_BR, Val);
	}
	
// 	if ( GetIntFromEdit(&mComNoEdit, &Val) > 0 )
// 	{
// 		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_START_COM, Val);
// 	}
	 
	if (GetIntFromEdit(&mCommBREdit, &Val) > 0)
	{
		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_COMM_BR, Val);
	}
	
// 	if (GetIntFromEdit(&mComSearchFrqEdit, &Val) > 0)
// 	{
// 		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_SEARCH_FRQ, Val);
// 	}
// 	
// 	if (GetIntFromEdit(&mComVarFrqEdit, &Val) > 0)
// 	{
// 		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_VAR_FRQ, Val);
// 	}	
}

BOOL CComCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int i;
	CString Str;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CComCtrl::OnBnClickedSlipWorkButton()
{
	// TODO:  在此添加控件通知处理程序代码
	uint32_t SearchBR, CommBR;
	GetIntFromEdit(&mSearchBREdit, &SearchBR);
	GetIntFromEdit(&mCommBREdit, &CommBR);
	SLIP_WorkStart(SearchBR, CommBR);
}


void CComCtrl::OnBnClickedSlipStopButton()
{
	// TODO:  在此添加控件通知处理程序代码
	SLIP_WorkStop();
}
