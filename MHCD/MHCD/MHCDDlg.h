// MHCDDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CMHCDDlg �Ի���
class CMHCDDlg : public CDialog
{
// ����
public:
	afx_msg LRESULT OnMyIconNotify(WPARAM wParam, LPARAM lParam);
	CMHCDDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MHCD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
