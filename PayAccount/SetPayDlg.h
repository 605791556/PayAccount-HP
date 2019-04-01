#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSetPayDlg �Ի���

class CDaiNumberPayDlg;
class CZheYeDlg;
class COtherProDlg;
class CDianYePayDlg;
class CSetPayDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetPayDlg)

public:
	CSetPayDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetPayDlg();

// �Ի�������
	enum { IDD = IDD_SETPAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

public:
	void InitTabCtrl();
	virtual void UpdateDlg();

public:
	CTabCtrl m_tabCtrl;
	CDaiNumberPayDlg* m_pDaiNumPayDlg;
	CZheYeDlg*        m_pZheYeDlg;
	CDianYePayDlg*    m_pDianYePayDlg;
	COtherProDlg*     m_pOtherProDlg;
	CSetPayDlg* m_TabDlgs[4];
	int             m_TabSelIndex;
	
};
