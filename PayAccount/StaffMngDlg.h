#pragma once
#include "afxcmn.h"
#include "..\common\globle.h"
#include "afxwin.h"


// CStaffMngDlg 对话框
#define  WM_STAFFMNG_CALL WM_USER+501

class CStaffMngDlg : public CDialog
{
	DECLARE_DYNAMIC(CStaffMngDlg)

public:
	CStaffMngDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStaffMngDlg();

// 对话框数据
	enum { IDD = IDD_STAFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	int m_nItem;
	int m_nCount;//数据总数
	int m_dex;//当前页数
	int m_nPage;//总页数
	CString m_strKeyWord;

	CListCtrl m_ListCtrl;
	vector<STAFF_STU> m_vet;
	void SendToGetStaff(CString strKeyWord,int nStart,int nNum);
	void SendToDelStaff(CString strStaffID);
	void GetStaff(Json::Value root);
	afx_msg void OnBnClickedStaffAdd();
	void SetListCtrlValue();
	afx_msg void DelStaff();
	afx_msg void MdfStaff();
	afx_msg void OnBnClickedStaffFind();
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnGo();
	afx_msg void OnBnClickedBtnLast();
	afx_msg void OnBnClickedBtnNext();
	CStatic m_sta_all;
	CStatic m_sta_go;
	CEdit m_edit_go;
	CButton m_btn_go;
	CButton m_BtnLast;
	CButton m_BtnNext;
};
