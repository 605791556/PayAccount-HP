// PayAccountDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define WM_INITDLG   WM_USER+101
#define WM_SOCKMSG   WM_USER+102
#define WM_MAINCALL WM_USER+103

enum CloseType
{
	Type_LogOff,//注销
	Type_Close,//退出
	Type_Max

};

// CPayAccountDlg 对话框
class CStaffWriteDlg;
class CProcessDlg;
class CDetailDlg;
class CMonthCheckDlg;
class CDayCheckDlg;
class CWorkCalDlg;
class CPayAccountDlg : public CDialog
{
// 构造
public:
	CPayAccountDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PAYACCOUNT_DIALOG };

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
	afx_msg LRESULT OnInitDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBookUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnStaff();
	afx_msg void OnBook();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	void InitTabControl();
	void InitDlg();
	void InitMenu();
public:
	CTabCtrl         m_TabCtrl;
	CStaffWriteDlg*  m_pStaffWriteDlg;
	CProcessDlg*     m_ProcessDlg;
	CDetailDlg*      m_DetailDlg;
	CDayCheckDlg*    m_DPayDlg;
	CMonthCheckDlg*  m_MPayDlg;
	CWorkCalDlg*     m_WorkCalDlg;
	CPayAccountDlg*  m_TabDlgs[5];
	int              m_TabSelIndex;
	CloseType        m_CloseType;
	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void UpdateDlg(){}
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStatic m_top;
	BOOL m_bExit;
	CStatic m_staNet;
	CStatic m_sta_net1;
	NET_STATE m_netState;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
