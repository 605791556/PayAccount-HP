#pragma once
#include "afxcmn.h"


// CUserMngDlg 对话框
#define  WM_USERMNG_CALL WM_USER+201

class CUserMngDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserMngDlg)

public:
	CUserMngDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserMngDlg();

// 对话框数据
	enum { IDD = IDD_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	vector<USER_STU> m_vet;
	int m_nItem;

	void SetListCtrlValue();
	void SendToFindUser();
	void SendToDelUser(CString strName);
	void GetUser(Json::Value root);
	afx_msg void OnBnClickedDel();
	afx_msg void OnLvnColumnclickUser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickUser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickUser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAdd();
};
