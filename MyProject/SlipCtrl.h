#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSlipCtrl 对话框

class CSlipCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CSlipCtrl)

public:
	CSlipCtrl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSlipCtrl();

// 对话框数据
	enum { IDD = IDD_SLIP_CTRL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	CButton mSlipDLButton;
	CButton mSlipLoadBinButton;
	CEdit mSlipLoadBinEdit;
	CButton mSlipLoadLodButton;
	CEdit mSlipLoadLodEdit;
	CButton mSlipLoadUpdateBinButton;
	CEdit mSlipLoadUpdateBinEdit;
	CButton mSlipMakeBinButton;
	CButton mSlipMakeUpdateBinButton;
	CEdit mSlipUpDateVersionEdit;
	CString LastFileDir;
	CString LodFileDir;
	CString BinFileDir;
	CString UpdateBinFileDir;
	void LoadParam();
	void SaveParam();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSlipLoadLodButton();
	afx_msg void OnBnClickedSlipLoadBinButton();
	afx_msg void OnBnClickedSlipLoadUpdateBinButton();
	CEdit mSlipMainVersionEdit;
	afx_msg void OnBnClickedSlipMakeBinButton();
	void CreateUpdateFile();
	CProgressCtrl mSlipProcProgress;
	afx_msg void OnBnClickedSlipDlButton();
	bool ReadUpdateFile();
	CButton mSlipResetButton;
	afx_msg void OnBnClickedSlipResetButton();
	afx_msg void OnBnClickedSlipMakeUpdateBinButton();
};
