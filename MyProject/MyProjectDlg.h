
// MyProjectDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMyProjectDlg 对话框
class CMyProjectDlg : public CDialogEx
{
// 构造
public:
	CMyProjectDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYPROJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	uint8_t mLastByte = 0;
	afx_msg void OnClose();
	CButton mMyClearButton;
	CButton mMySaveButton;
	CEdit mMyShowEdit;
	CListCtrl mMyShowList;
	afx_msg void OnBnClickedMyClearButton();
	afx_msg LRESULT OnTraceMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateResetMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateVarMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateParamMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateVersionMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateDLMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnComRxMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnComStateMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnComCtrl();
private:
	CComCtrl mComCtrl;
	CDevUpgrade mDevUpgrade;
	GDTM_DataStruct ShowData;
	void Reset();
public:
	afx_msg void OnUpgradeCtrl();
};
