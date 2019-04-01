#pragma once
#include "..\common\EditSet.h"
// CAddUserDlg �Ի���

#define  WM_ADDUSER_CALL WM_USER+301

class CAddUserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddUserDlg)

public:
	CAddUserDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddUserDlg();

// �Ի�������
	enum { IDD = IDD_ADD_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void SendToJudgeUser(CString strName);
	void SendToAddUser();
	CEditSet m_edit_no_chn_name;
	CEditSet m_edit_no_chn_pass;
	CEditSet m_edit_no_chn_pass2;

};
