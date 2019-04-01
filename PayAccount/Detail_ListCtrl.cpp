// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "Detail_ListCtrl.h"
#include "DetailDlg.h"


// CDtlListCtrl

IMPLEMENT_DYNAMIC(CDtlListCtrl, CListCtrl)

CDtlListCtrl::CDtlListCtrl()
{
	m_aDex = 0;
	pThis = NULL;
}

CDtlListCtrl::~CDtlListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDtlListCtrl, CListCtrl)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


LRESULT CDtlListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lret = CListCtrl::DefWindowProc(message, wParam, lParam);
	LVITEM* pItem = NULL;
	RECT rectItem;

	DTL_LINE  StuLine;
	HWND hCtrl = NULL;
	CRect rect;
	static int nItem = -1;
	switch (message)
	{
	case LVM_INSERTITEM:
		pItem = (LVITEM*)lParam;
		nItem = pItem->iItem;
		
		StuLine.nItem = nItem;
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
		{
		}
		else if (pItem->iSubItem == 2)
		{
		}
		else if (pItem->iSubItem == 3)
		{
		}
		break;
	case LVM_DELETEITEM:
		if (nItem>=0)
		{
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
		}
		m_ListLine.clear();
		nItem = -1;
		m_aDex = 0;
		break;
	}
	return lret;
}