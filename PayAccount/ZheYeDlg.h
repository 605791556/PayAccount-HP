#pragma once
#include "afxwin.h"
#include "SetPayDlg.h"
#include "..\common\EditSet.h"

// CZheYeDlg 对话框

#define  WM_ZHEYEPAY_CALL WM_USER+751

class CZheYeDlg : public CSetPayDlg
{
	DECLARE_DYNAMIC(CZheYeDlg)

public:
	CZheYeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CZheYeDlg();

// 对话框数据
	enum { IDD = IDD_ZHEYE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	virtual void UpdateDlg();
	afx_msg void OnBnClickedBtnSave();
	void SendToGetZheyePay();
	void GetZheyePay(Json::Value root);
	void SendToSaveZheyePay(STU_ZHEYE_PAY stu);
public:
	//师傅+工人
	CEditSet m_a_q4;
	CEditSet m_a_4;
	CEditSet m_a_d3;
	CEditSet m_a_3;
	CEditSet m_a_2;
	//师傅
	CEditSet m_sf_q4;
	CEditSet m_sf_4;
	CEditSet m_sf_d3;
	CEditSet m_sf_3;
	CEditSet m_sf_2;
	//工人
	CEditSet m_td_q4;
	CEditSet m_td_4;
	CEditSet m_td_d3;
	CEditSet m_td_3;
	CEditSet m_td_2;
};
