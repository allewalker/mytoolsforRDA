#pragma once
#include "afxwin.h"


// CComCtrl �Ի���

class CComCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CComCtrl)

public:
	CComCtrl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CComCtrl();

// �Ի�������
	enum { IDD = IDD_COM_CTRL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString BinFilePath;
public:
	uint8_t *DLData;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void UpdateProfile();
	void Stop();
	void SaveProfile();
	virtual BOOL OnInitDialog();

	CEdit mCommBREdit;
	CEdit mSearchBREdit;
	CButton mStopButton;
	CButton mWorkButton;
	afx_msg void OnBnClickedSlipWorkButton();
	afx_msg void OnBnClickedSlipStopButton();
};
