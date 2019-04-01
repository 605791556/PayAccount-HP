#pragma once
#include "BookMngDlg.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "..\common\EditSet.h"

// CAddBookDlg 对话框
#define  WM_ADDBOOK_CALL WM_USER+801

class CAddBookDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddBookDlg)

public:
	CAddBookDlg(CWnd* pParent = NULL);   // 标准构造函数
	CAddBookDlg(CBookMngDlg* dlg);
	virtual ~CAddBookDlg();

// 对话框数据
	enum { IDD = IDD_ADD_BOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void SetModifyInit();
	void SetNotifyWnd(CBookMngDlg* dlg);
	void SendToJudgeBook(CString strName);
	void SendToAddOrMdfBook(EM_SOCK_CMD type=SOCK_CMD_ADD_BOOK);
	bool m_bAdd;
	int m_ndex;//当为修改时，代表m_vet的索引s
	CBookMngDlg* m_dlg;
	CComboBox m_ComboZy;
	CDateTimeCtrl m_date;
	afx_msg void OnDtnDatetimechangeXdDate(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_comboRk;
	BOOK_STU m_bookstu;
	CEditSet m_edit_yz;//印张
	CEditSet m_edit_ls;//令数
};
