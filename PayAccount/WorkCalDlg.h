#pragma once
#include "afxcmn.h"
#include "afxdtctl.h"
#include "PayAccountDlg.h"
#include "afxwin.h"

// CWorkCalDlg 对话框
#define  WM_WORKCAL_MSG WM_USER+504

struct WORKMONEY
{
	CString strStaffID;
	CString strName;
	double  day_money;//按天工资
	double  jj_money;//计件工资
	double  del_money;//扣除工资

	WORKMONEY()
	{
		day_money = 0;
		jj_money = 0;
		del_money = 0;
	}
};

class CWorkCalDlg : public CPayAccountDlg
{
	DECLARE_DYNAMIC(CWorkCalDlg)

public:
	CWorkCalDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWorkCalDlg();

// 对话框数据
	enum { IDD = IDD_WORKCAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	void UpdateDlg();
	void SetListFontSize();
	void InitListCtrl();
	void SetListValue();
	int m_nItem;
	CFont m_font;
	vector<STAFF_STU> m_vet;
	vector<WORKMONEY> m_vWorkCal;
	CListCtrl m_ListCtrl;
	CDateTimeCtrl m_DateCtrl;
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	CStatic m_total;
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	void SendToGetStaff();
	void GetStaff(Json::Value root);
	void SendToGetOnePayList();
	void GetOnePayList(Json::Value root);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_sta2;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
