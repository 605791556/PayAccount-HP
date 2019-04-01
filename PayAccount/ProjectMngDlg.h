#pragma once
#include "afxcmn.h"
#include "ProMngListCtrl.h"
#include "afxdtctl.h"
#include "afxwin.h"

// CProjectMngDlg �Ի���
#define  WM_PROMNG_CALL WM_USER+721

class CProjectMngDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProjectMngDlg)

public:
	CProjectMngDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProjectMngDlg();

// �Ի�������
	enum { IDD = IDD_PRO_MNG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	int m_nItem;
	int m_row,m_col;  //��¼�û�������Ǹ���Ԫ�����ڵ������к�
	CProMngListCtrl m_ListCtrl;
	afx_msg void DelProject();
	afx_msg void MdfProject();
	afx_msg void OnBnClickedBtnAdd();
	virtual BOOL OnInitDialog();
	void SetListValue();
	void SendToGetProject();
	void SendToSaveProNdex();
	void SendToDelProject(int nProID);
	void GetProject(Json::Value root);
	vector<PROJECT_STU> m_vet;
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSavendex();
	CEdit m_EditIndex;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit1();
	void SetSubItemValue();
};
