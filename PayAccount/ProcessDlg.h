#pragma once
#include "afxwin.h"
#include "PayAccountDlg.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include <string>

// CProcessDlg �Ի���
class CProcessDlg : public CPayAccountDlg
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProcessDlg();

// �Ի�������
	enum { IDD = IDD_PROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	void SendToGetBook();
	void SendToGetPro();
	void SendToGetProgress(CString strBookID);
	void GetBook(Json::Value root);
	void GetProject(Json::Value root);
	void GetProgress(Json::Value root);
	void SetListFontSize();
	virtual void UpdateDlg();
	void InitListCtrl();
	void SetListCtrlValue();
	CListCtrl m_ListCtrl;
	afx_msg void OnBnClickedBtnUpdate();
	CComboBox m_combo_rk;
	CComboBox m_combo_book;
	vector<BOOK_STU> m_vet;
	vector<PROJECT_STU> m_vPro;
	CString m_LastBookID;
	afx_msg void OnCbnSelchangeComboRk();
	afx_msg void OnCbnSelchangeComboBook();
};
