#pragma once
#include "..\common\EditSet.h"

// CMdfPwdDlg �Ի���
#define  WM_MDFPWD_CALL WM_USER+401

class CMdfPwdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMdfPwdDlg)

public:
	CMdfPwdDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMdfPwdDlg();

// �Ի�������
	enum { IDD = IDD_MDFPWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
