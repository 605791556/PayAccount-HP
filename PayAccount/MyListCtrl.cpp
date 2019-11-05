// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "MyListCtrl.h"
#include "DayCheckDlg.h"


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{
	m_aDex = 0;
	pThis = NULL;
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_DRAWITEM()
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBOBOX, IDC_COMBOBOX + 1000, OnSelchangeCombo)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_COMBOBOX, IDC_COMBOBOX + 1000, OnSelchangeEdit)
END_MESSAGE_MAP()



// CMyListCtrl 消息处理程序
BOOL CMyListCtrl::OnInitDialog()
{
	TCHAR rgtsz[7][10] = { _T("序号"), _T("项目"), _T("书名"), _T("单价"), _T("件数"),_T("金额(￥)"), _T("ID") };

	LV_COLUMN lvcolumn;
	CRect rect;
	GetWindowRect(&rect);
	for (int i = 0; i < 7; i++)
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
		else if (i == 3)
		{
			lvcolumn.cx = 100;
		}
		else if (i == 4)
		{
			lvcolumn.cx = 80;
		}
		else if (i == 5)
		{
			lvcolumn.cx = 100;
		}
		else if (i == 6)
		{
			lvcolumn.cx = 0;
		}
		else
		{
			lvcolumn.cx = (rect.Width() -80-40-100*2-3) / 2;
		}
		InsertColumn(i, &lvcolumn);
	}
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SetBkColor(RGB(190 ,190, 190));
	SetTextBkColor(RGB(190, 190, 190));

	CImageList imagelist;
	imagelist.Create(1, 25, ILC_COLOR, 1, 1);
	SetImageList(&imagelist, LVSIL_SMALL);
	UpdateData(FALSE);

	return TRUE;
}

void CMyListCtrl::SetComboProjectValue(CComboBox* pComboProject,CString strVdex)
{
	if (pComboProject)
	{
		if (strVdex.IsEmpty())
		{
			int nSize = pThis->m_vProjects.size();
			for (int i = 0; i < nSize; i++)
			{
				pComboProject->InsertString(i,pThis->m_vProjects[i].strName);
				pComboProject->SetItemData(i,(DWORD_PTR)&pThis->m_vProjects[i].nID);
			}
			if(nSize>0)
				pComboProject->SetCurSel(0);
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

void CMyListCtrl::SetComboBookValue(CComboBox* pComboBook, CString strVdex)
{
	if (pComboBook)
	{
		if (strVdex.IsEmpty())
		{
			int nSize = pThis->m_vBooks.size();
			for (int i = 0; i < nSize; i++)
			{
				pComboBook->InsertString(i,pThis->m_vBooks[i].strname);
				pComboBook->SetItemData(i,(DWORD_PTR)&pThis->m_vBooks[i].strBookID);
			}
			if(nSize>0)
				pComboBook->SetCurSel(0);
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

LRESULT CMyListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lret = CListCtrl::DefWindowProc(message, wParam, lParam);
	LVITEM* pItem = NULL;
	RECT rectItem;

	STU_LINE  StuLine;
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
		StuLine.pEdit = new CEditSet;
		StuLine.pEdit->m_type = EDIT_TYPE_FLOAT;
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

			m_ListLine[nItem].pComboProject->Create(CBS_DROPDOWNLIST  | CBS_DISABLENOSCROLL | WS_VSCROLL, rectItem, this, IDC_COMBOBOX + (++m_aDex));
			m_ListLine[nItem].nComboxProID = IDC_COMBOBOX + m_aDex;
			m_ListLine[nItem].pComboProject->ShowWindow(SW_SHOW);
			SetComboProjectValue(m_ListLine[nItem].pComboProject, pItem->pszText);
		}
		else if (pItem->iSubItem == 2)
		{//书名
			rectItem.left += 2;
			rectItem.top += 0;
			rectItem.right -= 2;
			m_ListLine[nItem].pComboBook->Create(CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL, rectItem, this, IDC_COMBOBOX + (++m_aDex));
			m_ListLine[nItem].nComboxBookID = IDC_COMBOBOX + m_aDex;
			m_ListLine[nItem].pComboBook->ShowWindow(SW_SHOW);
			SetComboBookValue(m_ListLine[nItem].pComboBook, pItem->pszText);
		}
		else if (pItem->iSubItem == 3)
		{//单价
			CString strText = pItem->pszText;
		}
		else if (pItem->iSubItem == 4)
		{//件数
			rectItem.left += 2;
			rectItem.top += 2;
			rectItem.right -= 2;
			rectItem.bottom -= 2;
			m_ListLine[nItem].pEdit->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP  | ES_CENTER, rectItem, this, IDC_COMBOBOX + (++m_aDex));
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

void CMyListCtrl::GetPay(int nItem)
{
	CComboBox* pComboProject = NULL;
	CComboBox* pComboBook = NULL;

	CString strPay;

	int nNum = m_ListLine.size();
	if (nItem>=nNum)
	{
		return;
	}
	pComboProject = m_ListLine[nItem].pComboProject;
	pComboBook = m_ListLine[nItem].pComboBook;
	if (pComboProject && pComboBook)
	{
		int proID;
		int ndex = pComboProject->GetCurSel();
		if (ndex >= 0)
		{
			proID = *((int*)pComboProject->GetItemData(ndex));
		}

		CString strBookID = L"";
		ndex= pComboBook->GetCurSel();
		if (ndex >= 0)
		{
			strBookID = *((CString*)pComboBook->GetItemData(ndex));
		}

		if (!strBookID.IsEmpty())
		{
			pThis->SendToGetOnePay(pThis->m_strStaffID,proID, strBookID,nItem);
		}
	}
}

void CMyListCtrl::OnSelchangeCombo(UINT nID)
{
	int nSize = m_ListLine.size();
	for (int i = 0; i < nSize;i++)
	{
		STU_LINE line = m_ListLine[i];
		if (line.nComboxBookID == nID || line.nComboxProID == nID)
		{
			int proID;
			int ndex = line.pComboProject->GetCurSel();
			if (ndex >= 0)
			{
				proID = *((int*)line.pComboProject->GetItemData(ndex));
			}
			
			CString strBookID = L"";
			ndex= line.pComboBook->GetCurSel();
			if (ndex >= 0)
			{
				strBookID = *((CString*)line.pComboBook->GetItemData(ndex));
			}

			if (!strBookID.IsEmpty())
			{
				pThis->SendToGetOnePay(pThis->m_strStaffID,proID, strBookID,line.nItem);
			}
			else
			{
				SetItemText(i, 3, L"");
				m_ListLine[i].money = 0;
				SetItemText(i, 5, L"");
			}
		}
	}
}

void CMyListCtrl::OnSelchangeEdit(UINT nID)
{
	int nSize = m_ListLine.size();
	for (int i = 0; i < nSize; i++)
	{
		STU_LINE line = m_ListLine[i];
		if (line.nEidtID == nID)
		{
			CString strPay = GetItemText(i, 3);
			CString strNum;
			line.pEdit->GetWindowText(strNum);
			double num = _ttof(strNum);
			double pay = _ttof(strPay);
			double all = num * pay;
			m_ListLine[i].money = all;
			CString strAll;
			strAll.Format(L"%.04f", all);
			SetItemText(i, 5, strAll);
		}
	}
	double allMoney = 0;
	for (int i = 0; i < nSize; i++)
	{
		allMoney += m_ListLine[i].money;
	}
	//pThis->SetAllPayCtrl(DAYPAY_TYPE_JIJIAN,allMoney);
	pThis->SetAllPayCtrl();
}