#pragma once
#include "..\common\EditSet.h"

#define  WM_HEDE WM_USER+1//自定义消息
#define WM_CALLBACK WM_USER+2
// CLockDlg 对话框

class CLockDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLockDlg)

public:
	CLockDlg(BOOL bLogin=FALSE,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLockDlg();

// 对话框数据
	enum { IDD = IDD_LOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnSetVisible(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	void SetType(bool bLock);
	bool m_bLock;//true:锁定界面 false:其他验证需要
	BOOL m_bLogin;
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	CEditSet m_editSet;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
};
