#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDevUpgrade 对话框

class CDevUpgrade : public CDialogEx
{
	DECLARE_DYNAMIC(CDevUpgrade)

public:
	CDevUpgrade(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDevUpgrade();

// 对话框数据
	enum { IDD = IDD_DEV_UPGRADE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton mDLButton;
	CButton mLoadBinButton;
	CEdit mLoadBinEdit;
	CButton mLoadLODButton;
	CEdit mLoadLODEdit;
	CButton mLoadUpgradeBinButton;
	CEdit mLoadUpgradeBinEdit;
	CEdit mMainVersionEdit;
	CButton mMakeBinButton;
	CButton mMakeUpgradeBinButton;
	CProgressCtrl mProcProgress;
	CButton mResetButton;
	CEdit mUpgradeVersionEdit;

	CString LastFileDir;
	CString LodFileDir;
	CString BinFileDir;
	CString UpgradeBinFileDir;
	void LoadParam();
	void SaveParam();
	void CreateUpgradeFile();
	bool ReadUpgradeFile();
	afx_msg void OnBnClickedLoadLodButton();
	afx_msg void OnBnClickedLoadBinButton();
	afx_msg void OnBnClickedLoadUpgradeBinButton();
	afx_msg void OnBnClickedMakeBinButton();
	afx_msg void OnBnClickedMakeUpgradeBinButton();
	afx_msg void OnBnClickedDlButton();
	afx_msg void OnBnClickedResetButton();
};
