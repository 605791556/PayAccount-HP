#pragma once
#include "afxwin.h"
#include "PayAccountDlg.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include <string>

using std::string;

// CMonthCheckDlg 对话框
#define  WM_MONTHCHECK_CALL WM_USER+503
class CMonthCheckDlg : public CPayAccountDlg
{
	DECLARE_DYNAMIC(CMonthCheckDlg)

public:
	CMonthCheckDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMonthCheckDlg();

// 对话框数据
	enum { IDD = IDD_MPAY };

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void UpdateDlg();
	void SetListFontSize();
	CListCtrl m_listCtrl;
	afx_msg void OnPaint();
	CComboBox m_comboYear;
	CComboBox m_comboMonth;
	vector<STAFF_STU> m_vet;
	vector<MONTH_PAY_STAFF> m_vStaffs;
	vector<int>  m_vItem;//高亮选中行
	virtual BOOL OnInitDialog();
	void InitListCtrl();
	afx_msg void OnCbnSelchangeComboYear();
	afx_msg void OnCbnSelchangeComboMonth();
	afx_msg void OnBnClickedBtnUpdate();
	void SendToGetMonthPay();
	void SendToGetStaff();
	void GetStaff(Json::Value root);
	void GetMonthPay(Json::Value root);
	CStatic m_total;
	CFont m_font;
	CStatic m_staw;
	afx_msg void OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdfinditemList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_keywordCtrl;
	afx_msg void OnEnChangeEditKeyword();
};
