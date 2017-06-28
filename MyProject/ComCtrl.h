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
	u8 mLastByte;
public:
	uint8_t *DLData;
	void UpdateProfile();
	void Stop();
	void SaveProfile();
	void OnComRxMessage(void);
	void OnComStateMessage(void);
	CEdit mComRXEdit;
	CButton mComRXSaveButton;
	CButton mComSwitchButton;
	CButton mComTXButton;
	CEdit mComTXEdit;
	CButton mComTXNewlineButton;
	afx_msg void OnBnClickedComSwitchButton();
	CEdit mUSPCommBREdit;
	CEdit mUSPSearchBREdit;
	CButton mUSPStopButton;
	CButton mUSPWorkButton;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedUspWorkButton();
	afx_msg void OnBnClickedUspStopButton();
	afx_msg void OnBnClickedComTxButton();
	afx_msg void OnBnClickedComTxNlButton();
	afx_msg void OnBnClickedComRxSaveButton();
	afx_msg void OnDropdownComnoCombo();
	afx_msg void OnSelchangeComnoCombo();
	CComboBox mComNoCombox;
	CButton mComClearButton;
	afx_msg void OnBnClickedComClearButton();
};
