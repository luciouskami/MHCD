// MHCDDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CMHCDDlg 对话框
class CMHCDDlg : public CDialog
{
// 构造
public:
	afx_msg LRESULT OnMyIconNotify(WPARAM wParam, LPARAM lParam);
	CMHCDDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MHCD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox CListMsg;
	afx_msg void OnBnClickedButtonListclear();
	afx_msg void OnBnClickedButtonStart();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBnClickedButtonShowhelp();
	afx_msg void OnBnClickedButtonShowabout();
	afx_msg void OnDestroy();
};
