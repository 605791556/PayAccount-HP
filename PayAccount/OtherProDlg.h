#pragma once
#include "afxwin.h"
#include "SetPayDlg.h"
#include "afxcmn.h"
#include "OtherProListCtrl.h"
#include "afxdtctl.h"
#include "..\common\EditSet.h"

#define  WM_OTHERPAY_CALL WM_USER+771

// COtherProDlg 对话框
//其它项目单价设置页面
class COtherProDlg : public CSetPayDlg
{
	DECLARE_DYNAMIC(COtherProDlg)

public:
	COtherProDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COtherProDlg();

// 对话框数据
	enum { IDD = IDD_OTHER_PRO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPro();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void UpdateDlg();
	void SetListCtrlValue();
	void SendToGetProject();
	void SendToGetBook();
	void SendToGetOtherPay(int nID);
	void SendToSetOtherPay(int proID, vector<OTHER_PRO_PAY> vec);
	void SendToSetOtherAllBookPay(int proID,CString strPay);
	void GetProject(Json::Value root);
	void GetBook(Json::Value root);
	void GetOtherPay(Json::Value root);
	void SetSubItemValue();
public:
	int m_row,m_col;  //记录用户点击的那个单元格所在的行与列号
	COtherProListCtrl m_ListCtrl;
	vector<PROJECT_STU> m_vProList;
	vector<BOOK_STU> m_vBookList;
	vector<OTHER_PRO_PAY> m_vOtherPay;
	CComboBox m_ComboProject;
	afx_msg void OnBnClickedButton1();
	CEditSet m_edit;
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEditSet m_payEdit;
	afx_msg void OnBnClickedCheck();
};
