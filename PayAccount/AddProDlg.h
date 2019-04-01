#pragma once
#include "ProjectMngDlg.h"

// CAddProDlg 对话框

#define  WM_ADDPRO_CALL WM_USER+731

class CAddProDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddProDlg)

public:
	CAddProDlg(CWnd* pParent = NULL);   // 标准构造函数
	CAddProDlg(CProjectMngDlg* dlg);
	virtual ~CAddProDlg();

// 对话框数据
	enum { IDD = IDD_ADD_PRO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
	int m_ndex;//当为修改时，代表m_vet的索引s
	CProjectMngDlg* m_dlg;
};
