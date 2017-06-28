
// MyProjectDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMyProjectDlg �Ի���
class CMyProjectDlg : public CDialogEx
{
// ����
public:
	CMyProjectDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYPROJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
