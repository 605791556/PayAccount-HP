#pragma once
#include <vector>

using std::vector;

#define IDC_COMBOBOX WM_USER+100

struct WCK_LINE
{
	int nItem;
	CComboBox* pComboProject;
	int nComboxProID;
	CComboBox* pComboBook;
	int nComboxBookID;
	CEdit*     pEdit;
	int nEidtID;

	WCK_LINE()
	{
		pComboProject = NULL;
		pComboBook = NULL;
		pEdit = NULL;
	}
};

class CProcessDlg;
class CWkcListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CWkcListCtrl)

public:
	CWkcListCtrl();
	virtual ~CWkcListCtrl();
	virtual BOOL OnInitDialog();
	void SetNotifyWnd(CProcessDlg* pdata){ pThis = pdata; }
	void UpdateVdata();
	void SetComboProjectValue(CComboBox* pComboProject,CString strVdex);
	void SetComboBookValue(CComboBox* pComboBook,CString strVdex);
	afx_msg void OnSelchangeCombo(UINT nID);
	afx_msg void OnSelchangeEdit(UINT nID);
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()

public:
	vector<PROJECT_STU> m_vProjects;
	vector<BOOK_STU> m_vBooks;
	vector<WCK_LINE> m_ListLine;
	long m_aDex;
	CProcessDlg* pThis;
	
};


