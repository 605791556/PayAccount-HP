#pragma once
#include "afxwin.h"
#include "PayAccountDlg.h"
#include "afxcmn.h"
#include "Detail_ListCtrl.h"

// CDetailDlg 对话框

#define  WM_DETAIL_CALL WM_USER+781

struct PRO_DETAIL
{
	int nProID;
	int ndex;//listctrl排列顺序，从0开始
	vector<STU_DETAIL> vDetails;
};

class CDetailDlg : public CPayAccountDlg 
{
	DECLARE_DYNAMIC(CDetailDlg)

public:
	CDetailDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDetailDlg();

// 对话框数据
	enum { IDD = IDD_DETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void UpdateDlg();
	void InitListCtrl();
	void SetListValue(CString strDate,int nYs,double fLs, int nNum,int* n,vector<PRO_DETAIL> vts);
	void SetListFontSize();
	vector<BOOK_STU> m_vet;
	vector<PROJECT_STU> m_vPro;
	CComboBox m_combooks;
	CListCtrl m_ListCtrl;
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_comboRk;
	CString m_LastBookID;
	afx_msg void OnCbnSelchangeCombo4();
	void SendToGetBook();
	void SendToGetProject();
	void SendToGetDetails();
	void GetBook(Json::Value root);
	void GetProject(Json::Value root);
	void GetDetails(Json::Value root);
};
