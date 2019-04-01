#pragma once
#include "afxwin.h"
#include "PayAccountDlg.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include <string>
#include "..\common\EditSet.h"

// CStaffWriteDlg 对话框

class CStaffWriteDlg : public CPayAccountDlg
{
	DECLARE_DYNAMIC(CStaffWriteDlg)

public:
	CStaffWriteDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStaffWriteDlg();

// 对话框数据
	enum { IDD = IDD_STAFF_WRITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEditSet m_number;
	CComboBox m_combo_book;
	CComboBox m_combo_pro;
	vector<BOOK_STU> m_vet;
	vector<PROJECT_STU> m_vPro;
	afx_msg void OnBnClickedOk();
	virtual void UpdateDlg();
	virtual BOOL OnInitDialog();
	void SetListFontSize();
	void SendToGetBook();
	void SendToGetPro();
	void SendToGetProgress(CString strBookID);
	void SendToGetStaffWrite();
	void SendToSaveStaffWrite(CString strBookID,int proID,double number);
	void GetProgress(Json::Value root);
	void GetProject(Json::Value root);
	void GetBook(Json::Value root);
	void GetStaffWrite(Json::Value root);
	
	void InitListCtrl();
	void SetListCtrlValue();
	void SetComboBookValue();
	void SetComboProValue();
	CListCtrl m_ListCtrl;
	CString m_LastBookID;
	int m_LastProID;
	CFont m_font;
	HANDLE           m_Handle;
	BOOL m_bExit;
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnCbnSelchangeComboBook();
	afx_msg void OnCbnSelchangeComboPro();
	CButton m_btnsave;
};
