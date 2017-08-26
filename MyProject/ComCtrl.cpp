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
	mLastByte = 0;
}

CComCtrl::~CComCtrl()
{
}

void CComCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COM_RX_EDIT, mComRXEdit);
	DDX_Control(pDX, IDC_COM_RX_SAVE_BUTTON, mComRXSaveButton);
	DDX_Control(pDX, IDC_COM_SWITCH_BUTTON, mComSwitchButton);
	DDX_Control(pDX, IDC_COM_TX_BUTTON, mComTXButton);
	DDX_Control(pDX, IDC_COM_TX_EDIT, mComTXEdit);
	DDX_Control(pDX, IDC_COM_TX_NL_BUTTON, mComTXNewlineButton);
	DDX_Control(pDX, IDC_USP_COMM_BR_EDIT, mUSPCommBREdit);
	DDX_Control(pDX, IDC_USP_SEARCH_BR_EDIT, mUSPSearchBREdit);
	DDX_Control(pDX, IDC_USP_STOP_BUTTON, mUSPStopButton);
	DDX_Control(pDX, IDC_USP_WORK_BUTTON, mUSPWorkButton);
	DDX_Control(pDX, IDC_COMNO_COMBO, mComNoCombox);
	DDX_Control(pDX, IDC_COM_CLEAR_BUTTON, mComClearButton);
}


BEGIN_MESSAGE_MAP(CComCtrl, CDialogEx)


	ON_BN_CLICKED(IDC_COM_SWITCH_BUTTON, &CComCtrl::OnBnClickedComSwitchButton)
	ON_BN_CLICKED(IDC_USP_WORK_BUTTON, &CComCtrl::OnBnClickedUspWorkButton)
	ON_BN_CLICKED(IDC_USP_STOP_BUTTON, &CComCtrl::OnBnClickedUspStopButton)
	ON_BN_CLICKED(IDC_COM_TX_BUTTON, &CComCtrl::OnBnClickedComTxButton)
	ON_BN_CLICKED(IDC_COM_TX_NL_BUTTON, &CComCtrl::OnBnClickedComTxNlButton)
	ON_BN_CLICKED(IDC_COM_RX_SAVE_BUTTON, &CComCtrl::OnBnClickedComRxSaveButton)
	ON_CBN_DROPDOWN(IDC_COMNO_COMBO, &CComCtrl::OnDropdownComnoCombo)
	ON_CBN_SELCHANGE(IDC_COMNO_COMBO, &CComCtrl::OnSelchangeComnoCombo)
	ON_BN_CLICKED(IDC_COM_CLEAR_BUTTON, &CComCtrl::OnBnClickedComClearButton)
END_MESSAGE_MAP()


// CComCtrl 消息处理程序

void CComCtrl::UpdateProfile()
{
	uint32_t Val;
	CString Str;

	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_SEARCR_BR, 9600);
	Str.Format(_T("%d"), Val);
	mUSPSearchBREdit.SetWindowTextW(Str);

// 	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_START_COM, 1);
// 	Str.Format(_T("%d"), Val);


	Val = ProfileRSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_COMM_BR, 115200);
	Str.Format(_T("%d"), Val);
	mUSPCommBREdit.SetWindowTextW(Str);

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
	SaveProfile();
}


void CComCtrl::SaveProfile()
{
	uint32_t Val;
	if (GetIntFromEdit(&mUSPSearchBREdit, &Val) > 0)
	{
		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_SEARCR_BR, Val);
	}
	 
	if (GetIntFromEdit(&mUSPCommBREdit, &Val) > 0)
	{
		ProfileWSInt(PROFILE_NAME, COM_CTRL_APP, COM_CTRL_COMM_BR, Val);
	}
	
}


//void CComCtrl::OnBnClickedSlipWorkButton()
//{
//	// TODO:  在此添加控件通知处理程序代码
//	uint32_t SearchBR, CommBR;
//	GetIntFromEdit(&mSearchBREdit, &SearchBR);
//	GetIntFromEdit(&mCommBREdit, &CommBR);
//	SLIP_WorkStart(SearchBR, CommBR);
//}


//void CComCtrl::OnBnClickedSlipStopButton()
//{
//	// TODO:  在此添加控件通知处理程序代码
//	SLIP_WorkStop();
//}


void CComCtrl::OnBnClickedComSwitchButton()
{
	uint32_t BR;
	// TODO:  在此添加控件通知处理程序代码
	if (WORK_NORMAL_COM == gSys.WorkMode)
	{
		MyDeviceStopUartMode();
	}
	else
	{
		if (mComNoCombox.GetCurSel() && GetIntFromEdit(&mUSPCommBREdit, &BR))
		{
			MyDeviceStartUartMode(gSys.ComNoList[mComNoCombox.GetCurSel()], BR);
		}
	}
}


BOOL CComCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	OnDropdownComnoCombo();
	mComRXEdit.SetLimitText(UINT32_MAX);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CComCtrl::OnComRxMessage(void)
{
	uint8_t Buf[DBG_BUF_MAX + 4];
	uint8_t Buf1[DBG_BUF_MAX + 4];
	uint16_t Buf2[DBG_BUF_MAX + 4];
	uint32_t Len;
	Len = ReadRBuffer(&gSys.UartRxBuf, Buf, DBG_BUF_MAX);
	mLastByte = ShowReceiveData(Buf, Len, Buf1, Buf2, &mComRXEdit, mLastByte);

}

void CComCtrl::OnComStateMessage(void)
{
	if (WORK_NORMAL_COM == gSys.WorkMode)
	{
		mComSwitchButton.SetWindowTextW(L"关闭串口");
	}
	else
	{
		mComSwitchButton.SetWindowTextW(L"打开串口");
	}
}

void CComCtrl::OnBnClickedUspWorkButton()
{
	// TODO:  在此添加控件通知处理程序代码
	uint32_t SearchBR, CommBR;
	GetIntFromEdit(&mUSPSearchBREdit, &SearchBR);
	GetIntFromEdit(&mUSPCommBREdit, &CommBR);
	MyDeviceStartUSPMode(SearchBR, CommBR);
}


void CComCtrl::OnBnClickedUspStopButton()
{
	// TODO:  在此添加控件通知处理程序代码
	MyDeviceStopUSPMode();
}


void CComCtrl::OnBnClickedComTxButton()
{
	// TODO:  在此添加控件通知处理程序代码
	uint8_t Buf[1024];
	uint32_t TxLen;
	memset(Buf, 0, 1024);
	TxLen = GetStrFromEdit(&mComTXEdit, Buf, 1024);
	MyDeviceUartSend(Buf, TxLen);
}


void CComCtrl::OnBnClickedComTxNlButton()
{
	// TODO:  在此添加控件通知处理程序代码
	uint8_t Buf[1024];
	uint32_t TxLen;
	memset(Buf, 0, 1024);
	TxLen = GetStrFromEdit(&mComTXEdit, Buf, 1020);
	Buf[TxLen] = '\r';
	Buf[TxLen + 1] = '\n';
	MyDeviceUartSend(Buf, TxLen + 2);
}


void CComCtrl::OnBnClickedComRxSaveButton()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CComCtrl::OnDropdownComnoCombo()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str;
	GetAvailableCom(gSys.ComNoList, 256);
	mComNoCombox.ResetContent();
	GetAvailableCom(gSys.ComNoList, MAXCOMNO);
	for (int i = 0; i < MAXCOMNO; i++)
	{
		if (gSys.ComNoList[i])
		{

			str.Format(_T("COM%d"), gSys.ComNoList[i]);
			mComNoCombox.InsertString(i, str);
		}
		else
		{
			break;
		}

	}
	mComNoCombox.SetCurSel(0);
}


void CComCtrl::OnSelchangeComnoCombo()
{
	// TODO:  在此添加控件通知处理程序代码
	if (WORK_NORMAL_COM == gSys.WorkMode)
	{
		OnBnClickedComSwitchButton();
	}
}


void CComCtrl::OnBnClickedComClearButton()
{
	// TODO:  在此添加控件通知处理程序代码
	mComRXEdit.SetWindowTextW(L"");
}
