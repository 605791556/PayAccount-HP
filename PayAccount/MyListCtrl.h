#pragma once
#include <vector>
#include "..\common\EditSet.h"

using std::vector;

#define IDC_COMBOBOX WM_USER+100
// CMyListCtrl
struct STU_LINE
{
	int nItem;
	CComboBox* pComboProject;
	int nComboxProID;
	CComboBox* pComboBook;
	int nComboxBookID;
	CEditSet*     pEdit;
	int nEidtID;
	double money;

	STU_LINE()
	{
		pComboProject = NULL;
		pComboBook = NULL;
		pEdit = NULL;
		money = 0;
	}
};

class CDayCheckDlg;
class CMyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMyListCtrl)

public:
	CMyListCtrl();
	virtual ~CMyListCtrl();
	virtual BOOL OnInitDialog();
	void SetNotifyWnd(CDayCheckDlg* pdata){ pThis = pdata; }
	void SetComboProjectValue(CComboBox* pComboProject,CString strVdex);
	void SetComboBookValue(CComboBox* pComboBook,CString strVdex);
	void GetPay(int nItem);
	afx_msg void OnSelchangeCombo(UINT nID);
	afx_msg void OnSelchangeEdit(UINT nID);
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()

public:
	vector<STU_LINE> m_ListLine;
	long m_aDex;
	CDayCheckDlg* pThis;
	
};


