#pragma once
#include "..\common\EditSet.h"

// CMdfPwdDlg 对话框
#define  WM_MDFPWD_CALL WM_USER+401

class CMdfPwdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMdfPwdDlg)

public:
	CMdfPwdDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMdfPwdDlg();

// 对话框数据
	enum { IDD = IDD_MDFPWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void SendToMdfPwd(CString strName,CString strPwd);
public:
	CEditSet m_edit_old_pass;
	CEditSet m_edit_new_pass;
	CEditSet m_edit_new_pass2;
};
