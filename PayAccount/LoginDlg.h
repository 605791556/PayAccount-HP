#pragma once
#include "afxwin.h"
#include "..\common\EditSet.h"

#define WM_INITDLG   WM_USER+1
#define WM_CALLBACK WM_USER+2
// CLoginDlg 对话框

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnInitDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strLstLgName;
	void GetUser(Json::Value root);
	void GetLogin(Json::Value root);
	void SendToLogin(CString strName,CString strPass,int show_pass);
	afx_msg void OnBnClickedOk();
	CComboBox m_comboUser;
	CEditSet m_edit_no_chn_pass;
	vector<USER_STU> m_vet;
	afx_msg void OnCbnSelchangeUser();
	afx_msg void OnCbnEditchangeUser();
	afx_msg void OnStnClickedStabtn();
	CButton m_btnLogin;
	CStatic m_staMsg;
};
