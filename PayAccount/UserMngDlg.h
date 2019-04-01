#pragma once
#include "afxcmn.h"


// CUserMngDlg �Ի���
#define  WM_USERMNG_CALL WM_USER+201

class CUserMngDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserMngDlg)

public:
	CUserMngDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUserMngDlg();

// �Ի�������
	enum { IDD = IDD_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
