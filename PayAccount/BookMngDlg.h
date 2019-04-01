#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CBookMngDlg 对话框

#define  WM_BOOKMNG_CALL WM_USER+701

class CBookMngDlg : public CDialog
{
	DECLARE_DYNAMIC(CBookMngDlg)

public:
	CBookMngDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBookMngDlg();

// 对话框数据
	enum { IDD = IDD_BOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	int m_nItem;
	int m_nCount;//数据总数
	int m_dex;//当前页数
	int m_nPage;//总页数

	CListCtrl m_ListCtrl;
	afx_msg void DelBook();
	afx_msg void MdfBook();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	void SetListCtrlValue();
	void SetListFontSize();
	void FindBook(int pageDex);
	vector<BOOK_STU> m_vet;
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox m_comboRk;
	CStatic m_sta_all;
	CStatic m_sta_go;
	CEdit m_edit_go;
	CButton m_btn_go;
	CButton m_BtnLast;
	CButton m_BtnNext;
	afx_msg void OnBnClickedBtnGo();
	afx_msg void OnBnClickedBtnLast();
	afx_msg void OnBnClickedBtnNext();
	void SendToGetBook(CString strKeyWord,BOOK_RK rkType,EM_DATE_TYPE date_Type,int nStart,int nNum);
	void SendToDelBook(CString strBookID);
	void GetBook(Json::Value root);
	//比较两个时间字符串的先后顺序strDate1<strDate2,返回值小于0,,相等返回0，否则大于0
	int JudgeDateStringSize(CString strDate1,CString strDate2);
	CComboBox m_comboDate;
	BOOK_RK m_rkType;
	EM_DATE_TYPE m_date_Type;
	afx_msg void OnCbnSelchangeComboDate();
};
