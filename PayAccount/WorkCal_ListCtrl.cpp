// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "WorkCal_ListCtrl.h"
#include "ProcessDlg.h"


// CWkcListCtrl

IMPLEMENT_DYNAMIC(CWkcListCtrl, CListCtrl)

CWkcListCtrl::CWkcListCtrl()
{
	m_aDex = 0;
	pThis = NULL;
}

CWkcListCtrl::~CWkcListCtrl()
{
}


BEGIN_MESSAGE_MAP(CWkcListCtrl, CListCtrl)
	ON_WM_DRAWITEM()
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBOBOX, IDC_COMBOBOX + 1000, OnSelchangeCombo)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_COMBOBOX, IDC_COMBOBOX + 1000, OnSelchangeEdit)
END_MESSAGE_MAP()



// CWkcListCtrl 消息处理程序
BOOL CWkcListCtrl::OnInitDialog()
{
	TCHAR rgtsz[5][10] = { _T("序号"), _T("项目"), _T("书名"), _T("件数"), _T("时间")};

	LV_COLUMN lvcolumn;
	CRect rect;
	GetWindowRect(&rect);
	for (int i = 0; i < 5; i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = HDF_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i == 0 )
		{
			lvcolumn.cx = 40;
		}
		else if (i == 1||i==2)
		{
			lvcolumn.cx = 150;
		}
		else
		{
			lvcolumn.cx = 80;
		}
		InsertColumn(i, &lvcolumn);
	}
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SetBkColor(RGB(102, 205, 170));
	SetTextBkColor(RGB(102, 205, 170));

	CImageList imagelist;
	imagelist.Create(1, 25, ILC_COLOR, 1, 1);
	SetImageList(&imagelist, LVSIL_SMALL);
	UpdateData(FALSE);

	UpdateVdata();
	return TRUE;
}

void CWkcListCtrl::UpdateVdata()
{
	m_vBooks.clear();
	int nCount;
	m_vBooks = theApp.m_dbData->GetBook(L"",BOOK_RK_NO,nCount,0,1000);

	m_vProjects.clear();
	m_vProjects = theApp.m_dbData->GetProjectList();
}

void CWkcListCtrl::SetComboProjectValue(CComboBox* pComboProject,CString strVdex)
{
	if (pComboProject)
	{
		if (strVdex.IsEmpty())
		{
			int nSize = m_vProjects.size();
			for (int i = 0; i < nSize; i++)
			{
				pComboProject->InsertString(i,m_vProjects[i].strName);
				pComboProject->SetItemData(i,(DWORD_PTR)&m_vProjects[i].nID);
			}
			if (nSize>0)
			{
				pComboProject->SetCurSel(0);
			}
		}
		else
		{
			//表示已保存的记录
			int n = _ttoi(strVdex);
			pComboProject->InsertString(0,pThis->m_vCal[n].strProName);
			pComboProject->SetItemData(0,(DWORD_PTR)&pThis->m_vCal[n].proID);
			pComboProject->SetCurSel(0);
		}
	}
}

void CWkcListCtrl::SetComboBookValue(CComboBox* pComboBook, CString strVdex)
{
	USES_CONVERSION;
	if (pComboBook)
	{
		if (strVdex.IsEmpty())
		{
			//表示添加
			int nSize = m_vBooks.size();
			for (int i = 0; i < nSize; i++)
			{
				pComboBook->InsertString(i,m_vBooks[i].strname);
				pComboBook->SetItemData(i,(DWORD_PTR)&m_vBooks[i].strBookID);
			}
			if (nSize>0)
			{
				pComboBook->SetCurSel(0);
			}
		}
		else
		{
			//表示已保存的记录
			int n = _ttoi(strVdex);
			pComboBook->InsertString(0,pThis->m_vCal[n].strBookName);
			pComboBook->SetItemData(0,(DWORD_PTR)&pThis->m_vCal[n].strBookID);
			pComboBook->SetCurSel(0);
		}
	}
}

LRESULT CWkcListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lret = CListCtrl::DefWindowProc(message, wParam, lParam);
	LVITEM* pItem = NULL;
	RECT rectItem;

	WCK_LINE  StuLine;
	HWND hCtrl = NULL;
	CRect rect;
	static int nItem = -1;
	switch (message)
	{
	case LVM_INSERTITEM:
		pItem = (LVITEM*)lParam;
		nItem = pItem->iItem;
		
		StuLine.nItem = nItem;
		StuLine.pComboBook = new CComboBox();
		StuLine.pComboProject = new CComboBox();
		StuLine.pEdit = new CEdit;
		m_ListLine.push_back(StuLine);
		break;
	case LVM_SETITEMTEXT:
		pItem = (LVITEM*)lParam;
		StuLine = m_ListLine[nItem];

		GetSubItemRect(nItem, pItem->iSubItem, LVIR_LABEL, rect);
		rectItem.left = rect.left;
		rectItem.top = rect.top;
		rectItem.right = rect.right;
		rectItem.bottom = rect.bottom;

		if (pItem->iSubItem == 1)
		{//项目
			rectItem.left += 2;
			rectItem.top += 0;
			rectItem.right -= 2;
			rectItem.bottom += 150;
			m_ListLine[nItem].pComboProject->Create(CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL, rectItem, this, IDC_COMBOBOX + (++m_aDex));
			m_ListLine[nItem].nComboxProID = IDC_COMBOBOX + m_aDex;
			m_ListLine[nItem].pComboProject->ShowWindow(SW_SHOW);
			SetComboProjectValue(m_ListLine[nItem].pComboProject, pItem->pszText);
		}
		else if (pItem->iSubItem == 2)
		{//书
			rectItem.left += 2;
			rectItem.top += 0;
			rectItem.right -= 2;
			rectItem.bottom += 180;
			m_ListLine[nItem].pComboBook->Create(CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL, rectItem, this, IDC_COMBOBOX + (++m_aDex));
			m_ListLine[nItem].nComboxBookID = IDC_COMBOBOX + m_aDex;
			m_ListLine[nItem].pComboBook->ShowWindow(SW_SHOW);
			SetComboBookValue(m_ListLine[nItem].pComboBook, pItem->pszText);
		}
		else if (pItem->iSubItem == 3)
		{//件数
			rectItem.left += 2;
			rectItem.top += 2;
			rectItem.right -= 2;
			rectItem.bottom -= 2;
			m_ListLine[nItem].pEdit->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_CENTER, rectItem, this, IDC_COMBOBOX + (++m_aDex));
			m_ListLine[nItem].nEidtID = IDC_COMBOBOX + m_aDex;
			m_ListLine[nItem].pEdit->SetWindowText(pItem->pszText);
		}
		break;
	case LVM_DELETEITEM:
		if (nItem>=0)
		{
			//StuLine = m_ListLine[nItem];
			m_ListLine[nItem].pComboProject->DestroyWindow();
			m_ListLine[nItem].pComboBook->DestroyWindow();
			m_ListLine[nItem].pEdit->DestroyWindow();
			delete m_ListLine[nItem].pComboProject;
			delete m_ListLine[nItem].pComboBook;
			delete m_ListLine[nItem].pEdit;
			m_ListLine.erase(m_ListLine.begin() + nItem);
			nItem--;
		}
		else
		{
			m_aDex = 0;
		}
		break;
	case LVM_DELETEALLITEMS: //删除所有
		for (int i = 0; i < m_ListLine.size();i++)
		{
			m_ListLine[i].pComboProject->DestroyWindow();
			m_ListLine[i].pComboBook->DestroyWindow();
			m_ListLine[i].pEdit->DestroyWindow();
			delete m_ListLine[i].pComboProject;
			delete m_ListLine[i].pComboBook;
			delete m_ListLine[i].pEdit;
		}
		m_ListLine.clear();
		nItem = -1;
		m_aDex = 0;
		break;
	}
	return lret;
}

void CWkcListCtrl::OnSelchangeCombo(UINT nID)
{
	CString strStaffID=L"";
	
	int ndex = pThis->m_staff.GetCurSel();
	if (ndex>=0)
	{
		strStaffID = *((CString*)pThis->m_staff.GetItemData(ndex));
	}

	int nSize = m_ListLine.size();
	for (int i = 0; i < nSize;i++)
	{
		WCK_LINE line = m_ListLine[i];
		if (line.nComboxBookID == nID || line.nComboxProID == nID)
		{
			int proID;
			ndex = line.pComboProject->GetCurSel();
			if (ndex >= 0)
			{
				proID = *((int*)line.pComboProject->GetItemData(ndex));
			}
			CString strBookID = L"";
			int ndex= line.pComboBook->GetCurSel();
			if (ndex >= 0)
			{
				strBookID = *((CString*)line.pComboBook->GetItemData(ndex));
			}

			if (!strBookID.IsEmpty() && !strStaffID.IsEmpty())
			{
				
			}
			else
			{
				
			}
		}
	}
}

void CWkcListCtrl::OnSelchangeEdit(UINT nID)
{
	
}