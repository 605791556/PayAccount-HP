#pragma once
#include "afxwin.h"
#include "SetPayDlg.h"
#include "..\common\EditSet.h"

// CDaiNumberPayDlg 对话框

#define  WM_JDPAY_CALL WM_USER+741

class CDaiNumberPayDlg : public CSetPayDlg
{
	DECLARE_DYNAMIC(CDaiNumberPayDlg)

public:
	CDaiNumberPayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDaiNumberPayDlg();

// 对话框数据
	enum { IDD = IDD_DAI_BUMBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	virtual void UpdateDlg();
	afx_msg void OnBnClickedBtnSave();
	void SendToGetDaiPay();
	void GetDaiPay(Json::Value root);
	void SendToSaveDaiPay(STU_DAI_PAY stu);
public:
	//师傅+工人
	CEditSet m_edit_a_w;
	CEditSet m_edit_a_2;
	CEditSet m_edit_a_2_5;
	CEditSet m_edit_a_5_9;
	CEditSet m_edit_a_10;
	CEditSet m_edit_a_18;
	//师傅
	CEditSet m_edit_sf_w;
	CEditSet m_edit_sf_2;
	CEditSet m_edit_sf_2_5;
	CEditSet m_edit_sf_5_9;
	CEditSet m_edit_sf_10;
	CEditSet m_edit_sf_18;
	//工人
	CEditSet m_edit_td_w;
	CEditSet m_edit_td_2;
	CEditSet m_edit_td_2_5;
	CEditSet m_edit_td_5_9;
	CEditSet m_edit_td_10;
	CEditSet m_edit_td_18;
};
