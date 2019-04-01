#pragma once
#include "ProjectMngDlg.h"

// CAddProDlg �Ի���

#define  WM_ADDPRO_CALL WM_USER+731

class CAddProDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddProDlg)

public:
	CAddProDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CAddProDlg(CProjectMngDlg* dlg);
	virtual ~CAddProDlg();

// �Ի�������
	enum { IDD = IDD_ADD_PRO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSave();

	void SetModifyInit();
	void SetNotifyWnd(CProjectMngDlg* dlg);
	void SendToJudgePro(CString strProName);
	void SendToAddOrMdfProject(EM_SOCK_CMD type=SOCK_CMD_ADD_PROJECT);
	bool m_bAdd;
	int m_ndex;//��Ϊ�޸�ʱ������m_vet������s
	CProjectMngDlg* m_dlg;
};
