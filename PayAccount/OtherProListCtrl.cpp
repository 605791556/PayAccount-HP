// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "OtherProListCtrl.h"
#include "OtherProDlg.h"

// COtherProListCtrl

IMPLEMENT_DYNAMIC(COtherProListCtrl, CListCtrl)

COtherProListCtrl::COtherProListCtrl()
{
	
}

COtherProListCtrl::~COtherProListCtrl()
{
}


BEGIN_MESSAGE_MAP(COtherProListCtrl, CListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


void COtherProListCtrl::InitListCtrl()
{
	TCHAR rgtsz[3][10] = { _T("序号"), _T("书名"), _T("单价") };

	LV_COLUMN lvcolumn;
	CRect rect;
	GetWindowRect(&rect);
	for (int i = 0; i < 3; i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = HDF_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i == 0)
		{
			lvcolumn.cx = 50;
		}
		else if (i ==2)
		{
			lvcolumn.cx = 80;
		}
		else
		{
			lvcolumn.cx = rect.Width() - 130-25;
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
}

void COtherProListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pThis)
		pThis->m_edit.ShowWindow(SW_HIDE);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}
void COtherProListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pThis)
	   pThis->m_edit.ShowWindow(SW_HIDE);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}