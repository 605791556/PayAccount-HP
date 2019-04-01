#pragma once
#include "afxwin.h"
#include "SetPayDlg.h"
#include "..\common\EditSet.h"

// CDianYePayDlg 对话框

#define  WM_DYPAY_CALL WM_USER+761

class CDianYePayDlg : public CSetPayDlg
{
	DECLARE_DYNAMIC(CDianYePayDlg)

public:
	CDianYePayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDianYePayDlg();

// 对话框数据
	enum { IDD = IDD_DIANYE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual void UpdateDlg();
	afx_msg void OnBnClickedBtnSave();

	void SendToGetDyPay();
	void GetDyPay(Json::Value root);
	void SendToSaveDyPay(CString strDown,CString strUp);

	CEditSet m_w_down;
	CEditSet m_w_up;
	virtual BOOL OnInitDialog();
};
